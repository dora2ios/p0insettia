#!/bin/bash

gcc -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS10.2.sdk -arch armv7 main.c -o restored_external
codesign -f -s - -i com.apple.restored_external restored_external
