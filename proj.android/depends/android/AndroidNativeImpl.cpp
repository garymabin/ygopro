#include "AndroidGlueCompat.h"
#include <unistd.h>

struct android_app* mainApp;

#if defined BUILD_WITH_GLES
 	PFNGLGENVERTEXARRAYSOESPROC glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC) eglGetProcAddress("glGenVertexArraysOES");
    PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC) eglGetProcAddress("glBindVertexArrayOES");
    PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSOESPROC) eglGetProcAddress("glDeleteVertexArraysOES");
#endif

static bool testEGLError()
{
#if defined(_DEBUG)
	EGLint g = eglGetError();
	switch (g)
	{
		case EGL_SUCCESS:
			return false;
		case EGL_NOT_INITIALIZED :
			LOGW("Not Initialized");
			break;
		case EGL_BAD_ACCESS:
			LOGW("Bad Access");
			break;
		case EGL_BAD_ALLOC:
			LOGW("Bad Alloc");
			break;
		case EGL_BAD_ATTRIBUTE:
			LOGW("Bad Attribute");
			break;
		case EGL_BAD_CONTEXT:
			LOGW("Bad Context");
			break;
		case EGL_BAD_CONFIG:
			LOGW("Bad Config");
			break;
		case EGL_BAD_CURRENT_SURFACE:
			LOGW("Bad Current Surface");
			break;
		case EGL_BAD_DISPLAY:
			LOGW("Bad Display");
			break;
		case EGL_BAD_SURFACE:
			LOGW("Bad Surface");
			break;
		case EGL_BAD_MATCH:
			LOGW("Bad Match");
			break;
		case EGL_BAD_PARAMETER:
			LOGW("Bad Parameter");
			break;
		case EGL_BAD_NATIVE_PIXMAP:
			LOGW("Bad Native Pixmap");
			break;
		case EGL_BAD_NATIVE_WINDOW:
			LOGW("Bad Native Window");
			break;
		case EGL_CONTEXT_LOST:
			LOGW("Context Lost");
			break;
	};
	return true;
#else
	return false;
#endif
}

/**
 * Initialize an EGL context for the current display.
 */
static int engine_init_display(struct engine* engine) {
	LOGI("engine_init_display");
	// initialize OpenGL ES and EGL

	/*
	 * Here specify the attributes of the desired configuration.
	 * Below, we select an EGLConfig with at least 8 bits per color
	 * component compatible with on-screen windows
	 */
	const EGLint contextAttribs[] =
	{
	    EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, 0
	};

	EGLint attribs[] =
		{
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 0,
			EGL_BUFFER_SIZE, 24,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_DEPTH_SIZE, 16,
			EGL_STENCIL_SIZE, 0,
			EGL_SAMPLE_BUFFERS, 0,
			EGL_SAMPLES, 0,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_NONE, 0
		};

	EGLint w, h, dummy, format;
	EGLint numConfigs;
	EGLConfig config;
	int Steps = 5;
	int eglMajorVersion, eglMinorVersion;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, &eglMajorVersion, &eglMinorVersion);
	LOGI("EGL version %d.%d", eglMajorVersion, eglMinorVersion);

	// Choose the best EGL config.
	while (!eglChooseConfig(display, attribs, &config, 1, &numConfigs) || !numConfigs)
	{
		switch (Steps)
		{
		case 5: // samples
			if (attribs[19] > 2)
				--attribs[19];
			else
			{
				attribs[17] = 0;
				attribs[19] = 0;
				--Steps;
			}
			break;
		case 4: // alpha
			if (attribs[7])
			{
				attribs[7] = 0;

				if (0)
				{
					attribs[17] = 1;
					attribs[19] = 0;
					Steps = 5;
				}
			}
			else
				--Steps;
			break;
		case 3: // stencil
			if (attribs[15])
			{
				attribs[15] = 0;

				if (0)
				{
					attribs[17] = 1;
					attribs[19] = 0;
					Steps = 5;
				}
			}
			else
				--Steps;
			break;
		case 2: // depth size
			if (attribs[13] > 16)
			{
				attribs[13] -= 8;
			}
			else
				--Steps;
			break;
		case 1: // buffer size
			if (attribs[9] > 16)
			{
				attribs[9] -= 8;
			}
			else
				--Steps;
			break;
		default:
			LOGI("Could not get config for EGL display.");
			return false;
		}
	}
	if (0 && !attribs[17])
		LOGI("No multisampling.");

	if (0 && !attribs[7])
		LOGI("No alpha.");

	if (0 && !attribs[15])
		LOGI("No stencil buffer.");

	if (attribs[13] < 16)
		LOGI("No full depth buffer.");

	if (attribs[9] < 24)
		LOGI("No full color buffer.");

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	 * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	 * As soon as we picked a EGLConfig, we can safely reconfigure the
	 * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, engine->app->window,
			NULL);
	if (eglMinorVersion > 1) {
		eglBindAPI(EGL_OPENGL_ES_API);
	}
	context = eglCreateContext(display, config, NULL, contextAttribs);
	if (testEGLError()) {
		LOGW("Could not create EGL context.");
		return -1;
	}

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		testEGLError();
		LOGW("Unable to eglMakeCurrent");
		return -1;
	}

	// Initialize GL state.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable (GL_CULL_FACE);
//	glShadeModel (GL_SMOOTH);
	glDisable (GL_DEPTH_TEST);

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	engine->display = display;
	engine->context = context;
	engine->surface = surface;
	engine->width = w;
	engine->height = h;
	engine->state.angle = 0;

	engine->animating = 1;

	return 0;
}

/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine* engine) {
	if (engine->display == NULL) {
		// No display.
		return;
	}

	// Just fill the screen with a color.
	glClearColor(((float) engine->state.x) / engine->width, engine->state.angle,
			((float) engine->state.y) / engine->height, 1);
	glClear (GL_COLOR_BUFFER_BIT);

	eglSwapBuffers(engine->display, engine->surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
void engine_term_display(struct engine* engine) {
	if (engine->display != EGL_NO_DISPLAY) {
		eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE,
				EGL_NO_CONTEXT);
		if (engine->context != EGL_NO_CONTEXT) {
			eglDestroyContext(engine->display, engine->context);
		}
		if (engine->surface != EGL_NO_SURFACE) {
			eglDestroySurface(engine->display, engine->surface);
		}
		eglTerminate(engine->display);
	}
	engine->animating = 0;
	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app,
		AInputEvent* event) {
	struct engine* engine = (struct engine*) app->userData;
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		engine->state.x = AMotionEvent_getX(event, 0);
		engine->state.y = AMotionEvent_getY(event, 0);
		return 1;
	}
	return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
	struct engine* engine = (struct engine*) app->userData;
	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		LOGI("APP_CMD_SAVE_STATE");
		// The system has asked us to save our current state.  Do so.
		engine->app->savedState = malloc(sizeof(struct saved_state));
		*((struct saved_state*) engine->app->savedState) = engine->state;
		engine->app->savedStateSize = sizeof(struct saved_state);
		break;
	case APP_CMD_INIT_WINDOW:
		LOGI("APP_CMD_INIT_WINDOW");
		// The window is being shown, get it ready.
		if (engine->app->window != NULL) {
			engine_init_display(engine);
			engine_draw_frame(engine);
		}
		break;
	case APP_CMD_TERM_WINDOW:
		LOGI("APP_CMD_TERM_WINDOW");
		// The window is being hidden or closed, clean it up.
		engine_term_display(engine);
		break;
	case APP_CMD_GAINED_FOCUS:
		LOGI("APP_CMD_GAINED_FOCUS");
		// When our app gains focus, we start monitoring the accelerometer.
		if (engine->accelerometerSensor != NULL) {
			ASensorEventQueue_enableSensor(engine->sensorEventQueue,
					engine->accelerometerSensor);
			// We'd like to get 60 events per second (in us).
			ASensorEventQueue_setEventRate(engine->sensorEventQueue,
					engine->accelerometerSensor, (1000L / 60) * 1000);
		}
		break;
	case APP_CMD_LOST_FOCUS:
		LOGI("APP_CMD_LOST_FOCUS");
		// When our app loses focus, we stop monitoring the accelerometer.
		// This is to avoid consuming battery while not being used.
		if (engine->accelerometerSensor != NULL) {
			ASensorEventQueue_disableSensor(engine->sensorEventQueue,
					engine->accelerometerSensor);
		}
		// Also stop animating.
		engine->animating = 0;
		engine_draw_frame(engine);
		break;
	}
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void init_android_world(struct android_app* state, struct engine* engine) {
	// Make sure glue isn't stripped.
	app_dummy();

	mainApp = state;

	memset(engine, 0, sizeof(struct engine));
	state->userData = engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	engine->app = state;

	// Prepare to monitor accelerometer
	engine->sensorManager = ASensorManager_getInstance();
	engine->accelerometerSensor = ASensorManager_getDefaultSensor(
			engine->sensorManager, ASENSOR_TYPE_ACCELEROMETER);
	engine->sensorEventQueue = ASensorManager_createEventQueue(
			engine->sensorManager, state->looper, LOOPER_ID_USER, NULL, NULL);

	if (state->savedState != NULL) {
		// We are starting with a previous saved state; restore from it.
		engine->state = *(struct saved_state*) state->savedState;
	}
	engine->animating = 0;
	const std::string working_dir = android::getCoreConfigDir(state);
	chdir(working_dir.c_str());
}
