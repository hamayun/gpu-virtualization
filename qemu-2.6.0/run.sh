#!/bin/bash

TOPDIR=..
BINDIR=$TOPDIR/install1/bin
#MEDIA=$TOPDIR/media/image
MEDIA=../media/image1
CDROM=$TOPDIR/media/ubuntu.iso

QEMU=qemu-system-x86_64
QEMU_ARGS="-device virtio-serial -device datatransfer-serial,id=dtransf_test,name=robineier -M pc-0.14 -boot order=d -display sdl -enable-kvm -cpu qemu64 -m 2024M -drive file=$MEDIA,format=raw -device ivshmem,size=256m,shm=mymem -show-cursor"
#QEMU_ARGS="-device virtio-serial -chardev pipe,id=hostpipe,path=/home/kristoffer/pripo -device virtserialport,chardev=hostpipe,name=guestpipe -M pc-0.14 -boot order=d -sdl -enable-kvm -cpu qemu64 -m 2024M $MEDIA"
#QEMU_ARGS="-M pc-0.14 -boot order=d -sdl -enable-kvm -cpu qemu64 -m 1024M $MEDIA"

CUDAGDB=cuda-gdb
CUDAGDBARGS="-M pc-0.14 -boot order=d -sdl -enable-kvm -cpu qemu64 -m 2024M $MEDIA"

VNCVIEWER="gtkvncviewer"
VNCARGS="-s 127.0.0.1:5901"

VALGRIND=valgrind

function help(){
    echo "Helper script for running the qemu system emulator in normal and debugging mode."
    echo -e "Valid targets:\n\tnormal: Runs the qemu system using gtkvncvviewer.\n\tdebug: Runs the qemu system in debug mode.\n\tkill: Terminates all programs started by this script."
    echo -e "\nHarddisk image found in $MEDIA\nCDROM image found in $CDROM"
    echo "Script looks for $QEMU in $BINDIR, running with arguments $QEMU_ARGS. Connects to VNC server using $VNCVIEWER with arguments $VNCARGS."
}

function normal(){
    $BINDIR/$QEMU $QEMU_ARGS &
}

function valgrind(){
    $VALGRIND --tool=memcheck $BINDIR/$QEMU $QEMU_ARGS
}

function debug(){
    gdb --args $BINDIR/$QEMU $QEMU_ARGS
}

function cudagdb(){
    $CUDAGDB --args $BINDIR/$QEMU $QEMU_ARGS
}

if [ "$1" == help ]
then 
    help;
elif [ "$1" == normal ]
then
    normal;
elif [ "$1" == valgrind ]
then
    valgrind;
elif [ "$1" == debug ]
then
    debug;
elif [ "$1" == cudagdb ]
then
    cudagdb;
else 
    help;
fi

