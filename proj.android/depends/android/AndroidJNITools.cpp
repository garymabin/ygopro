#include "AndroidJNITools.h"

namespace android {
const std::string getCoreConfigDir(android_app* app) {
	const std::string ret;
	if (!app || !app->activity || !app->activity->vm)
		return ret;

	JNIEnv* jni = 0;
	app->activity->vm->AttachCurrentThread(&jni, NULL);
	if (!jni)
		return ret;

	jobject lNativeActivity = app->activity->clazz;
	jclass ClassNativeActivity = jni->GetObjectClass(lNativeActivity);
	jmethodID MethodGetApp = jni->GetMethodID(ClassNativeActivity,
			"getApplication", "()Landroid/app/Application;");
	jobject application = jni->CallObjectMethod(lNativeActivity, MethodGetApp);
	jclass classApp = jni->GetObjectClass(application);
	jmethodID coreConfigMethod = jni->GetMethodID(classApp, "getCoreConfigDir",
			"()Ljava/lang/String;");

	jstring retString = (jstring) jni->CallObjectMethod(application,
			coreConfigMethod);
	jni->DeleteLocalRef(classApp);
	jni->DeleteLocalRef(ClassNativeActivity);

	const char* chars = jni->GetStringUTFChars(retString, NULL);
	ret.append(chars);

	jni->ReleaseStringUTFChars(retString, chars);
	app->activity->vm->DetachCurrentThread();

	return ret;
}
}
