//
//  unjail.h
//
//  Created by sakuRdev on 2021/11/18.
//  Copyright (c) 2021 sakuRdev. All rights reserved.
//

#ifndef unjail_h
#define unjail_h

#include <stdio.h>
#include <mach/mach.h>

#define KERNEL_TEXT_BASEADDR (0x80001000)

#define TTB_SIZE            4096
#define L1_SECT_S_BIT       (1 << 16)
#define L1_SECT_PROTO       (1 << 1)        /* 0b10 */
#define L1_SECT_AP_URW      (1 << 10) | (1 << 11)
#define L1_SECT_APX         (1 << 15)
#define L1_SECT_DEFPROT     (L1_SECT_AP_URW | L1_SECT_APX)
#define L1_SECT_SORDER      (0)            /* 0b00, not cacheable, strongly ordered. */
#define L1_SECT_DEFCACHE    (L1_SECT_SORDER)
#define L1_PROTO_TTE(entry) (entry | L1_SECT_S_BIT | L1_SECT_DEFPROT | L1_SECT_DEFCACHE)
#define L1_PAGE_PROTO       (1 << 0)
#define L1_COARSE_PT        (0xFFFFFC00)
#define PT_SIZE             256
#define L2_PAGE_APX         (1 << 9)

#define CHUNK_SIZE 0x800

struct kinfo {
    uint32_t addr;
    size_t   size;
};

struct kernel_text {
    struct kinfo base;
    struct kinfo _text;
    struct kinfo _const;
    struct kinfo _cstring;
    struct kinfo _os_log;
};

struct kernel_data {
    struct kinfo base;
    //struct kinfo _nl_symbol_ptr;
    //struct kinfo _mod_init_func;
    //struct kinfo _mod_term_func;
    //struct kinfo _const;
    //struct kinfo _data;
    //struct kinfo _sysctl_set;
    //struct kinfo _llvm_prf_cnts;
    //struct kinfo _llvm_prf_data;
    //struct kinfo _llvm_prf_names;
    //struct kinfo _llvm_prf_vnds;
    //struct kinfo _bss;
    //struct kinfo _common;
};

//struct kernel_kld {
//    struct kinfo base;
//    struct kinfo _text;
//    struct kinfo _cstring;
//    struct kinfo _const;
//    struct kinfo _mod_init_func;
//    struct kinfo _mod_term_func;
//    struct kinfo _nl_symbol_ptr;
//    struct kinfo _bss;
//};

//struct kernel_last {
//    struct kinfo base;
//    struct kinfo _mod_init_func;
//    struct kinfo _last;
//};

//struct kernel_prelink_text {
//    struct kinfo base;
//    struct kinfo _text;
//};

//struct kernel_prelink_info {
//    struct kinfo base;
//    struct kinfo _info;
//};

typedef struct {
    //uint8_t* kernel;
    struct kinfo base;
    uint32_t kdumpbase;
    //uint32_t kernel_slide;
    uint32_t kernel_entry;
    //void* kernel_mh;
    //uint32_t kernel_delta;
    //uint8_t *buf;
    struct kernel_text          text;
    struct kernel_data          data;
    //struct kernel_kld           kld;
    //struct kernel_last          last;
    //struct kernel_prelink_text  prelink_text;
    //struct kernel_prelink_info  prelink_info;
} kdata_t;

void unjail(mach_port_t tfp0);

#endif /* unjail_h */
