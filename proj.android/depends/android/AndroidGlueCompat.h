#ifndef _ANDROID_GLUE_COMPAT_
#define _ANDROID_GLUE_COMPAT_
#if defined _ANDROID

#include "AndroidGLESCompat.h"
#include "AndroidJNITools.h"


#include <android_native_app_glue.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/sensor.h>
#include <android/log.h>


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "ygopro-native", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "ygopro-native", __VA_ARGS__))

/**
 * Our saved state data.
 */
struct saved_state {
    float angle;
    int32_t x;
    int32_t y;
};

/**
 * Shared state for our app.
 */
typedef struct engine {
    struct android_app* app;

    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;

    int animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    struct saved_state state;
} AndGameEngine;

extern void init_android_world(struct android_app* state, struct engine* engine);

extern void engine_term_display(struct engine* engine);

extern struct android_app* mainApp;


#endif /*_ANDROID*/
#endif /*_ANDROID_GLUE_COMPAT_*/
