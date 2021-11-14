#!/bin/sh


function clean(){

  rm -rf payload
  rm -rf payload_rd

  cd hook/
  make clean
  rm -rf ../prehook/payload.h
  rm -rf payload
  cd ../

  cd prehook/
  make clean
  rm -rf payload
  cd ../

  cd prehook/
  make clean
  cd ../

  cd maker
  rm -rf maker
  rm -rf payload
  rm -rf payload_rd
  cd ..

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
    echo "bad args"
    exit
fi

clean

cd hook/
make
xxd -i payload >../prehook/payload.h
cd ../

cd prehook/
make
cd ../

cd maker
gcc maker.c -o maker
./maker ../hook/payload ../prehook/payload payload_rd payload
mv -v payload ..
mv -v payload_rd ..
cd ..
