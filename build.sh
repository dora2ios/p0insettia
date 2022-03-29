#!/bin/sh


function clean(){
  echo "cleaning..."

  cd build/
  rm -rf dl_files
  rm -rf image3
  rm -rf ipwnder_lite
  cd ..

  cd iphoneos-arm/iboot
  ./mk.sh clean
  cd ../..

  cd iphoneos-arm/launchd
  rm -rf launchd
  cd ../..

  cd iphoneos-arm/rdsk
  rm -rf restored_external
  cd ../..

}

function injector(){
  echo "injector"

  cd Injector/
  ./mk_macosx.sh
  cd ..
}

function pwnder(){
  echo "ipwnder_lite"

  cd ipwnder_lite/
  make all
  mv -v ipwnder_macosx ../build/ipwnder_lite
  cd ..
}

function payload(){
  echo "iboot payload"

  cd iphoneos-arm/iboot/
  ./mk.sh
  cd ../..
}

function launchd(){
  echo "launchd payload"

  cd iphoneos-arm/launchd/
  ./build.sh ramdisk
  cd ../..
}

function rdsk(){
  echo "ramdisk utils"

  cd iphoneos-arm/rdsk/
  ./build.sh
  cd ../..
}

function all(){
  injector
  pwnder
  payload
  launchd
  rdsk
}



if [ $# == 1 ]; then
  if [ $1 == "clean" ]; then
    clean
    exit
  fi

  if [ $1 == "injector" ]; then
    injector
    exit
  fi

  if [ $1 == "pwnder" ]; then
    pwnder
    exit
  fi

  if [ $1 == "payload" ]; then
    payload
    exit
  fi

  if [ $1 == "launchd" ]; then
    launchd
    exit
  fi

  if [ $1 == "rdsk" ]; then
    rdsk
    exit
  fi

  if [ $1 == "all" ]; then
    clean
    all
    exit
  fi

    echo "bad args"
    exit

else
  echo ""$0" [all|injector|pwnder|payload|launchd|rdsk|clean]"
  exit
fi

clean

