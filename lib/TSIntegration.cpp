
#include "TSIntegration.h"
#include "TSLogger.h"
#include "TSThread.h"

#include <string>

GLint internalBackBufferWidth = 0;
GLint internalBackBufferHeight = 0;

TSConditionVariable renderCommandReady;
TSConditionVariable rendererPaused;
TSMutex mutex;

enum TSRendererCommand
{
    TSRENDERER_IDLE,
    TSRENDERER_RECONFIGURE,
    TSRENDERER_PAUSE
};

class TSRenderer : public TSThread {
public:
    TSRendererCommand command;
    bool destroyRequested;

    std::string name()
    {
        return "rendering thread";
    }

    void run()
    {
        destroyRequested = false;
        mutex.lock();

        TSIntegration::setPixelFormat();
        TSIntegration::createContext();
        TSIntegration::makeContextCurrent();

        static bool firstTime = true;
        if (firstTime)
        {
            firstTime = false;
            TSInitialized();
        }

        printf("initialized\n");

        while (!destroyRequested) 
        {
            switch (command) 
            {
                case TSRENDERER_RECONFIGURE:
                    printf("TSRENDERER_RECONFIGURE: began\n");

                    if (!internalBackBufferWidth || !internalBackBufferHeight) 
                    {
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
                    } 
                    else 
                    {
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
        
        mutex.unlock();

        delete this;
    }
};

TSRenderer* renderer = NULL;

void TSIntegration::initialize()
{
    TSIntegration::createWindow();
    TSRedirectStandardOutput();
    renderer = new TSRenderer();
    renderer->command = TSRENDERER_IDLE;
    renderer->start();
}

void TSIntegration::start()
{
    mutex.lock();
    renderer->command = TSRENDERER_RECONFIGURE;
    renderCommandReady.notify();
    mutex.unlock();
}

void TSIntegration::stop()
{
    if (renderer)
    {
        mutex.lock();
        renderer->command = TSRENDERER_PAUSE;
        renderCommandReady.notify();
        rendererPaused.wait(mutex);
        mutex.unlock();
    }
}

void TSIntegration::shutDown()
{
    stop();

    mutex.lock();
    renderer->destroyRequested = true;
    renderCommandReady.notify();
    mutex.unlock();
    renderer = NULL;

}
