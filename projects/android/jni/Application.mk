# The ARMv7 is significanly faster due to the use of the hardware FPU
NDK_TOOLCHAIN_VERSION:=clang3.5
APP_PROJECT_PATH := $(shell pwd)
APP_BUILD_SCRIPT := $(APP_PROJECT_PATH)/Android.mk
$(warning $(APP_PROJECT_PATH))
APP_STL := c++_static
APP_MODULES := zslib_android




