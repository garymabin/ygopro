LOCAL_PATH := $(call my-dir)/..

include $(CLEAR_VARS)

LOCAL_MODULE    := YGOPro
BUILDIN_C_FILES := $(wildcard $(LOCAL_PATH)/../buildin/*.c)
NETC_SRC_FILES := $(wildcard $(LOCAL_PATH)/../netc/*.cpp)
NETC_SRC_FILES += $(wildcard $(LOCAL_PATH)/../netc/bs/*.cpp)
NETC_SRC_FILES += $(wildcard $(LOCAL_PATH)/../netc/ds/*.cpp)
NETS_SRC_FILES := $(wildcard $(LOCAL_PATH)/../nets/*.cpp)
COMMON_SRC_FILES := $(wildcard $(LOCAL_PATH)/../common/*.cpp)
#GS_SRC_FILES := $(wildcard $(LOCAL_PATH)/../gs/*.cpp)


LOCAL_SRC_FILES := $(BUILDIN_C_FILES)
LOCAL_SRC_FILES += $(NETC_SRC_FILES)
LOCAL_SRC_FILES += $(NETS_SRC_FILES)
LOCAL_SRC_FILES += $(COMMON_SRC_FILES)
#LOCAL_SRC_FILES += $(GS_SRC_FILES)
LOCAL_SRC_FILES += $(LOCAL_PATH)/jni/YGOProApp.cpp


LOCAL_CFLAGS := -pipe -frtti -fexceptions -fstrict-aliasing -D_ANDROID

ifndef NDEBUG
LOCAL_CFLAGS += -g -D_DEBUG
else
LOCAL_CFLAGS += -fexpensive-optimizations -O3 
endif

ifeq ($(TARGET_ARCH_ABI),x86)
LOCAL_CFLAGS += -fno-stack-protector
LOCAL_CFLAGS += -malign-double
endif

LOCAL_C_INCLUDES := $(LOCAL_PATH)/jni
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../depends/freetype/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../depends/sqlite3
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../depends/libevent/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../nvcompat/extensions/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/android
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../depends


LOCAL_LDLIBS := -lEGL -llog -lGLESv1_CM -lGLESv2 -lz -landroid

LOCAL_STATIC_LIBRARIES += libnvcompat
LOCAL_STATIC_LIBRARIES += libssl_static
LOCAL_STATIC_LIBRARIES += libcrypto_static
LOCAL_STATIC_LIBRARIES += libevent2
LOCAL_STATIC_LIBRARIES += libocgcore_static
LOCAL_STATIC_LIBRARIES += liblua5.2
LOCAL_STATIC_LIBRARIES += sqlite3
LOCAL_STATIC_LIBRARIES += libft2

include $(BUILD_SHARED_LIBRARY)

$(call import-add-path,$(LOCAL_PATH)/../depends)
$(call import-add-path,$(LOCAL_PATH)/../)
$(call import-module, nvcompat)
$(call import-module, openssl)
$(call import-module, libevent)
$(call import-module, sqlite3)
$(call import-module, ocgcore)
$(call import-module, lua)
$(call import-module, freetype)

