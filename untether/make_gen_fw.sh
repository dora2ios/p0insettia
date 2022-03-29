#/bin/sh

#define

BUILD="BUILD_11D257"
#BUILD="BUILD_11D201"
#BUILD="BUILD_11D167"

rm gen_fw

cd ../Injector
clang untether.c partialzip/partial.c static/*.a -o gen_fw -I./include -framework Security -framework LDAP -Os -DN42 -DHAVE_IBOOT_EXPLOIT -D"$BUILD"
strip gen_fw

mv gen_fw ../untether
