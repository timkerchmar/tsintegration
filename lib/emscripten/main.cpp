#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <TSIntegration.h>
#include <TSThread.h>

EGLDisplay display = EGL_NO_DISPLAY;
EGLSurface surface = EGL_NO_SURFACE;
EGLContext context = EGL_NO_CONTEXT;
EGLConfig config = NULL;

void TSIntegration::createWindow()
{
}

void TSIntegration::setPixelFormat()
{
	
	
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, 0, 0);

    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };

	EGLint numConfigs;
    eglChooseConfig(display, attribs, nullptr,0, &numConfigs);
   	EGLConfig* supportedConfigs = new EGLConfig[numConfigs];
    eglChooseConfig(display, attribs, supportedConfigs, numConfigs, &numConfigs);
   	config = supportedConfigs[0];
   	for (int i = 0; i < numConfigs; i++)
	{
       	EGLConfig& cfg = supportedConfigs[i];
       	EGLint r, g, b, d;
       	if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r)   &&
           	eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
           	eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b)  &&
           	eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) &&
           	r == 8 && g == 8 && b == 8 && d == 0 )
		{
           	config = supportedConfigs[i];
           	break;
       	}
   	}
   	delete[] supportedConfigs;
}

void TSIntegration::createContext()
{

    surface = eglCreateWindowSurface(display, config, 0, NULL);

    int attrib_list[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    context = eglCreateContext(display, config, NULL, attrib_list);
}

void TSIntegration::makeContextCurrent()
{
    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        printf("Unable to eglMakeCurrent\n");
        return;
    } else {
        printf ("context is now current\n");
    }
}

void TSIntegration::clearContext()
{
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

void TSIntegration::enableDisplay()
{
    if (display == EGL_NO_DISPLAY) {
        setPixelFormat();
        createContext();
        makeContextCurrent();
    }
}

void TSIntegration::disableDisplay()
{
    printf("term_display\n");

    if (display != EGL_NO_DISPLAY) {
		clearContext();
        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
        }
        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
        }
        eglTerminate(display);
    }
    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;
}

void TSIntegration::displaySize(GLint& width, GLint& height)
{
    eglQuerySurface(display, surface, EGL_WIDTH, &width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &height);
}

void TSIntegration::setSwapInterval()
{
    eglSwapInterval(display, 1);
}

void TSIntegration::pageFlip()
{
    eglSwapBuffers(display, surface);
}

int main() {
    TSIntegration::initialize();
    TSIntegration::start();
    return 0;
}