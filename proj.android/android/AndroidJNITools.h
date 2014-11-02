#ifndef _ANDROID_JNI_TOOLS_
#define _ANDROID_JNI_TOOLS_
#if defined _ANDROID
#include <jni.h>
#include <string>
#include <NvAppBase/NvPlatformContext.h>

namespace android
{
	extern const std::string getCoreConfigDir(NvPlatformContext* context);
}

#endif /*_ANDROID*/
#endif /*_ANDROID_JNI_TOOLS_*/
