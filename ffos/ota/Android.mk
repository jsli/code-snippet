LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= src/main.cpp \
    src/download/DownloadTask.cpp \
    src/download/Downloader.cpp \
	src/json/json_reader.cpp \
	src/json/json_value.cpp \
	src/json/json_writer.cpp \
	src/net/HttpClient.cpp \
	src/ota/daemon/CheckDaemon.cpp \
	src/ota/daemon/OtaDaemon.cpp \
	src/ota/daemon/ServerDaemonImpl.cpp \
	src/ota/net/HttpParameter.cpp \
	src/ota/net/HttpResult.cpp \
	src/ota/message/Message.cpp \
	src/ota/message/OtaMessage.cpp \
	src/ota/message/SystemMessage.cpp \
	src/socket/ClientSocket.cpp \
	src/socket/ServerDaemon.cpp \
	src/socket/ServerSocket.cpp \
	src/platform/DeviceInfo.cpp \
	src/platform/RecoveryManager.cpp \
    src/platform/SystemProperty.cpp \
    src/platform/ProcessMonitor.cpp \
    src/platform/mtd/mtdutils.c \
    src/utils/FsUtils.cpp \
    src/utils/StringUtils.cpp \
    src/utils/TimeUtils.cpp \
    src/utils/Md5.cpp \
    src/thread/Thread.cpp \
    src/event/Event.cpp \
    src/event/EventTarget.cpp \
    src/event/EventLooper.cpp

LOCAL_C_INCLUDES += \
	frameworks/base/cmds/otadv2/include \
	frameworks/base/include \
	ndk/sources/cxx_stl/gnu-libstdc++/4.6/libs/armeabi-v7a/include \
	ndk/sources/cxx_stl/gnu-libstdc++/4.6/include \
	frameworks/base/cmds/otad/librecovery

LOCAL_SHARED_LIBRARIES := libcutils liblog libstlport librecovery libcurl

LOCAL_CFLAGS := -DHAVE_CONFIG_H -DANDROID -DEXPAT_RELATIVE_PATH -DALLOW_QUOTED_COOKIE_VALUES -DCOMPONENT_BUILD -DGURL_DLL 

LOCAL_CPPFLAGS += -fexceptions  -fno-rtti

LOCAL_STATIC_LIBRARIES := libsupc++ 

#LOCAL_LDFLAGS += -L$(prebuilt_stdcxx_PATH)/thumb  -lsupc++ 

LOCAL_MODULE:= otadv2
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)
LOCAL_MODULE_TAGS := eng
include external/stlport/libstlport.mk #shit here?
include $(BUILD_EXECUTABLE)

