#ifndef _ANDROID_GLUE_COMPAT_
#define _ANDROID_GLUE_COMPAT_
#if defined _ANDROID

#include "AndroidGLESCompat.h"
#include "AndroidJNITools.h"

#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "ygopro-native", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "ygopro-native", __VA_ARGS__))

#endif /*_ANDROID*/
#endif /*_ANDROID_GLUE_COMPAT_*/
