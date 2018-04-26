//
//  main.m
//  PainterOSX
//
//  Created by Tim Kerchmar on 4/15/18.
//

#import "AppDelegate.h"

int main(int argc, const char * argv[]) {
    [NSApplication sharedApplication];
    
    AppDelegate *appDelegate = [[AppDelegate alloc] init];
    [NSApp setDelegate:appDelegate];
    [NSApp run];

    return 0;
}
