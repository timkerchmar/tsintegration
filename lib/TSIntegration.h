
#ifndef TSIntegration_H
#define TSIntegration_H

//=====================================================================================
// LIBRARY CONSUMING APP MUST DEFINE THESE CALLBACKS
//=====================================================================================
void TSInitialized();
void TSDisplayChanged(int width, int height);
void TSRenderFrame();
void TSTouchAdded(float x, float y, int pointerIndex);
void TSTouchRemoved(float x, float y, int pointerIndex);
void TSTouchMoved(float x, float y, int pointerIndex);
void TSAccelerometerChanged(float x, float y, float z);

//=====================================================================================
// OPENGL OPENGL, OH WHERE IS MY OPENGL?
//=====================================================================================
#if defined(WIN32)
#include <windows.h>
#include <gl\glew.h>
#include <gl\wglew.h>
#include <gl\GL.h>
#include <gl\GLU.h>

#elif defined(ANDROID)
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#elif TARGET_OS_OSX && !TARGET_OS_SIMULATOR
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>

#else
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif

//=====================================================================================
// PLATFORM SUPPORT API. USER SHOULD NOT HAVE TO TOUCH THIS CLASS
//=====================================================================================
class TSIntegration
{
public:
    // Platform integration calls these
	static void initialize();
    static void start();
    static void stop();

	// Platform implements these
	static void createWindow(); // main thread should prepare an OpenGL compatible surface
	static void setPixelFormat(); // rendering thread should prepare an OpenGL compatible surface
    static void createContext(); // rendering thread should prepare an OpenGL context
    static void makeContextCurrent(); // make OpenGL context current to rendering thread
    static void clearContext(); // disable OpenGL context on rendering thread
    static void enableDisplay(); // rendering thread callback each time a new valid display size is known
    static void disableDisplay(); // rendering thread callback for disabled display
	static void displaySize(int& width, int& height); // rendering thread callback to access display dimensions
	static void pageFlip(); // rendering thread pageFlip callback
    static void setSwapInterval(); // rendering thread should set a pageFlip interval
};

#endif
