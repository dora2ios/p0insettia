/*
 * injector.c
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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <partial.h>

#define N42 (1) // n41/n42 only
#define A6_IBOOT_LOADADDR_BASE (0x80000000)
#define A6_IBOOT_IMAGE_BASE (0xBFF00000)

#define IBOOT_DATA_START (0x40)

static int open_file(const char *file, size_t *sz, void **buf){
    FILE *fd = fopen(file, "r");
    if (!fd) {
        printf("error opening %s\n", file);
        return -1;
    }
    
    fseek(fd, 0, SEEK_END);
    *sz = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    
    *buf = malloc(*sz);
    if (!*buf) {
        printf("error allocating file buffer\n");
        fclose(fd);
        return -1;
    }
    
    fread(*buf, *sz, 1, fd);
    fclose(fd);
    
    return 0;
}

static void patch_iboot(const char* path)
{

    void* buf;
    size_t size;
    
    char str[] = "rd=md0 serial=3 debug=0x14\x00";
    
    uint32_t rsa_patch;
    uint32_t debug_patch;
    uint32_t bootArgs_normal;
    uint32_t bootArgs_rdsk1;
    uint32_t bootArgs_rdsk2;
    uint32_t bootmode;
    uint32_t goCmd;
    uint32_t bootArgsStr;
    
#ifdef N42
    /*
     0x1621A: 00201860
     0x16B62: 01200120
     0x17AA0: 1f78f4bf
     0x17AA4: 1878f4bf
     0x17AA8: 1878f4bf
     0x1E1B0: 01207047
     0x45FDC: 00000080
     0x47B18: str
     */
    rsa_patch       = 0x1621A;
    debug_patch     = 0x16B62;
    bootArgs_normal = 0x17AA0;
    bootArgs_rdsk1  = 0x17AA4;
    bootArgs_rdsk2  = 0x17AA8;
    bootmode        = 0x1E1B0;
    goCmd           = 0x45FDC;
    bootArgsStr     = 0x47B18;
    
    if(open_file(path, &size, &buf) != 0){
        printf("[%s] ERROR: Failed to get image: %s!\n", __FUNCTION__, path);
        return;
    }
    
    printf("[%s] Apply rsa_patch: 0x%08x\n", __FUNCTION__, rsa_patch);
    *(uint32_t*)(buf+IBOOT_DATA_START+rsa_patch)        = 0x60182000;
    
    printf("[%s] Apply debug_patch: 0x%08x\n", __FUNCTION__, debug_patch);
    *(uint32_t*)(buf+IBOOT_DATA_START+debug_patch)      = 0x20012001;
    
    printf("[%s] Apply bootArgs_normal patch: 0x%08x\n", __FUNCTION__, bootArgs_normal);
    *(uint32_t*)(buf+IBOOT_DATA_START+bootArgs_normal)  = A6_IBOOT_IMAGE_BASE + bootArgsStr + 7;
    
    printf("[%s] Apply bootArgs_rdsk1 patch: 0x%08x\n", __FUNCTION__, bootArgs_rdsk1);
    *(uint32_t*)(buf+IBOOT_DATA_START+bootArgs_rdsk1)   = A6_IBOOT_IMAGE_BASE + bootArgsStr;
    
    printf("[%s] Apply bootArgs_rdsk2 patch: 0x%08x\n", __FUNCTION__, bootArgs_rdsk2);
    *(uint32_t*)(buf+IBOOT_DATA_START+bootArgs_rdsk2)   = A6_IBOOT_IMAGE_BASE + bootArgsStr;
    
    printf("[%s] Apply bootmode patch: 0x%08x\n", __FUNCTION__, bootmode);
    *(uint32_t*)(buf+IBOOT_DATA_START+bootmode)         = 0x47702001;
    
    printf("[%s] Apply goCmd patch: 0x%08x\n", __FUNCTION__, goCmd);
    *(uint32_t*)(buf+IBOOT_DATA_START+goCmd)            = A6_IBOOT_LOADADDR_BASE;
    
    printf("[%s] New bootArgs: \"%s\"\n", __FUNCTION__, str);
    printf("[%s] Apply bootArgsStr patch: 0x%08x\n", __FUNCTION__, bootArgsStr);
    strcpy((char *)(buf+IBOOT_DATA_START+bootArgsStr), str);
    
    FILE *out = fopen(path, "w");
    if (!out) {
        printf("[%s] ERROR: Failed to open image: %s!\n", __FUNCTION__, path);
        return;
    }
    
    fwrite(buf, size, 1, out);
    fflush(out);
    fclose(out);
    
    free(buf);
    
    return;
    
#endif
    
}

static int dl_file(const char* url, const char* path, const char* realpath)
{
    int r;
    printf("[%s] Downloading image: %s ...\n", __FUNCTION__, realpath);
    r = partialzip_download_file(url, path, realpath);
    if(r != 0){
        printf("[%s] ERROR: Failed to get image!\n", __FUNCTION__);
        return -1;
    }
    return 0;
}

int main(void)
{

    int r;
    
    const char* outdir = "image3/";
    DIR *d = opendir(outdir);
    if (!d) {
        printf("[%s] Making directory: %s\n", __FUNCTION__, outdir);
        r = mkdir(outdir, 0755);
        if(r != 0){
            printf("[%s] ERROR: Failed to make dir: %s!\n", __FUNCTION__, outdir);
            return -1;
        }
    }

#ifdef N42
    if(open("image3/rrdsk", O_RDONLY) == -1) {
        if(dl_file("https://updates.cdn-apple.com/2019/ios/091-25277-20190722-0C1B94DE-992C-11E9-A2EE-E2C9A77C2E40/iPhone_4.0_32bit_10.3.4_14G61_Restore.ipsw", "058-75249-065.dmg", "image3/rrdsk") != 0) {
            return -1;
        }
    }
    
    if(open("image3/iBoot.n42", O_RDONLY) == -1) {
        if(dl_file("https://updates.cdn-apple.com/2019/ios/091-25277-20190722-0C1B94DE-992C-11E9-A2EE-E2C9A77C2E40/iPhone_4.0_32bit_10.3.4_14G61_Restore.ipsw", "Firmware/dfu/iBEC.iphone5.RELEASE.dfu", "image3/iBoot.n42") != 0) {
            return -1;
        } else {
            patch_iboot("image3/iBoot.n42");
        }
    }
    
#endif
    
    return 0;
}
