@ iboot_p1.s
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


    .text
    .syntax    unified


    .arm
_entry:
    b       _entry


    .org    0x1770
    .thumb
    .thumb_func
_sub_bff01770:
    bx      lr


    .org    0x21078
    .thumb
    .thumb_func
_get_current_task:
    bx      lr


    .org    0x22800
    .thumb_func
_arch_cpu_quiesce:
    bx      lr


    .org    0x257f0
    .thumb
    .thumb_func
_decompress_lzss:
    bx      lr


    .org    0x3444c
    .arm
_bcopy:
    bx      lr


    .org    0x34ea0
    .thumb
    .thumb_func
_disable_interrupts:
    bx      lr


    .org    0x478a0
    .thumb
    .thumb_func
_nettoyeur:
    ldr     r3, =0x840441b0
    ldr     r1, =0xbff44334

    add     r0, r3, #0x184
    str     r1, [r0]
    str     r1, [r0, #0x4]
    adds    r1, #0x38
    str     r1, [r0, #0x38]
    str     r1, [r0, #0x3c]
    adds    r0, #0x40
    adds    r1, #0x8
    str     r1, [r0]
    str     r1, [r0, #0x4]
    adds    r1, #0x4c
    str     r1, [r0, #0x2c]
    subs    r1, #0x10
    str     r1, [r0, #0x3c]
    str     r1, [r0, #0x40]
    adds    r1, #0x8
    str     r1, [r0, #0x44]
    str     r1, [r0, #0x48]
    add     r2, r0, #0x280
    add     r1, r1, #0x23c
    str     r1, [r2]
    str     r1, [r2, #0x4]

    movs    r1, 0x2
    str     r1, [r0, #0x60]

    mvn     r1, #0x0
    str     r1, [r2, #0x6c]
    str     r1, [r0, #0xc]
    str     r1, [r0, #0x10]
    subs    r2, #0xc4
    str     r1, [r2, #0x44]

    movs    r1, #0x0
    adds    r0, #0x64
_loop1:
    adds    r0, #0x4
    str     r1, [r0]
    cmp     r0, r2
    bne.n   _loop1

    add     r0, r3, #0x118
    str     r1, [r0]
    adds    r0, #0x4
    add     r2, r3, #0x17c
_loop2:
    adds    r0, #0x4
    str     r1, [r0]
    cmp     r0, r2
    bne.n   _loop2

    strb    r1, [r3]
    strb    r1, [r3, #0x10]
    strb    r1, [r3, #0x24]
    movs    r1, #0x1
    strb    r1, [r3, #0x5c]

    b       _end


    .org    0x47bb0
    .global _payload
    .thumb
    .thumb_func
_payload:
    ldr     sp, =0xbfff8000     @ move back SP
    bl      _disable_interrupts @ disable interrupts
    mov     r4, 0x84000000      @ jumpaddr

    ldr     r0, =0xbff00000     @ src
    mov     r1, r4              @ dst
    ldr     r2, =0x446c0        @ sz
    blx     _bcopy

    ldr    r0, =0xbff47a7c      @ src
    ldr    r1, =0x84043240      @ payload start
    movs   r2, #0x6c            @ sz
    blx    _bcopy

    @ apply desired patches
    ldr     r0, =0x1ad20
    ldr     r1, =0x60182000     @ movs r0  #0
                                @ str  r0, [r3]
    str     r1, [r4, r0]        @ accept unsigned images

    ldr     r1, =0xbb17f042     @ opcode
    str     r1, [r4, #0xc0e]

_get_task:
    bl      _get_current_task
    movs    r1, #0
    str     r1, [r0, #0x44]

    b.n     _nettoyeur          @ nettoyeur()

_end:                           @ quiesce the hardware
    bl      _sub_bff01770
    bl      _arch_cpu_quiesce
    bx      r4                  @ jump back to iBoot start point


.align    2



    .org    0x47bfa
    .thumb
    .thumb_func
_next:
    nop

.align    2
