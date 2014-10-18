#ifndef _ANDROID_JNI_TOOLS_
#define _ANDROID_JNI_TOOLS_
#if defined _ANDROID
#include <android_native_app_glue.h>
#include <jni.h>
#include <string>

namespace android
{
	extern const std::string getCoreConfigDir(android_app* app);
}

#endif /*_ANDROID*/
#endif /*_ANDROID_JNI_TOOLS_*/
