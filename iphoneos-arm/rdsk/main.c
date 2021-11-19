/*
 * restored_external
 * copyright (C) 2021/11/14 sakuRdev
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>

extern char **environ;

static void mv_file(char* path1, char* path2)
{
    pid_t pid = fork();
    if (pid == 0) {
        // child
        char *args[] = { "/bin/mv", "-v", path1, path2, NULL };
        execve("/bin/mv", args, environ);
        exit(-1);
    }
    
    // sleep a bit
    sleep(1);
    
    // parent
    waitpid(pid, NULL, 0);
    
}

static void symlink_file(char* path1, char* path2)
{
    pid_t pid = fork();
    if (pid == 0) {
        // child
        char *args[] = { "/bin/ln", "-s", path1, path2, NULL };
        execve("/bin/ln", args, environ);
        exit(-1);
    }
    
    // sleep a bit
    sleep(1);
    
    // parent
    waitpid(pid, NULL, 0);
    
}


int main(int argc, char **argv)
{
    // set PATH
    
    sleep(15);
    
    setenv("PATH", "/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin", 1);
    
    // remount ramdiskfs
    {
        printf("[*] remounting ramdiskfs\n");
        char* nmr = strdup("/dev/md0");
        int mntr = mount("hfs", "/", MNT_UPDATE, &nmr);
        printf("remount = %d\n",mntr);
    }
    
    // remount rootfs
    {
        pid_t pid = fork();
        if (pid == 0) {
            // child
            char *args[] = { "/sbin/mount_hfs", "/dev/disk0s1s1", "/mnt1", NULL };
            execve("/sbin/mount_hfs", args, environ);
            exit(-1);
        }
        
        // sleep a bit
        sleep(1);
        
        // parent
        waitpid(pid, NULL, 0);
    }
    
    {
        pid_t pid = fork();
        if (pid == 0) {
            // child
            char *args[] = { "/bin/rm", "-rf", "/mnt1/haxx", NULL };
            execve("/bin/rm", args, environ);
            exit(-1);
        }
        
        // sleep a bit
        sleep(1);
        
        // parent
        waitpid(pid, NULL, 0);
    }
    
    mkdir("/mnt1/haxx", 0755);
    mkdir("/mnt1/haxx/LaunchDaemons", 0755);
    sleep(1);
    
    mv_file("/haxx/launchd", "/mnt1/haxx/launchd");
    mv_file("/haxx/loader.dmg", "/mnt1/haxx/loader.dmg");
    mv_file("/haxx/xpcd.dmg", "/mnt1/haxx/xpcd.dmg");
    
    mv_file("/dirhelper", "/mnt1/usr/libexec/dirhelper");
    
    mv_file("/bin/launchctl", "/mnt1/bin/launchctl");
    
    symlink_file("/System/Library/LaunchDaemons/com.apple.backboardd.plist", "/mnt1/haxx/LaunchDaemons/com.apple.backboardd.plist");
    symlink_file("/System/Library/LaunchDaemons/com.apple.mobile.lockdown.plist", "/mnt1/haxx/LaunchDaemons/com.apple.mobile.lockdown.plist");
    symlink_file("/System/Library/LaunchDaemons/com.apple.SpringBoard.plist", "/mnt1/haxx/LaunchDaemons/com.apple.SpringBoard.plist");
    
    {
        pid_t pid = fork();
        if (pid == 0) {
            // child
            char *args[] = { "/usr/sbin/nvram", "auto-boot=false", NULL };
            execve("/usr/sbin/nvram", args, environ);
            exit(-1);
        }
        
        // sleep a bit
        sleep(1);
        
        // parent
        waitpid(pid, NULL, 0);
    }
    
    sync();
    sync();
    sync();
    sleep(1);
    
    reboot(0);
 
    return 0;
        
}
