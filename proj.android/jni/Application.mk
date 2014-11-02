APP_ABI := x86 armeabi-v7a
APP_PLATFORM := android-14
APP_MODULES := YGOPro
APP_STL := gnustl_static
NDK_TOOLCHAIN_VERSION=4.8
APP_CPPFLAGS := -Wno-error=format-security -std=c++11 -fpermissive -DANDROID
APP_OPTIM := debug
