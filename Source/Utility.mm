// *********************************************************************************
// MIT License
//
// Copyright (c) 2021 Filippo-BSW
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// *********************************************************************************

#include "Utility.hpp"

#if defined(ADH_DEBUG)
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
