#include "TSIntegration.h"

void TSInitialized()
{
}

int backBufferWidth, backBufferHeight;

void TSDisplayChanged(int newWidth, int newHeight)
{
    backBufferWidth = newWidth;
    backBufferHeight = newHeight;
    if (backBufferWidth && backBufferHeight)
    {
        glViewport(0, 0, backBufferWidth, backBufferHeight);
    }
}

float red = 1.0, green = 0.0, blue = 0.0;

void TSRenderFrame()
{
    glClearColor(red, green, blue, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void TSTouchAdded(float x, float y, int pointerIndex)
{
    red = x / backBufferWidth;
    green = y / backBufferHeight;
}

void TSTouchRemoved(float x, float y, int pointerIndex)
{

}

void TSTouchMoved(float x, float y, int pointerIndex)
{
    red = x / backBufferWidth;
    green = y / backBufferHeight;
}

void TSAccelerometerChanged(float x, float y, float z)
{
    blue = (x * x + y * y) / (z * z);
}
