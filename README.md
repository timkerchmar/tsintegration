# TSIntegration

Get a full screen OpenGL surface, with a separate rendering thread receiving touches and accelerometer input on Windows, Android, OSX, iOS.

Intended as a starter kit for a multiplatform game.
 
## Running the Painter sample

Open the project file for the platform you want to test on and hit run. You'll probably need to wade through some developer identity stuff for the mobile platforms, or set up an Android app configuration in Android Studio.

## API

The API is based upon callbacks the app must implement.

```
void TSInitialized();
```
Called once on the render thread after the application starts. The OpenGL render context is valid inside the callback.

```
void TSDisplayChanged(int newWidth, int newHeight);
```
Called on the render thread each time the app window changes dimensions, including once after TSInitialized is first called. When the arguments are zeros, the application was backgrounded.

```
void TSRenderFrame();
```
Called on the render thread as often as the renderer is idle and the display is valid, limited by vsync.

```
void TSTouchAdded(float x, float y, int pointerIndex);
```
Called on the main/UI thread directly from the host OS callback. pointerIndex can be used to track subsequent updates to a particular touch. The same pointerIndex may be reused by future touches after being released.

```
void TSTouchMoved(float x, float y, int pointerIndex);
```
A touch moved.

```
void TSTouchRemoved(float x, float y, int pointerIndex)
```
A touch ended, or a mouse button was released.

```
void TSAccelerometerChanged(float x, float y, float z);
```
The accelerometer changed values. The desktops currently provide no way to test the accelerometer.

## TODO

Android: There is commented out code in the APP_CMD_RESUME case. Android's native glue thread handling is super wonky around app backgrounding and it crashes saying the thread that created the view is the only one that can modify it. That commented out code was intended to re-hide the navigation bar after resuming, like it is when it first launches.

OSX: Standard keyboard shortcuts like command + Q need to be re-enabled somehow.

Desktop: Support all mouse buttons and mouse wheel.
