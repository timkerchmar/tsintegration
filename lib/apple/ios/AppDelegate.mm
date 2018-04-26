#import "TSRendererView.h"
#import "AppDelegate.h"
#include "TSIntegration.h"
#include "TSThread.h"

GLuint defaultFramebuffer = 0, colorRenderbuffer = 0;
EAGLContext *context = nil;
UIWindow *window = nil;
CAEAGLLayer *eaglLayer = nil;

@interface TSViewController : UIViewController
@end

@implementation TSViewController

- (BOOL)prefersHomeIndicatorAutoHidden
{
    return YES;
}

- (UIRectEdge)preferredScreenEdgesDeferringSystemGestures
{
    return UIRectEdgeAll;
}

@end

void TSIntegration::createWindow()
{
    UIViewController *viewController = [[TSViewController alloc] init];
    viewController.view = [[TSRendererView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

    window = [[UIWindow alloc] init];
    window.rootViewController = viewController;
    [window makeKeyAndVisible];
	
    float scale = [[UIScreen mainScreen] scale];
    viewController.view.layer.contentsScale = scale;
    viewController.view.userInteractionEnabled = YES;
    viewController.view.multipleTouchEnabled = YES;
    viewController.view.opaque = YES;
    viewController.view.contentScaleFactor = scale;
    
    TSRendererView *view = (TSRendererView *)window.rootViewController.view;
    eaglLayer = (CAEAGLLayer *)view.layer;
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithBool:false],
                                    kEAGLDrawablePropertyRetainedBacking,
                                    kEAGLColorFormatRGBA8,
                                    kEAGLDrawablePropertyColorFormat,
                                    nil];
}

void TSIntegration::setPixelFormat()
{
}

void TSIntegration::createContext()
{
	context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    if (!context)
    {
        printf("Failed to create OpenGL ES 3.0 context\n");
    }
    else printf("Successfully created OpenGL ES 3.0 context\n");
}

void TSIntegration::makeContextCurrent()
{
    if (![EAGLContext setCurrentContext:context]) {
        printf("Failed to activate OpenGL ES 3.0 context\n");
    }
    else printf("Successfully activated OpenGL ES 3.0 context\n");
}

void TSIntegration::clearContext()
{
    [EAGLContext setCurrentContext:nil];
}

void TSIntegration::enableDisplay()
{
    // Create default framebuffer object.
    glGenFramebuffers(1, &defaultFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    // Create color render buffer and allocate backing store.
    glGenRenderbuffers(1, &colorRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    if (![context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer]) {
        NSLog(@"Failed to renderbufferStorage:fromDrawable:%@", eaglLayer);
    }
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);

    glFlush();
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
    
    // important to have last render buffer bind be the color buffer in order to avoid white screen.
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
}

void TSIntegration::disableDisplay()
{
    glDeleteFramebuffers(1, &defaultFramebuffer);
    defaultFramebuffer = 0;
    
    glDeleteRenderbuffers(1, &colorRenderbuffer);
    colorRenderbuffer = 0;
}

void TSIntegration::displaySize(GLint& width, GLint& height)
{
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
}

void TSIntegration::setSwapInterval()
{
}

void TSIntegration::pageFlip()
{
    // some odd code recommended to me by instruments opengl driver profiler
    const GLenum discards[] = {GL_COLOR_ATTACHMENT0};
    glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, discards);
    [context presentRenderbuffer:GL_RENDERBUFFER];
}

//=====================================================================================
// PLATFORM EVENT SCAFFOLDING
//=====================================================================================

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(UIApplication *)application {
    TSIntegration::initialize();
    [[UIAccelerometer sharedAccelerometer] setDelegate:self];
    [[UIAccelerometer sharedAccelerometer] setUpdateInterval:1.0/30.0];
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    printf("applicationDidBecomeActive\n");
    TSIntegration::start();
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    TSIntegration::stop();
}

- (void)application:(UIApplication *)application willChangeStatusBarOrientation:(UIInterfaceOrientation)newStatusBarOrientation duration:(NSTimeInterval)duration
{
    printf("willChangeStatusBarOrientation\n");
    TSIntegration::stop();
}

- (void)application:(UIApplication *)application didChangeStatusBarOrientation:(UIInterfaceOrientation) oldStatusBarOrientation
{
    TSIntegration::start();
}

- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration {
    TSAccelerometerChanged(acceleration.x, acceleration.y, acceleration.z);
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    printf("applicationDidReceiveMemoryWarning\n");
}

@end
