
#import "AppDelegate.h"
#include "TSIntegration.h"
#include "TSThread.h"
#import "TSRendererView.h"

NSOpenGLContext* context = nil;

NSWindow *window = nil;

TSMutex clientViewDimensionsLock;
int clientViewWidth = 960, clientViewHeight = 720;

void TSIntegration::createWindow()
{
    NSRect frame = NSMakeRect(0, 0, clientViewWidth, clientViewHeight);
    window  = [[NSWindow alloc] initWithContentRect:frame
                                          styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                                                       backing:NSBackingStoreBuffered
                                                         defer:NO];
    window.contentView = [[TSRendererView alloc] init];
    [window makeKeyAndOrderFront:NSApp];
    window.acceptsMouseMovedEvents = YES;
    window.backgroundColor = [NSColor blackColor];
}

void TSIntegration::setPixelFormat()
{
}

void TSIntegration::createContext()
{
    NSOpenGLPixelFormatAttribute attribs[] = {
        NSOpenGLPFAColorSize, 32,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFADoubleBuffer,
        (NSOpenGLPixelFormatAttribute)0
    };
    NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
    context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
    GLint swapInt = 1;
    [context setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    GLint opaque = 1;
    [context setValues:&opaque forParameter:NSOpenGLCPSurfaceOpacity];
}

void TSIntegration::makeContextCurrent()
{
    [context setView:window.contentView];
    [context makeCurrentContext];
}

void TSIntegration::clearContext()
{
    [NSOpenGLContext clearCurrentContext];
}

void TSIntegration::enableDisplay()
{
    [context update];
}

void TSIntegration::disableDisplay()
{
}

void TSIntegration::displaySize(GLint& width, GLint& height)
{
    clientViewDimensionsLock.lock();
    width = clientViewWidth;
    height = clientViewHeight;
    clientViewDimensionsLock.unlock();
}

void TSIntegration::setSwapInterval()
{
}

void TSIntegration::pageFlip()
{
    [context flushBuffer];
}

//=====================================================================================
// PLATFORM EVENT SCAFFOLDING
//=====================================================================================

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    TSIntegration::initialize();
    [window setDelegate:self];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}

- (void)windowWillClose:(NSNotification *)notification
{
    TSIntegration::stop();
}

- (void)windowDidResize:(NSNotification *)notification
{
    clientViewDimensionsLock.lock();
    clientViewWidth  = window.contentView.bounds.size.width;
    clientViewHeight = window.contentView.bounds.size.height;
    clientViewDimensionsLock.unlock();

    [context update];
    TSIntegration::start();
}

- (void)windowDidChangeOcclusionState:(NSNotification *)notification
{
    if (window.occlusionState & NSWindowOcclusionStateVisible)
    {
        TSIntegration::start();
    }
    else
    {
        TSIntegration::stop();
    }
}

- (void)windowDidChangeScreenProfile:(NSNotification *)notification
{
    TSIntegration::start();
}

- (void)windowDidChangeBackingProperties:(NSNotification *)notification
{
    TSIntegration::start();
}

@end
