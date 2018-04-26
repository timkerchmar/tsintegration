#ifndef TSRENDERERVIEW_H
#define TSRENDERERVIEW_H

@interface TSRendererView :
#if TARGET_OS_OSX && !TARGET_OS_SIMULATOR
NSView
#else
UIView
#endif
{
}

@end

#endif
