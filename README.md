# TSIntegration

Get a full screen OpenGL surface, receiving touches and accelerometer input, with a separate rendering thread on Windows, Android, OSX, iOS.

Intended as a starter kit for a multiplatform game and a reference for handling the host OS display related callbacks.
 
## Running the Painter sample

Open the project file for the platform you want to test on and hit run. You'll probably need to wade through some developer identity stuff for the mobile platforms, or set up an Android app configuration in Android Studio.

## API

The API is based upon callbacks the app must implement. Never access OpenGL from the main thread.
<br><br><br>
```
void TSInitialized();
```
Called once on the render thread after the application starts. It is safe to call OpenGL from this callback.
<br><br>
```
void TSDisplayChanged(int newWidth, int newHeight);
```
Called on the render thread each time the app window changes dimensions, including once after TSInitialized is first called. When the arguments are zeros the application was backgrounded. It is safe to call OpenGL from this callback.
<br><br>
```
void TSRenderFrame();
```
Called on the render thread as often as the renderer is idle and the display is valid, limited by vsync. It is safe to call OpenGL from this callback.
<br><br>
```
void TSTouchAdded(float x, float y, int pointerIndex);
```
Called on the main/UI thread directly from the host OS callback. pointerIndex can be used to track subsequent updates to a particular touch. The same pointerIndex may be reused by future touches after being released.
<br><br>
```
void TSTouchMoved(float x, float y, int pointerIndex);
```
Called on the main/UI thread when a touch moved.
<br><br>
```
void TSTouchRemoved(float x, float y, int pointerIndex)
```
Called on the main/UI thread when a touch ended, or a mouse button was released.
<br><br>
```
void TSAccelerometerChanged(float x, float y, float z);
```
Called on the main/UI thread when the accelerometer changed values. The desktops currently provide no way to test the accelerometer.
<br><br>


## Platform notes:

Android: There is commented out code in the APP_CMD_RESUME case. Android's native glue thread handling is super wonky around app  backgrounding and it crashes saying the thread that created the view is the only one that can modify it. That commented out code was intended to re-hide the navigation bar after resuming, like it is when it first launches.

iOS: No share groups are required and no passing of a context around between threads during initialization. No CADisplayLink and no NSTimer. There's so much misinformation out there about how to go about setting up a non main thread OpenGL setup.

OSX: Standard keyboard shortcuts like command + Q need to be re-enabled somehow.

Desktop: Support all mouse buttons and mouse wheel.
