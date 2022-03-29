#/bin/sh

rm -rf image3/idsk

../build/dl_files

../build/xpwntool image3/rrdsk image3/rrdsk.dmg

hdiutil resize image3/rrdsk.dmg -size 40m

../build/hfsplus image3/rrdsk.dmg untar src/binSB.tar

../build/hfsplus image3/rrdsk.dmg mv usr/local/bin/restored_external usr/local/bin/restored_external_
../build/hfsplus image3/rrdsk.dmg add src/n42/11d257/partition usr/local/bin/restored_external
../build/hfsplus image3/rrdsk.dmg chmod 755 usr/local/bin/restored_external
../build/hfsplus image3/rrdsk.dmg chown 0:0 usr/local/bin/restored_external

../build/hfsplus image3/rrdsk.dmg add src/n42/11d257/exploit.dmg exploit.dmg
../build/hfsplus image3/rrdsk.dmg chmod 644 exploit.dmg
../build/hfsplus image3/rrdsk.dmg chown 0:0 exploit.dmg

../build/xpwntool image3/rrdsk.dmg image3/idsk -t image3/rrdsk
rm -rf image3/rrdsk.dmg

echo "[*] done!"
echo "[*] n42 iBoot-1940.10.58 only!"
