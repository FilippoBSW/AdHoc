#include <Carbon/Carbon.h>
#include <Cocoa/Cocoa.h>
#include <GameController/GCExtern.h>
#include <GameController/GameController.h>
#include <QuartzCore/CAMetalLayer.h>
#include <Foundation/Foundation.h>

#include <Window.hpp>

#if defined(ADH_APPLE)
namespace adh {
    void MessageBox(const char* message, const char* str1, const char* str2) {
        @autoreleasepool {
            NSString* s = [[NSString alloc] initWithUTF8String:str1];
            NSString* s2 = [[NSString alloc] initWithUTF8String:str2];

            NSAlert* alert = [[NSAlert alloc] init];
            [alert addButtonWithTitle:@"OK"];
            [alert addButtonWithTitle:@"Ignore"];
            [alert setMessageText:s];
            [alert setInformativeText:s2];
            [alert setAlertStyle:NSAlertStyleCritical];

            alert.accessoryView = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 500, 0)];

            if ([alert runModal] == NSAlertFirstButtonReturn) {
                throw std::runtime_error(message);
            }
        }
    }
} // namespace adh
#endif
#endif
