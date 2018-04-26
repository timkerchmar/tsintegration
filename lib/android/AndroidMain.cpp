#include <initializer_list>
#include <android/sensor.h>
#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <TSIntegration.h>
#include <TSThread.h>

struct android_app* app = NULL;
ASensorManager* sensorManager = NULL;
const ASensor* gyroscopicSensor = NULL;
ASensorEventQueue* sensorEventQueue = NULL;
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

    surface = eglCreateWindowSurface(display, config, app->window, NULL);

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

int api_version( struct android_app *app ) {

    JNIEnv* env;
    app->activity->vm->AttachCurrentThread( &env, NULL );

    // VERSION is a nested class within android.os.Build (hence "$" rather than "/")
    jclass versionClass = env->FindClass("android/os/Build$VERSION" );
    jfieldID sdkIntFieldID = env->GetStaticFieldID(versionClass, "SDK_INT", "I" );

    int sdkInt = env->GetStaticIntField(versionClass, sdkIntFieldID );
    app->activity->vm->DetachCurrentThread();
    return sdkInt;
}

void AutoHideNavBar(struct android_app* state)
{
    if (api_version(state) < 19) return;

    JNIEnv* env;
    state->activity->vm->AttachCurrentThread(&env, NULL);

    jclass activityClass = env->FindClass("android/app/NativeActivity");
    jmethodID getWindow = env->GetMethodID(activityClass, "getWindow", "()Landroid/view/Window;");

    jclass windowClass = env->FindClass("android/view/Window");
    jmethodID getDecorView = env->GetMethodID(windowClass, "getDecorView", "()Landroid/view/View;");

    jclass viewClass = env->FindClass("android/view/View");
    jmethodID setSystemUiVisibility = env->GetMethodID(viewClass, "setSystemUiVisibility", "(I)V");

    jobject window = env->CallObjectMethod(state->activity->clazz, getWindow);

    jobject decorView = env->CallObjectMethod(window, getDecorView);

    jfieldID flagFullscreenID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_FULLSCREEN", "I");
    jfieldID flagHideNavigationID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_HIDE_NAVIGATION", "I");
    jfieldID flagImmersiveID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_IMMERSIVE_STICKY", "I");

    int flagFullscreen = env->GetStaticIntField(viewClass, flagFullscreenID);
    int flagHideNavigation = env->GetStaticIntField(viewClass, flagHideNavigationID);
    int flagImmersive = env->GetStaticIntField(viewClass, flagImmersiveID);

    int flag = flagFullscreen | flagHideNavigation | flagImmersive;

    env->CallVoidMethod(decorView, setSystemUiVisibility, flag);

    state->activity->vm->DetachCurrentThread();
}

//=====================================================================================
// PLATFORM EVENT SCAFFOLDING
//=====================================================================================

/**
 * Process the next input event.
 */
static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    struct engine* engine = (struct engine*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {

        switch(AInputEvent_getSource(event)){
            case AINPUT_SOURCE_TOUCHSCREEN:
                int actionEvent = AKeyEvent_getAction(event);

                int pointerIndex = (actionEvent & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                        >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                int pointerId = AMotionEvent_getPointerId(event, pointerIndex);

                int action = actionEvent & AMOTION_EVENT_ACTION_MASK;
                switch(action){
                    case AMOTION_EVENT_ACTION_DOWN: {
                        TSTouchAdded(AMotionEvent_getX(event, pointerIndex),
                                            AMotionEvent_getY(event, pointerIndex), pointerId);

                    }
                        break;
                    case AMOTION_EVENT_ACTION_UP: {
                        TSTouchRemoved(AMotionEvent_getX(event, pointerIndex),
                                              AMotionEvent_getY(event, pointerIndex), pointerId);
                    }
                        break;
                    case AMOTION_EVENT_ACTION_MOVE: {
                        int count = AMotionEvent_getPointerCount(event);
                        for (int i = 0; i < count; ++i)
                        {
                            TSTouchMoved(AMotionEvent_getX(event, i),
                                                 AMotionEvent_getY(event, i),
                                                 AMotionEvent_getPointerId(event, i));
                        }
                    }
                        break;
                    case AMOTION_EVENT_ACTION_POINTER_DOWN: {
                        TSTouchAdded(AMotionEvent_getX(event, pointerIndex),
                                          AMotionEvent_getY(event, pointerIndex), pointerId);
                    }
                        break;
                    case AMOTION_EVENT_ACTION_POINTER_UP: {
                        TSTouchRemoved(AMotionEvent_getX(event, pointerIndex),
                                              AMotionEvent_getY(event, pointerIndex), pointerId);
                    }
                        break;
                }
                break;
        } // end switch

        return 1;
    }
    return 0;
}

/**
 * Process the next main command.
 */
static void handle_cmd(struct android_app* app, int32_t cmd) {
    printf("got command %i\n", cmd);

    switch (cmd)
    {
        case APP_CMD_RESUME:
            //AutoHideNavBar(app);

        case APP_CMD_CONFIG_CHANGED:
            break;
        case APP_CMD_INIT_WINDOW:
            TSIntegration::initialize();
            TSIntegration::start();
            break;
        case APP_CMD_TERM_WINDOW:
            TSIntegration::stop();
            break;

        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus, we start monitoring the accelerometer.
            if (gyroscopicSensor != NULL) {
                ASensorEventQueue_enableSensor(sensorEventQueue,
                                               gyroscopicSensor);
                // We'd like to get 60 events per second (in us).
                ASensorEventQueue_setEventRate(sensorEventQueue,
                                               gyroscopicSensor,
                                               (1000L/30)*1000);
            }
            break;
        case APP_CMD_LOST_FOCUS:
            // When our app loses focus, we stop monitoring the accelerometer.
            // This is to avoid consuming battery while not being used.
            if (gyroscopicSensor != NULL) {
                ASensorEventQueue_disableSensor(sensorEventQueue,
                                                gyroscopicSensor);
            }
            break;
    }
}

void android_main(struct android_app* state)
{
    printf("========================== ENTERED MAIN \"==========================\n");

    // Android Studio will tell lies that this is deprecated and unnecessary.
    // Don't listen to the lies. The native linkage will break if this is removed.
    app_dummy();

    AutoHideNavBar(state);

    state->onAppCmd = handle_cmd;
    state->onInputEvent = handle_input;
    app = state;

    // Prepare to monitor accelerometer
    sensorManager = ASensorManager_getInstance();
    gyroscopicSensor = ASensorManager_getDefaultSensor(
            sensorManager,
            ASENSOR_TYPE_ACCELEROMETER);
    sensorEventQueue = ASensorManager_createEventQueue(
            sensorManager,
            state->looper, LOOPER_ID_USER,
            NULL, NULL);

    for(;;) {
        int ident, events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(-1, NULL, &events,
                                      (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // If a sensor has data, process it now.
            if (ident == LOOPER_ID_USER) {
                if (gyroscopicSensor != NULL) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(sensorEventQueue,
                                                       &event, 1) > 0) {
                        TSAccelerometerChanged(event.acceleration.x, event.acceleration.y, event.acceleration.z);
                    }
                }
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {

                ASensorManager_destroyEventQueue(sensorManager, sensorEventQueue);

                return;
            }
        }
    }
}