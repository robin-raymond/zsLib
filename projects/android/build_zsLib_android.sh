#!/bin/sh
echo "Please enter your android ndk path:"
echo "For example:/home/manishg/Adhikari/ObjectRTC/android-ndk-r8e"
read Input
echo "You entered:$Input"

echo "----------------- Exporting the android-ndk path ----------------"

#Set path
export PATH=$PATH:$Input:$Input/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin

#create install directories
mkdir -p ./../../../build
mkdir -p ./../../../build/android

echo "----------- Building boost 1.53.0 for ANDROID platform -----------------"

#zsLib module build
echo "------------------- Building zsLib for ANDROID platform ---------------"
pushd `pwd`
mkdir ./../../../build/android/zsLib

rm -rf ./obj/*
export ANDROIDNDK_PATH=$Input
export NDK_PROJECT_PATH=`pwd`
ndk-build APP_PLATFORM=android-9
popd

echo "-------- Installing zsLib libs -----"
cp -r ./obj/local/armeabi/lib* ./../../../build/android/zsLib/

#clean
rm -rf ./obj

