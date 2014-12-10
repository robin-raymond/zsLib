LOCAL_PATH := $(call my-dir)/../../

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_CFLAGS	:= -Wall \
-W \
-O2 \
-pipe \
-fPIC \
-D_ANDROID \

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

include $(BUILD_STATIC_LIBRARY)

