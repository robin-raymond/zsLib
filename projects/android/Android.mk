LOCAL_PATH := $(call my-dir)/../../

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm


LOCAL_CFLAGS	:= -O2 -g -W -Wall -pipe -fPIC -D_ANDROID\
	-DHAVE_INTTYPES_H \
	-DHAVE_UNISTD_H \
	-DHAVE_ERRNO_H \
	-DHAVE_NETINET_IN_H \
	-DHAVE_SYS_IOCTL_H \
	-DHAVE_SYS_MMAN_H \
	-DHAVE_SYS_MOUNT_H \
	-DHAVE_SYS_PRCTL_H \
	-DHAVE_SYS_RESOURCE_H \
	-DHAVE_SYS_SELECT_H \
	-DHAVE_SYS_STAT_H \
	-DHAVE_SYS_TYPES_H \
	-DHAVE_STDLIB_H \
	-DHAVE_STRDUP \
	-DHAVE_MMAP \
	-DHAVE_UTIME_H \
	-DHAVE_GETPAGESIZE \
	-DHAVE_LSEEK64 \
	-DHAVE_LSEEK64_PROTOTYPE \
	-DHAVE_EXT2_IOCTLS \
	-DHAVE_LINUX_FD_H \
	-DHAVE_TYPE_SSIZE_T \
	-DHAVE_SYS_TIME_H \
        -DHAVE_SYS_PARAM_H \
	-DHAVE_SYSCONF





LOCAL_CPPFLAGS := -std=c++11 -pthread -frtti -fexceptions

LOCAL_CLANG=true 

LOCAL_MODULE    := zslib_android

LOCAL_EXPORT_C_INCLUDES:= $(LOCAL_PATH) \

$(warning $(LOCAL_PATH))
LOCAL_C_INCLUDES:= \
$(ANDROIDNDK_PATH)/sources/android/support/include \
$(ANDROIDNDK_PATH)/sources/cxx-stl/llvm-libc++/libcxx/include \
$(ANDROIDNDK_PATH)/platforms/android-19/arch-arm/usr/include \
$(LOCAL_PATH) \
$(LOCAL_PATH)/zsLib/extras \



LOCAL_SRC_FILES := zsLib/cpp/zsLib.cpp \
zsLib/cpp/zsLib_Event.cpp \
zsLib/cpp/zsLib_Exception.cpp \
zsLib/cpp/zsLib_helpers.cpp \
zsLib/cpp/zsLib_IPAddress.cpp \
zsLib/cpp/zsLib_Log.cpp \
zsLib/cpp/zsLib_MessageQueue.cpp \
zsLib/cpp/zsLib_MessageQueueThread.cpp \
zsLib/cpp/zsLib_MessageQueueThreadBasic.cpp \
zsLib/cpp/zsLib_MessageQueueThreadUsingBlackberryChannels.cpp \
zsLib/cpp/zsLib_MessageQueueThreadUsingCurrentGUIMessageQueueForWindows.cpp \
zsLib/cpp/zsLib_MessageQueueThreadUsingMainThreadMessageQueueForApple.cpp \
zsLib/cpp/zsLib_Numeric.cpp \
zsLib/cpp/zsLib_Proxy.cpp \
zsLib/cpp/zsLib_Socket.cpp \
zsLib/cpp/zsLib_SocketMonitor.cpp \
zsLib/cpp/zsLib_String.cpp \
zsLib/cpp/zsLib_Stringize.cpp \
zsLib/cpp/zsLib_Timer.cpp \
zsLib/cpp/zsLib_TimerMonitor.cpp \
zsLib/cpp/zsLib_XML.cpp \
zsLib/cpp/zsLib_XMLAttribute.cpp \
zsLib/cpp/zsLib_XMLComment.cpp \
zsLib/cpp/zsLib_XMLDeclaration.cpp \
zsLib/cpp/zsLib_XMLDocument.cpp \
zsLib/cpp/zsLib_XMLElement.cpp \
zsLib/cpp/zsLib_XMLGenerator.cpp \
zsLib/cpp/zsLib_XMLNode.cpp \
zsLib/cpp/zsLib_XMLParser.cpp \
zsLib/cpp/zsLib_XMLParserPos.cpp \
zsLib/cpp/zsLib_XMLParserWarningTypes.cpp \
zsLib/cpp/zsLib_XMLText.cpp \
zsLib/cpp/zsLib_XMLUnknown.cpp \
zsLib/extras/uuid/clear.c \
zsLib/extras/uuid/compare.c \
zsLib/extras/uuid/copy.c \
zsLib/extras/uuid/gen_uuid.c \
zsLib/extras/uuid/isnull.c \
zsLib/extras/uuid/pack.c \
zsLib/extras/uuid/parse.c \
zsLib/extras/uuid/unpack.c \
zsLib/extras/uuid/unparse.c \
zsLib/extras/uuid/uuid_time.c \

include $(BUILD_STATIC_LIBRARY)

