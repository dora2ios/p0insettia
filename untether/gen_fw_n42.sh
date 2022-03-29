#/bin/sh

# N42 11D257
DEVICE="n42ap"
VERSION="11D257"
RAMDISK="058-4276-009.dmg"
RDSK_IV="13b6456bec67fa74faada14e1c3607aa"
RDSK_KEY="4e0bcc542aefc750cd463f6d0ed4710f15fb0ec0d2a11d4e213b6f58c1e20e87"
IBSS="iBSS."$DEVICE".RELEASE"
IBSS_IV="d279e5c309be7ac035fd313958a178be"
IBSS_KEY="617f7e2d5d8e2940a325758cd42055b83e2e3d243f068d5a9015b0fe67bed815"
IBEC="iBEC."$DEVICE".RELEASE"
IBEC_IV="1d45b6ca42dafd5d711e3d23e5fa0fc7"
IBEC_KEY="459912ddeeeb9d4a1c66068c8c1d8f46d8dd72e3e7dfa3ff0326f1ab6bb59c28"
OS="058-4447-009.dmg"
FWBUNDLE="iPhone5,2_11D257.bundle"
OPTIONNAME="options.n42.plist"

# config end

rm -rf fw/

./gen_fw

../build/xpwntool fw/"$RAMDISK" fw/ramdisk.dmg -iv $RDSK_IV -k $RDSK_KEY
../build/hfsplus fw/ramdisk.dmg grow 12000000

../build/hfsplus fw/ramdisk.dmg extract usr/sbin/asr asr
bspatch asr asr_p injection/firmware/"$FWBUNDLE"/asr.patch
../build/hfsplus fw/ramdisk.dmg mv usr/sbin/asr usr/sbin/asr_
../build/hfsplus fw/ramdisk.dmg add asr_p usr/sbin/asr
../build/hfsplus fw/ramdisk.dmg chmod 755 usr/sbin/asr
../build/hfsplus fw/ramdisk.dmg chown 0:0 usr/sbin/asr

../build/hfsplus fw/ramdisk.dmg extract usr/local/bin/restored_external restored_external
bspatch restored_external restored_external_p injection/firmware/"$FWBUNDLE"/restored_external.patch
../build/hfsplus fw/ramdisk.dmg mv usr/local/bin/restored_external usr/local/bin/restored_external_
../build/hfsplus fw/ramdisk.dmg add restored_external_p usr/local/bin/restored_external
../build/hfsplus fw/ramdisk.dmg chmod 755 usr/local/bin/restored_external
../build/hfsplus fw/ramdisk.dmg chown 0:0 usr/local/bin/restored_external

../build/hfsplus fw/ramdisk.dmg mv usr/local/share/restore/"$OPTIONNAME" usr/local/share/restore/"$OPTIONNAME"_
../build/hfsplus fw/ramdisk.dmg add injection/options.plist usr/local/share/restore/"$OPTIONNAME"
../build/hfsplus fw/ramdisk.dmg chmod 644 usr/local/share/restore/"$OPTIONNAME"
../build/hfsplus fw/ramdisk.dmg chown 0:0 usr/local/share/restore/"$OPTIONNAME"

mv fw/"$RAMDISK" fw/tmp.dmg
../build/xpwntool fw/ramdisk.dmg fw/"$RAMDISK" -t fw/tmp.dmg

# ibss
../build/xpwntool fw/Firmware/dfu/"$IBSS".dfu fw/Firmware/dfu/"$IBSS".dec -iv $IBSS_IV -k $IBSS_KEY
bspatch fw/Firmware/dfu/"$IBSS".dec fw/Firmware/dfu/"$IBSS".pwnd injection/firmware/"$FWBUNDLE"/iBSS.patch
mv fw/Firmware/dfu/"$IBSS".dfu fw/Firmware/dfu/"$IBSS"_TMP.dfu
../build/xpwntool fw/Firmware/dfu/"$IBSS".pwnd fw/Firmware/dfu/"$IBSS".dfu -t fw/Firmware/dfu/"$IBSS"_TMP.dfu

# ibec
../build/xpwntool fw/Firmware/dfu/"$IBEC".dfu fw/Firmware/dfu/"$IBEC".dec -iv $IBEC_IV -k $IBEC_KEY
bspatch fw/Firmware/dfu/"$IBEC".dec fw/Firmware/dfu/"$IBEC".pwnd injection/firmware/"$FWBUNDLE"/iBEC.patch
mv fw/Firmware/dfu/"$IBEC".dfu fw/Firmware/dfu/"$IBEC"_TMP.dfu
../build/xpwntool fw/Firmware/dfu/"$IBEC".pwnd fw/Firmware/dfu/"$IBEC".dfu -t fw/Firmware/dfu/"$IBEC"_TMP.dfu

# pyld 
../build/xpwntool ../iphoneos-arm/iboot/hook/payload fw/Firmware/all_flash/all_flash."$DEVICE".production/applelogoP@2x~iphone.s5l8950x.img3 -t src/pyld_template.img3

# m
cp -a -v injection/firmware/"$FWBUNDLE"/manifest fw/Firmware/all_flash/all_flash."$DEVICE".production/

# os
touch fw/"$OS"

rm -rf asr
rm -rf asr_p
rm -rf restored_external
rm -rf restored_external_p
rm -rf fw/tmp.dmg
rm -rf fw/ramdisk.dmg
rm -rf fw/Firmware/dfu/"$IBSS".pwnd
rm -rf fw/Firmware/dfu/"$IBSS".dec
rm -rf fw/Firmware/dfu/"$IBSS"_TMP.dfu
rm -rf fw/Firmware/dfu/"$IBEC".pwnd
rm -rf fw/Firmware/dfu/"$IBEC".dec
rm -rf fw/Firmware/dfu/"$IBEC"_TMP.dfu

cd fw

zip -r0 ../restorenand_"$DEVICE"_"$VERSION".ipsw *

cd ..

rm -rf fw
