@ payload.s
@
@ Copyright (c) 2021-2022 @dora2ios
@
@ This program is free software: you can redistribute it and/or modify
@ it under the terms of the GNU General Public License as published by
@ the Free Software Foundation, either version 3 of the License, or
@ (at your option) any later version.
@
@ This program is distributed in the hope that it will be useful,
@ but WITHOUT ANY WARRANTY; without even the implied warranty of
@ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
@ GNU General Public License for more details.
@
@ You should have received a copy of the GNU General Public License
@ along with this program.  If not, see <http://www.gnu.org/licenses/>.
@


@ Build version : 2.0
@ iBoot version : iBoot-1940.10.58
@ Device        : N42AP

.set    JUMP_ADDRESS_PTR,   0xbff43540  @ end point of payload
.set    IMAGE3_NEW_TYPE,    0x69626f62  @ 'ibob' : new iBoot TYPE

    .text
    .syntax unified

    .arm
_entry:
    b   _entry


    .org    0x844
    .thumb
    .thumb_func
_find_boot_images:
    bx    lr

    .org    0xc18
    .thumb
    .thumb_func
_back_to_main:
    bx    lr

    .org    0x1f7a4
    .thumb
    .thumb_func
_platform_init:
    bx    lr

    .org    0x206a0
    .thumb
    .thumb_func
_prepare_and_jump:
    bx    lr

    .org    0x257c0
    .thumb
    .thumb_func
_image_load_type:
    bx    lr

    .org    0x34ea0
    .thumb
    .thumb_func
_disable_interrupts:
    bx    lr


    .org    0x43240
    .global _shook
    .thumb
    .thumb_func
_shook:
    bl         _payload
    ldr        r0, =0xbff35068             @ "main"
    movs       r2, #0x0                    @ NULL
    ldr        r1, =0xbff00c65             @ main_task
    mov.w      r3, #0x1c00                 @ size
    b.w        _back_to_main               @ go back and enter recovery mode


_payload:
    push       {r7, lr}
    add        r7, sp, #0x0

_payload$check_home_btn:
    movs       r1, #0x0                 @ homebtn offset
    movt       r1, #0x3fa0              @ gpioBase
    ldr        r0, [r1]
    tst.w      r0, #0x1
    beq.n      _payload$patch


_payload$image_load:                    @ find iboot image and extract it into jumpaddr
    bl         _platform_init
    bl         _find_boot_images

    ldr        r0, =JUMP_ADDRESS_PTR
    adds       r1, r0, #0x4
    mov.w      r2, #0x84000000          @ jumpaddr
    str        r2, [r0]
    mov.w      r2, #0x100000            @ maxsize
    str        r2, [r1]
    ldr        r2, =IMAGE3_NEW_TYPE     @ TYPE
    bl         _image_load_type         @ _image_load_type(*ptr, *sz, type)
    cmp        r0, #0x0
    beq.n      _payload$jump_to


_payload$patch:                         @ apply patches for pwned recovery mode
_payload$patch$force_recovery:
    ldr        r1, =0xbff00e54
    mov        r2, #0x2400
    strh       r2, [r1]
_payload$patch$enable_setenv:
    ldr        r1, =0xbff18d5e
    mov.w      r2, #0x20002000
    str        r2, [r1]
_payload$patch$cmd_endpoint:
    ldr        r1, =0xbff20e08
    ldr        r2, =0xbff446b8
    str        r2, [r1]
_payload$patch$cmd_regist:
    subs       r1, r2, #4               @ endpoint - 4
    ldr        r2, =JUMP_ADDRESS_PTR
    subs       r2, r2, #0x20            @ exec_cmd_start, cmd_name
    str        r2, [r1]
    adds       r3, r2, #0x10            @ exec_cmd_start + 0x10 = cmd_name_1
    str        r3, [r2]
    adds       r2, r2, #4               @ cmd_func
    mov.w      r1, #0x80000000          @ load_address
    str        r1, [r2]
    adds       r2, r2, #4               @ cmd_help
    adds       r1, r2, #0xc             @ '\x00'
    str        r1, [r2]
    adds       r2, r2, #4               @ cmd_meta
    movs       r0, #0
    str        r0, [r2]
    ldr        r2, =0x63657865          @ 'exec'
    str        r2, [r3]
    str        r0, [r1]

    pop        {r7, pc}                 @ move back to _main_task


_payload$jump_to:
    bl         _disable_interrupts
    movs       r0, #0x2                 @ BOOT_IBOOT
    mov.w      r1, #0x84000000          @ ptr
    movs       r2, #0x0                 @ args
    movs       r3, #0x0
    bl         _prepare_and_jump        @ _prepare_and_jump(BOOT_IBOOT, jumpaddr, 0, 0)
_loop:
    b.n        _loop                    @ never return
