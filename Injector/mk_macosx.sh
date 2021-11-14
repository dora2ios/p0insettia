#/bin/sh

rm ../build/dl_files

clang main.c partialzip/partial.c static/*.a -o dl_files -I./include -framework Security -framework LDAP -Os
strip dl_files

mv -v dl_files ../build
