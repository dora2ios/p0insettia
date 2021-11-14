@ shellcode.s
@
@ copyright (C) 2021/11/14 sauRdev
@
@  Permission is hereby granted, free of charge, to any person obtaining a copy
@  of this software and associated documentation files (the "Software"), to deal
@  in the Software without restriction, including without limitation the rights
@  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
@  copies of the Software, and to permit persons to whom the Software is
@  furnished to do so, subject to the following conditions:
@
@  The above copyright notice and this permission notice shall be included in all
@  copies or substantial portions of the Software.
@
@  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
@  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
@  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
@  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
@  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
@  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
@  SOFTWARE.

    .text
    .syntax unified

    .global _payload
    .thumb
    .thumb_func
_payload:
    b.w     _amfi_execve_hook
    b.w     _pre_hook

_amfi_execve_hook:              @ makes sure amfi doesn't try to kill our binaries
    ldr.w   r0, [sl]            @ cs_flags
    orr     r0, r0, #0x4000000  @ CS_PLATFORM_BINARY
    orr     r0, r0, #0x000f     @ CS_VALID | CS_ADHOC | CS_GET_TASK_ALLOW | CS_INSTALLER
    bic     r0, r0, #0x3f00     @ clearing CS_HARD | CS_KILL | CS_CHECK_EXPIRATION | CS_RESTRICT | CS_ENFORCEMENT | CS_REQUIRE_LV
    str.w   r0, [sl]
    movs    r0, #0x0
    add     sp, #0x18
    pop.w   {r8, sl, fp}
    pop     {r4, r5, r6, r7, pc}

_pre_hook:
    cbz     r3, _to_orig
    b.n     _pre_execve_hook
_to_orig:
    b.n     _mpo_cred_label_update_execve.stub

_pre_execve_hook:
    push    {r4, r5, r6, r7, lr}
    add     r7, sp, #0xc
    push.w  {r8, sl, fp}
    sub     sp, #0x1d8
    mov     r4, sp
    bfc     r4, #0x0, #0x3
    mov     sp, r4
    ldr.w   sb, [r7, #0xc]
    ldr.w   ip, [r7, #0x8]
    ldr.w   lr, [r7, #0x2c]
    ldr     r4, [r7, #0x28]
    ldr     r5, [r7, #0x24]
    ldr     r6, [r7, #0x20]
    ldr.w   r8, [r7, #0x1c]
    ldr.w   sl, [r7, #0x18]
    ldr.w   fp, [r7, #0x14]
    str     r0, [sp, #0x68]
    ldr     r0, [r7, #0x10]
    str     r0, [sp, #0x64]
    ldr     r0, [sp, #0x68]
    str     r0, [sp, #0x60]
    add     r0, sp, #0x74
    str     r0, [sp, #0x5c]
    movs    r0, #0x0
    str     r0, [sp, #0x58]
    ldr     r0, [sp, #0x60]
    str     r0, [sp, #0x1d4]
    str     r1, [sp, #0x1d0]
    str     r2, [sp, #0x1cc]
    str     r3, [sp, #0x1c8]
    str.w   sb, [sp, #0x1c4]
    str.w   ip, [sp, #0x1c0]
    str.w   fp, [sp, #0x54]
    str.w   r8, [sp, #0x50]
    str.w   sl, [sp, #0x4c]
    str     r6, [sp, #0x48]
    str     r5, [sp, #0x44]
    str.w   lr, [sp, #0x40]
    str     r4, [sp, #0x3c]
    bl      _vfs_context_current.stub   @ getting current vfs context
    str     r0, [sp, #0x70]
    movs    r0, #0x0
    str     r0, [sp, #0x78]
    str     r0, [sp, #0x74]
    str     r0, [sp, #0x80]
    mov.w   r0, #0x380
    str     r0, [sp, #0x7c]
    ldr     r0, [sp, #0x58]
    str     r0, [sp, #0x84]
    ldr     r0, [sp, #0x1c8]
    ldr     r2, [sp, #0x70]
    ldr     r1, [sp, #0x5c]
    bl      _vnode_getattr.stub         @ getting vnode attributes
    str     r0, [sp, #0x1bc]
    ldr     r0, [sp, #0x1bc]
    cmp     r0, #0x0
    bne.n   _pre_execve_hook$orig

    movs    r0, #0x0
    str     r0, [sp, #0x6c]             @ mark
    ldrh.w  r0, [sp, #0xc0]             @ va_mode
    and     r0, r0, #0x800              @ test for setuid bit
    cmp     r0, #0x0
    beq.n   _pre_execve_hook$orig$gid

    movs    r0, #0x1
    ldr     r1, [sp, #0xb8]             @ va_uid
    ldr     r2, [sp, #0x1d0]            @ new_cred->uid
    str     r1, [r2, #0xc]              @ patch
    str     r0, [sp, #0x6c]             @ mark this as having been setuid or setgid

_pre_execve_hook$orig$gid:
    ldrh.w  r0, [sp, #0xc0]             @ va_mode
    and     r0, r0, #0x400              @ test for setgid bit
    cmp     r0, #0x0
    beq.n   _pre_execve_hook$orig$p_flags

    movs    r0, #0x1
    ldr     r1, [sp, #0xbc]             @ va_gid
    ldr     r2, [sp, #0x1d0]            @ new_cred->gid
    str     r1, [r2, #0x1c]             @ patch
    str     r0, [sp, #0x6c]             @ mark this as having been setuid or setgid

_pre_execve_hook$orig$p_flags:
    ldr     r0, [sp, #0x6c]
    cmp     r0, #0x0
    beq.n   _pre_execve_hook$orig

    ldr     r0, [sp, #0x1cc]
    ldr.w   r0, [r0, #0xbc]             @ p->p_flag
    orr     r0, r0, #0x100              @ add P_SUGID
    ldr     r1, [sp, #0x1cc]
    str.w   r0, [r1, #0xbc]

_pre_execve_hook$orig:
    ldr     r0, [sp, #0x1d4]
    ldr     r1, [sp, #0x1d0]
    ldr     r2, [sp, #0x1cc]
    ldr     r3, [sp, #0x1c8]
    ldr.w   sb, [sp, #0x1c0]
    ldr.w   ip, [sp, #0x1c4]
    ldr.w   lr, [r7, #0x10]
    ldr     r4, [r7, #0x14]
    ldr     r5, [r7, #0x18]
    ldr     r6, [r7, #0x1c]
    ldr.w   r8, [r7, #0x20]
    ldr.w   sl, [r7, #0x24]
    ldr.w   fp, [r7, #0x28]
    str     r0, [sp, #0x38]
    ldr     r0, [r7, #0x2c]
    str     r0, [sp, #0x34]
    mov     r0, sp
    str     r0, [sp, #0x30]
    ldr     r0, [sp, #0x34]
    str     r1, [sp, #0x2c]
    ldr     r1, [sp, #0x30]
    str     r0, [r1, #0x24]
    str.w   fp, [r1, #0x20]
    str.w   sl, [r1, #0x1c]
    str.w   r8, [r1, #0x18]
    str     r6, [r1, #0x14]
    str     r5, [r1, #0x10]
    str     r4, [r1, #0xc]
    str.w   lr, [r1, #0x8]
    str.w   ip, [r1, #0x4]
    str.w   sb, [r1]
    ldr     r0, [sp, #0x38]
    ldr     r1, [sp, #0x2c]
    bl      _mpo_cred_label_update_execve.stub
    sub.w   r4, r7, #0x18
    mov     sp, r4
    pop.w   {r8, sl, fp}
    pop     {r4, r5, r6, r7, pc}

_vfs_context_current.stub:
    movw    r0, #0x20
    movt    r0, #0x0
    add     r0, pc
    ldr     r0, [r0]
    bx      r0

_vnode_getattr.stub:
    movw    r3, #0x16
    movt    r3, #0x0
    add     r3, pc
    ldr     r3, [r3]
    bx      r3

_mpo_cred_label_update_execve.stub:
    movw    sb, #0xc
    movt    sb, #0x0
    add     sb, pc
    ldr.w   ip, [sb]
    bx      ip

_vfs_context_current:
.long       0x41414141

_vnode_getattr:
.long       0x42424242

_mpo_cred_label_update_execve:
.long       0x43434343


_ret0_gadgat:
    movs    r0, #0
    bx      lr

_ret1_gadgat:
    movs    r0, #1
    bx      lr

.align    2
