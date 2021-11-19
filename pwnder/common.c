/*
 * common.c
 * copyright (C) 2021/11/14 sakuRdev
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#include <iousb.h>

int boot_checkm8_32(io_client_t client, unsigned char* data, size_t sz)
{
    transfer_t result;
    size_t blanksz;
    
    blanksz = 16;
    unsigned char blank[blanksz];
    
    memset(&blank, '\0', blanksz);
    
    LOG_PROGRESS("[%s] reconnecting", __FUNCTION__);
    io_reset(client);
    io_close(client);
    client = NULL;
    usleep(1000);
    get_device_time_stage(&client, 5, DEVICE_DFU, false);
    if(!client) {
        ERROR("[%s] ERROR: Failed to reconnect to device", __FUNCTION__);
        return -1;
    }
    
    result = usb_ctrl_transfer(client, 0x21, 1, 0x0000, 0x0000, blank, blanksz);
    DEBUGLOG("[%s] (1/5) %x", __FUNCTION__, result.ret);
    result = usb_ctrl_transfer(client, 0x21, 1, 0x0000, 0x0000, NULL, 0);
    DEBUGLOG("[%s] (2/5) %x", __FUNCTION__, result.ret);
    result = usb_ctrl_transfer(client, 0xA1, 3, 0x0000, 0x0000, blank, 6);
    DEBUGLOG("[%s] (3/5) %x", __FUNCTION__, result.ret);
    result = usb_ctrl_transfer(client, 0xA1, 3, 0x0000, 0x0000, blank, 6);
    DEBUGLOG("[%s] (4/5) %x", __FUNCTION__, result.ret);
    
    LOG_PROGRESS("[%s] sending payload", __FUNCTION__);
    
    {
        size_t len = 0;
        size_t size;
        while(len < sz) {
            size = ((sz - len) > 0x800) ? 0x800 : (sz - len);
            result = usb_ctrl_transfer(client, 0x21, 1, 0x0000, 0x0000, (unsigned char*)&data[len], size);
            if(result.wLenDone != size || result.ret != kIOReturnSuccess){
                ERROR("[%s] ERROR: Failed to send payload [%x, %x]", __FUNCTION__, result.ret, (unsigned int)result.wLenDone);
                return -1;
            }
            len += size;
        }
    }
    
    result = usb_ctrl_transfer_with_time(client, 0xA1, 2, 0xFFFF, 0x0000, NULL, 0, 100);
    DEBUGLOG("[%s] (5/5) %x", __FUNCTION__, result.ret);
    
    return 0;
}
