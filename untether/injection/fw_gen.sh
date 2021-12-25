#/bin/sh

# rdsk
xpwntool fw/058-4276-009.dmg fw/ramdisk.dmg -iv 13b6456bec67fa74faada14e1c3607aa -k 4e0bcc542aefc750cd463f6d0ed4710f15fb0ec0d2a11d4e213b6f58c1e20e87

./hfsplus fw/ramdisk.dmg grow 12000000

./hfsplus fw/ramdisk.dmg extract usr/sbin/asr asr
bspatch asr asr_p iPhone5,2_11D257.bundle/asr.patch
./hfsplus fw/ramdisk.dmg mv usr/sbin/asr usr/sbin/asr_
./hfsplus fw/ramdisk.dmg add asr_p usr/sbin/asr
./hfsplus fw/ramdisk.dmg chmod 755 usr/sbin/asr
./hfsplus fw/ramdisk.dmg chown 0:0 usr/sbin/asr

#./hfsplus fw/ramdisk.dmg extract usr/local/share/restore/options.n42.plist options.n42.plist
./hfsplus fw/ramdisk.dmg mv usr/local/share/restore/options.n42.plist usr/local/share/restore/options.n42.plist_
./hfsplus fw/ramdisk.dmg add options.n42.plist usr/local/share/restore/options.n42.plist
./hfsplus fw/ramdisk.dmg chmod 644 usr/local/share/restore/options.n42.plist
./hfsplus fw/ramdisk.dmg chown 0:0 usr/local/share/restore/options.n42.plist

mv fw/058-4276-009.dmg fw/tmp.dmg
xpwntool fw/ramdisk.dmg fw/058-4276-009.dmg -t fw/tmp.dmg

# ibss
xpwntool fw/Firmware/dfu/iBSS.n42ap.RELEASE.dfu fw/Firmware/dfu/iBSS.n42ap.RELEASE.dec -iv d279e5c309be7ac035fd313958a178be -k 617f7e2d5d8e2940a325758cd42055b83e2e3d243f068d5a9015b0fe67bed815
bspatch fw/Firmware/dfu/iBSS.n42ap.RELEASE.dec fw/Firmware/dfu/iBSS.n42ap.RELEASE.pwnd iPhone5,2_11D257.bundle/iBSS.n42ap.RELEASE.patch
mv fw/Firmware/dfu/iBSS.n42ap.RELEASE.dfu fw/Firmware/dfu/iBSS.n42ap.TEMP.dfu
xpwntool fw/Firmware/dfu/iBSS.n42ap.RELEASE.pwnd fw/Firmware/dfu/iBSS.n42ap.RELEASE.dfu -t fw/Firmware/dfu/iBSS.n42ap.TEMP.dfu

# ibec
xpwntool fw/Firmware/dfu/iBEC.n42ap.RELEASE.dfu fw/Firmware/dfu/iBEC.n42ap.RELEASE.dec -iv 1d45b6ca42dafd5d711e3d23e5fa0fc7 -k 459912ddeeeb9d4a1c66068c8c1d8f46d8dd72e3e7dfa3ff0326f1ab6bb59c28
bspatch fw/Firmware/dfu/iBEC.n42ap.RELEASE.dec fw/Firmware/dfu/iBEC.n42ap.RELEASE.pwnd iPhone5,2_11D257.bundle/iBEC.n42ap.RELEASE.patch
mv fw/Firmware/dfu/iBEC.n42ap.RELEASE.dfu fw/Firmware/dfu/iBEC.n42ap.TEMP.dfu
xpwntool fw/Firmware/dfu/iBEC.n42ap.RELEASE.pwnd fw/Firmware/dfu/iBEC.n42ap.RELEASE.dfu -t fw/Firmware/dfu/iBEC.n42ap.TEMP.dfu

rm -rf fw/Firmware/dfu/iBSS.n42ap.RELEASE.dec
rm -rf fw/Firmware/dfu/iBSS.n42ap.RELEASE.pwnd
rm -rf fw/Firmware/dfu/iBEC.n42ap.RELEASE.dec
rm -rf fw/Firmware/dfu/iBEC.n42ap.RELEASE.pwnd
rm -rf fw/ramdisk.dmg
rm -rf asr
rm -rf asr_p
