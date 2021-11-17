#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import <stdio.h>
#import <stdlib.h>
#import <string.h>
#import <sys/sysctl.h>
#import <sys/utsname.h>

#import "offsets.h"

#define SYSTEM_VERSION_EQUAL_TO(v)                  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedSame)
#define SYSTEM_VERSION_GREATER_THAN(v)              ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedDescending)
#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)
#define SYSTEM_VERSION_LESS_THAN(v)                 ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedAscending)
#define SYSTEM_VERSION_LESS_THAN_OR_EQUAL_TO(v)     ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedDescending)

int* offsets = NULL;

static int kstruct_offsets_10_x[] = {
    0x7,   // KSTRUCT_OFFSET_TASK_LCK_MTX_TYPE
    0x8,   // KSTRUCT_OFFSET_TASK_REF_COUNT
    0xc,   // KSTRUCT_OFFSET_TASK_ACTIVE
    0x14,  // KSTRUCT_OFFSET_TASK_VM_MAP
    0x18,  // KSTRUCT_OFFSET_TASK_NEXT
    0x1c,  // KSTRUCT_OFFSET_TASK_PREV
    0x9c,  // KSTRUCT_OFFSET_TASK_ITK_SELF
    0x1e8, // KSTRUCT_OFFSET_TASK_ITK_SPACE
    0x22c, // KSTRUCT_OFFSET_TASK_BSD_INFO

    0x0,   // KSTRUCT_OFFSET_IPC_PORT_IO_BITS
    0x4,   // KSTRUCT_OFFSET_IPC_PORT_IO_REFERENCES
    0x30,  // KSTRUCT_OFFSET_IPC_PORT_IKMQ_BASE
    0x3c,  // KSTRUCT_OFFSET_IPC_PORT_MSG_COUNT
    0x44,  // KSTRUCT_OFFSET_IPC_PORT_IP_RECEIVER
    0x48,  // KSTRUCT_OFFSET_IPC_PORT_IP_KOBJECT
    0x58,  // KSTRUCT_OFFSET_IPC_PORT_IP_PREMSG
    0x5c,  // KSTRUCT_OFFSET_IPC_PORT_IP_CONTEXT
    0x6c,  // KSTRUCT_OFFSET_IPC_PORT_IP_SRIGHTS

    0x8,   // KSTRUCT_OFFSET_PROC_PID
    0x9c,  // KSTRUCT_OFFSET_PROC_P_FD
    0xc,   // KSTRUCT_OFFSET_PROC_TASK

    0x0,   // KSTRUCT_OFFSET_FILEDESC_FD_OFILES

    0x8,   // KSTRUCT_OFFSET_FILEPROC_F_FGLOB

    0x28,  // KSTRUCT_OFFSET_FILEGLOB_FG_DATA

    0x10,  // KSTRUCT_OFFSET_SOCKET_SO_PCB

    0x10,  // KSTRUCT_OFFSET_PIPE_BUFFER

    0xc,   // KSTRUCT_OFFSET_IPC_SPACE_IS_TABLE_SIZE
    0x14,  // KSTRUCT_OFFSET_IPC_SPACE_IS_TABLE

    0x8,   // KSTRUCT_OFFSET_HOST_SPECIAL

    0x0,   // KFREE_ADDR_OFFSET
    0x10,  // KSTRUCT_SIZE_IPC_ENTRY
    0x4,   // KSTRUCT_OFFSET_IPC_ENTRY_IE_BITS
};

int koffset(enum kstruct_offset offset) {
    if (offsets == NULL) {
        printf("need to call offsets_init() prior to querying offsets\n");
        return 0;
    }
    return offsets[offset];
}

uint32_t create_outsize;

void offsets_init() {
    
    if (SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(@"10.0")) {
        printf("[i] offsets selected for iOS 10.x\n");
        offsets = kstruct_offsets_10_x;
        create_outsize = 0x3c8;
    } else {
        printf("[-] iOS version too low, 10.0 required\n");
        exit(EXIT_FAILURE);
    }
}
