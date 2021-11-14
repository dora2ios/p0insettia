/*
 * launchd payload
 * copyright (C) 2021/11/14 sauRdev
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFSerialize.h>
#include <CoreFoundation/CoreFoundation.h>
#include <assert.h>
#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/mount.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <dispatch/dispatch.h>
#include <mach/mach.h>

#include <sys/syscall.h>

extern char **environ;

mach_port_t bPort = MACH_PORT_NULL;

mach_port_t task_for_pid_backdoor(int pid) {
    mach_port_t   psDefault;
    mach_port_t   psDefault_control;
    
    task_array_t  tasks;
    mach_msg_type_number_t numTasks;
    
    kern_return_t kr;
    
    kr = processor_set_default(mach_host_self(), &psDefault);
    
    kr = host_processor_set_priv(mach_host_self(), psDefault, &psDefault_control);
    if (kr != KERN_SUCCESS) {
        fprintf(stderr, "host_processor_set_priv failed with error %x\n", kr);
        mach_error("host_processor_set_priv",kr);
        return 0;
    }
    
    kr = processor_set_tasks(psDefault_control, &tasks, &numTasks);
    if (kr != KERN_SUCCESS) {
        fprintf(stderr,"processor_set_tasks failed with error %x\n",kr);
        return 0;
    }
    
    for (int i = 0; i < numTasks; i++) {
        int foundPid;
        pid_for_task(tasks[i], &foundPid);
        if (foundPid == pid) return tasks[i];
    }
    
    return MACH_PORT_NULL;
}

void insertBP() {
    task_set_bootstrap_port(mach_task_self(), bPort);
}

void removeBP() {
    task_set_bootstrap_port(mach_task_self(), MACH_PORT_NULL);
}


// attach
int attach_dmg(const char* abspath, const char* mntpath)
{
    
    char buf[256];
    
    io_service_t service = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("IOHDIXController"));
    assert(service);
    io_connect_t connect;
    assert(!IOServiceOpen(service, mach_task_self(), 0, &connect));
    
    CFStringRef uuid = CFUUIDCreateString(NULL, CFUUIDCreate(kCFAllocatorDefault));
    CFMutableDictionaryRef props = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(props, CFSTR("hdik-unique-identifier"), uuid);
    CFDataRef path = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, (UInt8 *) abspath, strlen(abspath), kCFAllocatorNull);
    assert(path);
    CFDictionarySetValue(props, CFSTR("image-path"), path);
    
    CFDataRef props_data = CFPropertyListCreateData(kCFAllocatorDefault, props, 0x64, 0LL, 0LL);
    assert(props_data);
    
    struct HDIImageCreateBlock64 {
        uint32_t magic;
        uint32_t one;
        char *props;
        uint32_t null;
        uint32_t props_size;
        char ignored[0xf8 - 16];
    } stru;
    memset(&stru, 0, sizeof(stru));
    stru.magic = 0xbeeffeed;
    stru.one = 1;
    stru.props = (char *) CFDataGetBytePtr(props_data);
    stru.props_size = CFDataGetLength(props_data);
    assert(offsetof(struct HDIImageCreateBlock64, props) == 8);
    
    uint32_t val;
    size_t val_size = sizeof(val);
    
    kern_return_t ret = IOConnectCallStructMethod(connect, 0, &stru, 0x100, &val, &val_size);
    if(ret) {
        fprintf(stderr, "returned %x\n", ret);
        return 1;
    }
    assert(val_size == sizeof(val));
    
    CFMutableDictionaryRef pmatch = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(pmatch, CFSTR("hdik-unique-identifier"), uuid);
    CFMutableDictionaryRef matching = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(matching, CFSTR("IOPropertyMatch"), pmatch);
    service = IOServiceGetMatchingService(kIOMasterPortDefault, matching);
    if(!service) {
        fprintf(stderr, "successfully attached, but didn't find top entry in IO registry\n");
        return 1;
    }
    
    bool ok = false;
    io_iterator_t iter;
    assert(!IORegistryEntryCreateIterator(service, kIOServicePlane, kIORegistryIterateRecursively, &iter));
    while( (service = IOIteratorNext(iter)) ) {
        CFStringRef bsd_name = IORegistryEntryCreateCFProperty(service, CFSTR("BSD Name"), NULL, 0);
        if(bsd_name) {
            //char buf[MAXPATHLEN];
            assert(CFStringGetCString(bsd_name, buf, sizeof(buf), kCFStringEncodingUTF8));
            puts(buf);
            ok = true;
        }
    }
    
    if(!ok) {
        fprintf(stderr, "successfully attached, but didn't find BSD name in IO registry\n");
        return 1;
    }
    
    char str[512];
    memset(&str, 0x0, 512);
    sprintf(str, "/dev/%s", buf);
    
    char* nmr = strdup(str);
    int mntr = mount("hfs", mntpath, MNT_RDONLY, &nmr);
    printf("mount = %d\n",mntr);
    
    return 0;
}


int main(int argc, char **argv) {
    
    /* Linus Henze's iDownload */
    if (getpid() == 1) {

#ifdef RAMDISK_BOOT
        int isInstalled;
#endif
        
        struct stat st;
        while(1){
            if(0 == syscall(SYS_stat, "/dev/disk0s1s1", &st))
                break;
            usleep(10000);
        }
        
        char* nmr = strdup("/dev/disk0s1s1");
        int mntr = mount("hfs", "/", MNT_UPDATE | MNT_RDONLY, &nmr);

#ifdef RAMDISK_BOOT
        isInstalled = syscall(SYS_stat, "/.installed_p0insettia", &st);

        if(isInstalled != 0) {
            attach_dmg("/haxx/loader.dmg", "/Developer");
        } else {
#endif
            // already installed
            attach_dmg("/haxx/xpcd.dmg", "/System/Library/Caches/com.apple.xpcd");
#ifdef RAMDISK_BOOT
        }
#endif
        
        // we're the first process
        // spawn launchd
        pid_t pid = fork();
        if (pid != 0) {
            // parent
            char *args[] = { "/sbin/launchd", NULL };
            execve("/sbin/launchd", args, environ);
            return -1;
        }
        
        // sleep a bit for launchd to do some work
        sleep(3);
        
        // now get task port for something that has a valid bootstrap port
        int ctr = getpid() + 1;
        mach_port_t port = 0;
        while (!port) {
            port = task_for_pid_backdoor(ctr++);
            if (!port) {
                continue;
            }
            
            mach_port_t bp = 0;
            task_get_bootstrap_port(port, &bp);
            if (!bp) {
                port = 0;
                continue;
            }
            
            bPort = bp;
            insertBP();
        }
        
        
#ifdef RAMDISK_BOOT
        if(isInstalled != 0) {
            {
                char* nmr = strdup("/dev/disk0s1s1");
                int mntr = mount("hfs", "/", MNT_UPDATE, &nmr);
            }
            
            return 0;
        }
#endif
        // now we've got a valid bootstrap port
        // re-exec with the bp
        execv(argv[0], argv);
        return -1;
        
    } else if (getpid() == 2) {
        
        int console_fd;
        
        char banner[1024];
        console_fd = syscall(SYS_open, "/dev/console", O_RDWR);
        syscall(SYS_dup2, console_fd, 0);
        syscall(SYS_dup2, console_fd, 1);
        syscall(SYS_dup2, console_fd, 2);

#ifdef DEBUG
        sprintf(banner, \
                "================================================================\n" \
                ":: p0insettia launchd payload\n" \
                ":: Version: 1.0 beta 1 [1A134]\n" \
                ":: Build: DEBUG\n" \
                ":: (c) sakuRdev\n" \
                "================================================================\n");
#else
        sprintf(banner, \
                "================================================================\n" \
                ":: p0insettia launchd payload\n" \
                ":: Version: 1.0 beta 1\n" \
                ":: Build: RELEASE\n" \
                ":: (c) sakuRdev\n" \
                "================================================================\n");
#endif
        syscall(SYS_write, console_fd, banner, strlen(banner));
        
        
        // set PATH
        setenv("PATH", "/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin", 1);
        
        // remount rootfs
        {
            printf("[*] remounting rootfs\n");
            char* nmr = strdup("/dev/disk0s1s1");
            int mntr = mount("hfs", "/", MNT_UPDATE, &nmr);
            printf("remount = %d\n",mntr);
        }
        
        // substrate
        {
            pid_t pid = fork();
            if (pid == 0) {
                // child
                char *args[] = { "/bin/bash", "-c", "/usr/libexec/dirhelper", NULL };
                execve("/bin/bash", args, environ);
                exit(-1);
            }
            
            // sleep a bit
            sleep(1);
            
            // parent
            waitpid(pid, NULL, 0);
        }
        
        // jb daemons
        {
            pid_t pid = fork();
            if (pid == 0) {
                // child
                char *args[] = { "/bin/bash", "-c", "/bin/launchctl load /Library/LaunchDaemons/*", NULL };
                execve("/bin/bash", args, environ);
                exit(-1);
            }
            
            // parent
            waitpid(pid, NULL, 0);
        }
        
        // reload sys daemons
        {
            pid_t pid = fork();
            if (pid == 0) {
                // child
                char *args[] = { "/bin/bash", "-c", "/bin/launchctl unload /haxx/LaunchDaemons/*; /bin/launchctl load /haxx/LaunchDaemons/*", NULL };
                execve("/bin/bash", args, environ);
                exit(-1);
            }
            
            // parent
            waitpid(pid, NULL, 0);
        }
        
        // openssh
        if(open("/Library/LaunchDaemons/com.openssh.sshd.plist", O_RDONLY) != -1) {
            pid_t pid = fork();
            if (pid == 0) {
                // child
                char *args[] = { "/bin/bash", "-c", "/bin/launchctl unload /Library/LaunchDaemons/com.openssh.sshd.plist; /usr/libexec/sshd-keygen-wrapper", NULL };
                execve("/bin/bash", args, environ);
                exit(-1);
            }
            
            // parent
            waitpid(pid, NULL, 0);
        }
        
        
    }
    
    return 0;
        
}
