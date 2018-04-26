
#include "TSIntegration.h"
#include "TSLogger.h"
#include "TSThread.h"

#include <string>

GLint internalBackBufferWidth = 0;
GLint internalBackBufferHeight = -1;

TSConditionVariable renderCommandReady;
TSConditionVariable rendererPaused;

enum TSRendererCommand
{
    TSRENDERER_IDLE,
    TSRENDERER_RECONFIGURE,
    TSRENDERER_PAUSE
};

class TSRenderer : public TSThread {
public:
    TSRendererCommand command;
    TSMutex mutex;

    std::string name()
    {
        return "rendering thread";
    }

    void run()
    {
        mutex.lock();

        TSIntegration::setPixelFormat();
        TSIntegration::createContext();
        TSIntegration::makeContextCurrent();

        TSInitialized();

        printf("initialized\n");

        for (;;) {
            switch (command) {
                case TSRENDERER_RECONFIGURE:
                    printf("TSRENDERER_RECONFIGURE: began\n");

                    if (!internalBackBufferWidth || !internalBackBufferHeight) {
                        TSIntegration::enableDisplay();
                        TSIntegration::setSwapInterval();
                    }

                    TSIntegration::displaySize(internalBackBufferWidth,
                                               internalBackBufferHeight);

                    TSDisplayChanged(internalBackBufferWidth, internalBackBufferHeight);

                    printf("TSRENDERER_RECONFIGURE: ended with display change %i %i\n", internalBackBufferWidth, internalBackBufferHeight);

                    command = TSRENDERER_IDLE;
                    break;
                case TSRENDERER_IDLE:
                    if (internalBackBufferWidth && internalBackBufferHeight)
                    {
                        TSRenderFrame();

                        mutex.unlock();
                        TSIntegration::pageFlip();
                        mutex.lock();
                    } else {
                        renderCommandReady.wait(mutex);
                    }

                    break;
                case TSRENDERER_PAUSE:
                    internalBackBufferWidth = internalBackBufferHeight = 0;
                    TSDisplayChanged(internalBackBufferWidth, internalBackBufferHeight);
                    TSIntegration::disableDisplay();
                    rendererPaused.notify();
                    command = TSRENDERER_IDLE;
                    break;
            }
        }
    }
};

TSRenderer renderer;

void TSIntegration::initialize()
{
    TSIntegration::createWindow();

    if (internalBackBufferHeight < 0)
    {
        internalBackBufferHeight = 0;
        TSRedirectStandardOutput();
        renderer.command = TSRENDERER_IDLE;
        renderer.start();
    }
}

void TSIntegration::start()
{
    renderer.mutex.lock();
    renderer.command = TSRENDERER_RECONFIGURE;
    renderCommandReady.notify();
    renderer.mutex.unlock();
}

void TSIntegration::stop()
{
    renderer.mutex.lock();
    renderer.command = TSRENDERER_PAUSE;
    renderCommandReady.notify();
    rendererPaused.wait(renderer.mutex);
    renderer.mutex.unlock();
}
