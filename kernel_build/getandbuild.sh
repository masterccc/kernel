#!/bin/bash

#/!\ not tested yet

KURL="$1"
#https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.17.6.tar.xz

KVER=$(echo $KURL | egrep -o 'linux-[0-9.]{6}')
KFILE=${KVER}.tar.xz

echo "Downloading from $KURL ... "
wget $KURL --quiet

echo "Uncompressing..."
tar -xf $KFILE

echo "Creating build dir"
mkdir build/$KVER

cd $KVER

echo "Creating tinyconfig ..."
make O=../build/$KVER tinyconfig 
cp ../config_kernel build/$KVER/.config

echo "Compilation ..."
make O=../build/$KVER
