#ifndef _ANDROID_GLES_COMPAT_
#define _ANDROID_GLES_COMPAT_
#if defined _ANDROID

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES/glplatform.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2.h>

#define BUILD_WITH_GLES

extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArrays;

#endif /*_ANDROID*/
#endif /*_ANDROID__GLESCOMPAT_*/
