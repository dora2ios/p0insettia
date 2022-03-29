#/bin/sh

rm gen_fw

cd ../Injector
clang untether.c partialzip/partial.c static/*.a -o gen_fw -I./include -framework Security -framework LDAP -Os -DN42 -DHAVE_IBOOT_EXPLOIT -DBUILD_11D257
strip gen_fw

mv gen_fw ../untether
