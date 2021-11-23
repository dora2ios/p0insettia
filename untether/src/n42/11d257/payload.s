@ payload.s
@
@ Copyright (c) 2021 @dora2ios
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


.set    JUMP_ADDRESS_PTR,   0xbff43300  @ end point of payload
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


    .org    0xbf8
    .thumb
    .thumb_func
_main_generic:
    bx    lr

    .org    0xc18
    .thumb
    .thumb_func
_back_to_main:
    @bl   _task_create                      @ r0 = task_create("main", main_task, NULL, 0x1C00);
    @bl   _task_start                       @ task_start(r0);
    @movs r0, #0x0
    @bl   _task_exit                        @ task_exit(0);
    bx    lr

    .org    0x1f2a4
    .thumb
    .thumb_func
_platform_early_init:
    bx    lr



    .org    0x1f7a4
    .thumb
    .thumb_func
_platform_init:
    bx    lr


    .org    0x2049c
    .thumb
    .thumb_func
_platform_watchdog_tickle:
    bx    lr


    .org    0x206a0
    .thumb
    .thumb_func
_prepare_and_jump:
    bx    lr


    .org    0x20a54
    .thumb
    .thumb_func
_sys_init_stack_cookie:
    bx    lr


    .org    0x21084
    .thumb
    .thumb_func
_task_create:
    bx    lr


    .org    0x211e4
    .thumb
    .thumb_func
_task_start:
    bx    lr


    .org    0x21208
    .thumb
    .thumb_func
_task_exit:
    bx    lr


    .org    0x227b4
    .thumb
    .thumb_func
_sys_init:
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
    mov.w      r3, #0x1c00                 @ 0x1C00
    b.w        _back_to_main

_payload:
    push       {r7, lr}
    add        r7, sp, #0x0

    bl         _platform_init
    bl         _find_boot_images

    ldr        r0, =JUMP_ADDRESS_PTR
    adds       r1, r0, #0x4
    mov.w      r2, #0x84000000
    str        r2, [r0]
    mov.w      r2, #0x100000
    str        r2, [r1]
    ldr        r2, =IMAGE3_NEW_TYPE
    bl         _image_load_type         @ _image_load_type(*ptr, *sz, type)
    cmp        r0, #0x0
    beq.n      _jump_to

@_error:
    @ldr       r3, =0xbff026f6
    @ldr       r2, =0xbf00bf00
    @str       r2, [r3]
    @adds      r3, #0xe0                @ 0x27D6
    @str       r2, [r3]
    @adds      r3, #0xfa                @ 0x28d0
    @str       r2, [r3]
    @subs      r3, #0x2a                @ 0x28a6
    @ldr       r2, =0xbf002000
    @str       r2, [r3]

_enter_recovery:
    ldr        r3, =0xbff00000
    mov        r2, #0x2400
    strh       r2, [r3, #0xe54]

    @ldr       r0, =0x1ad20
    @ldr       r2, =0xfc9ef7ff
    @str       r2, [r3, r0]             @ accept only signed images

    pop        {r7, pc}                 @ move back to _main_task

_jump_to:
    bl         _disable_interrupts
    movs       r0, #0x2                 @ BOOT_IBOOT
    mov.w      r1, #0x84000000          @ ptr
    movs       r2, #0x0                 @ args
    movs       r3, #0x0
    bl         _prepare_and_jump        @ _prepare_and_jump(BOOT_IBOOT, jumpaddr, 0, 0)
