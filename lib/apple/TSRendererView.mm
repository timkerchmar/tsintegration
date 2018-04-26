#include "TSIntegration.h"

#if TARGET_OS_OSX && !TARGET_OS_SIMULATOR
#import <AppKit/AppKit.h>

extern int clientViewWidth;
extern int clientViewHeight;
int touchIndex = 0;
#endif

#import "TSRendererView.h"

@implementation TSRendererView

#if TARGET_OS_OSX && !TARGET_OS_SIMULATOR

- (void)mouseDown:(NSEvent *)event
{
    NSPoint eventLocation = [self convertPoint:[event locationInWindow] fromView:nil];
    TSTouchAdded(eventLocation.x, clientViewHeight - eventLocation.y, ++touchIndex);
}

- (void)mouseUp:(NSEvent *)event
{
    NSPoint eventLocation = [self convertPoint:[event locationInWindow] fromView:nil];
    TSTouchRemoved(eventLocation.x, clientViewHeight - eventLocation.y, touchIndex);
}

- (void)mouseDragged:(NSEvent *)event
{
    NSPoint eventLocation = [self convertPoint:[event locationInWindow] fromView:nil];
    TSTouchMoved(eventLocation.x, clientViewHeight - eventLocation.y, touchIndex);
}

- (void)mouseExited:(NSEvent *)event
{
    NSPoint eventLocation = [self convertPoint:[event locationInWindow] fromView:nil];
    TSTouchRemoved(eventLocation.x, clientViewHeight - eventLocation.y, touchIndex);
}

#else

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

//=========== TOUCH EVENTS
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	NSEnumerator *e = [touches objectEnumerator];
    while (UITouch* touch = [e nextObject])
    {
        CGPoint touchLocation = [touch locationInView:self];
        touchLocation.x *= [self contentScaleFactor];
        touchLocation.y *= [self contentScaleFactor];
        TSTouchAdded(touchLocation.x, touchLocation.y, (unsigned long long)(void *)touch);
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	NSEnumerator *e = [touches objectEnumerator];
    while (UITouch* touch = [e nextObject])
    {
        CGPoint touchLocation = [touch locationInView:self];
        touchLocation.x *= [self contentScaleFactor];
        touchLocation.y *= [self contentScaleFactor];
        TSTouchMoved(touchLocation.x, touchLocation.y, (unsigned long long)(void *)touch);
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	NSEnumerator *e = [touches objectEnumerator];
    while (UITouch* touch = [e nextObject])
    {
        CGPoint touchLocation = [touch locationInView:self];
        touchLocation.x *= [self contentScaleFactor];
        touchLocation.y *= [self contentScaleFactor];
        TSTouchRemoved(touchLocation.x, touchLocation.y, (unsigned long long)(void *)touch);
    }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    NSEnumerator *e = [touches objectEnumerator];
    while (UITouch* touch = [e nextObject])
    {
        CGPoint touchLocation = [touch locationInView:self];
        touchLocation.x *= [self contentScaleFactor];
        touchLocation.y *= [self contentScaleFactor];
        TSTouchRemoved(touchLocation.x, touchLocation.y, (unsigned long long)(void *)touch);
    }
}
#endif
@end
