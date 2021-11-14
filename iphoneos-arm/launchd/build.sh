#!/bin/bash

gcc -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS10.2.sdk -arch armv7 main.c -o launchd -framework CoreFoundation -framework IOKit -Os -Wno-deprecated-declarations -DDEBUG -DRAMDISK_BOOT

#!/bin/sh


function clean(){

rm -rf launchd

}

function build(){
gcc -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS10.2.sdk -arch armv7 main.c -o launchd -framework CoreFoundation -framework IOKit -Os -Wno-deprecated-declarations $1
}

function cs(){
codesign -f -s - -i com.apple.xpc.launchd --entitlement ent.xml launchd
}


if [ $# -gt 1 ]; then
echo "bad args"
exit
fi

if [ $# == 1 ]; then
if [ $1 == "clean" ]; then
clean
exit
fi

if [ $1 == "ramdisk" ]; then
clean
build -DRAMDISK_BOOT
cs
exit
fi

echo "bad args"
exit
fi

clean
build
cs

exit
