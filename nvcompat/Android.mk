LOCAL_PATH := $(call my-dir)/extensions/src
SRC_FILES := $(LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE  := libnvcompat

BUILD_FILES_PATH  :=  $(LOCAL_PATH)/NvGLUtils \
					  $(LOCAL_PATH)/NvEGLUtil \
                      $(LOCAL_PATH)/NvAssetLoader \
                      $(LOCAL_PATH)/NvGamePad \
                      $(LOCAL_PATH)/NvAppBase
                      
                      
BUILD_FILES_SUFFIX := %.cpp

ALL_BUILD_FILES := $(foreach src_path,$(BUILD_FILES_PATH), $(shell find $(src_path) -type f) ) 
ALL_BUILD_FILES := $(ALL_BUILD_FILES:$(CPP_PATH)/./%=$(CPP_PATH)%)
ALL_SRC_LIST  := $(filter $(BUILD_FILES_SUFFIX),$(ALL_BUILD_FILES)) 
ALL_SRC_LIST  := $(ALL_SRC_LIST:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := NvAppBase/NvAndroidNativeAppGlue.c
LOCAL_SRC_FILES += $(ALL_SRC_LIST)

LOCAL_CFLAGS := -frtti -D_LIB -DGL_API_LEVEL_ES2 -DANDROID -I$(LOCAL_PATH)/ -I$(LOCAL_PATH)/NvAppBase -I$(LOCAL_PATH)/../include

ifndef NDEBUG
LOCAL_CFLAGS += -fpic -fPIC -ffunction-sections -funwind-tables -fstack-protector -no-canonical-prefixes -O2 -g -fomit-frame-pointer -fstrict-aliasing -funswitch-loops -finline-limit=300
else
LOCAL_CFLAGS += -fpic -fPIC -ffunction-sections -funwind-tables -fstack-protector -no-canonical-prefixes -O2 -fomit-frame-pointer -fstrict-aliasing -funswitch-loops -finline-limit=300 
endif

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS += -march=armv7-a
else
LOCAL_CFLAGS += -malign-double
endif

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include

include $(BUILD_STATIC_LIBRARY)

