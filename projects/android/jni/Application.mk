# The ARMv7 is significanly faster due to the use of the hardware FPU
NDK_TOOLCHAIN_VERSION=4.7
APP_PROJECT_PATH := $(shell pwd)
APP_BUILD_SCRIPT := $(APP_PROJECT_PATH)/Android.mk
$(warning $(APP_PROJECT_PATH))
#APP_STL:=stlport_static
APP_STL := gnustl_static
APP_MODULES := zslib_android




