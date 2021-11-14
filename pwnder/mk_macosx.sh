#/bin/sh

rm ../build/ipwnder_lite

clang main.c iousb.c checkm8_s5l8950x.c common.c partialzip/partial.c static/*.a -o poc_macosx -I./include -framework IOKit -framework CoreFoundation -framework Security -framework LDAP -Os -DHAVE_DEBUG
strip poc_macosx
mv -v poc_macosx ../build/ipwnder_lite
