#include <Carbon/Carbon.h>
#include <Cocoa/Cocoa.h>
#include <GameController/GCExtern.h>
#include <GameController/GameController.h>
#include <QuartzCore/CAMetalLayer.h>
#include <Foundation/Foundation.h>

#include <Window.hpp>

using namespace adh;

@interface AppDelegate : NSObject <NSApplicationDelegate> {
}
@end

@implementation AppDelegate {
}

@end

@interface View : NSView <NSWindowDelegate> {
  @public
    adh::Window* window;
    NSTrackingArea* trackingArea;
}

- (instancetype)initWithFrame:(NSRect)frameRect;

@end

@implementation View {
}

- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:(frameRect)];
    if (self) {
        self.wantsLayer = YES;
        self.layer      = [CAMetalLayer layer];
    }
    return self;
}

- (void)updateTrackingAreas {
    [super updateTrackingAreas];

    NSTrackingAreaOptions options = (NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways);
    trackingArea                  = [[NSTrackingArea alloc] initWithRect:[self bounds]
                                                options:options
                                                  owner:self
                                               userInfo:nil];
    [self addTrackingArea:trackingArea];
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)keyDown:(NSEvent*)event {
    std::uint32_t charCode{};
    NSRange range = NSMakeRange(0, [event.characters length]);
    if ([event.characters getBytes:&charCode
                         maxLength:sizeof(charCode)
                        usedLength:NULL
                          encoding:NSUTF32StringEncoding
                           options:0
                             range:range
                    remainingRange:&range]) {
        // Event::Dispatch<CharEvent>(toupper(charCode));
		Event::Dispatch<CharEvent>(charCode);

    }

    if (!event.isARepeat) {
        Event::Dispatch<KeyboardEvent>(KeyboardEvent::Type::eKeyDown, event.keyCode);
    } else {
        Event::Dispatch<KeyboardEvent>(KeyboardEvent::Type::eKeyRepeat, event.keyCode);
    }
}

- (void)keyUp:(NSEvent*)event {
    Event::Dispatch<KeyboardEvent>(KeyboardEvent::Type::eKeyUp, event.keyCode);
}

- (NSPoint)getMouseLocalPoint:(NSEvent*)event {
    NSPoint location = [event locationInWindow];
    NSPoint point    = [self convertPoint:location fromView:nil];
    point.y          = self.frame.size.height - point.y;
    return point;
}

- (void)mouseMoved:(NSEvent*)event {
    auto points = [self getMouseLocalPoint:event];
    Event::Dispatch<MouseMoveEvent>(points.x, points.y);
}

- (void)mouseDragged:(NSEvent*)event {
    auto points = [self getMouseLocalPoint:event];
    Event::Dispatch<MouseMoveEvent>(points.x, points.y);
}

- (void)mouseDown:(NSEvent*)event {
    auto points = [self getMouseLocalPoint:event];
    Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eLeftButton, MouseButtonEvent::Type::eLeftButtonDown, points.x, points.y);
}

- (void)rightMouseDown:(NSEvent*)event {
    auto points = [self getMouseLocalPoint:event];
    Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eRightButton, MouseButtonEvent::Type::eRightButtonDown, points.x, points.y);
}

- (void)otherMouseDown:(NSEvent*)event {
    auto points = [self getMouseLocalPoint:event];
    Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eMiddleButton, MouseButtonEvent::Type::eMiddleButtonDown, points.x, points.y);
}

- (void)mouseUp:(NSEvent*)event {
    auto points = [self getMouseLocalPoint:event];
    Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eLeftButton, MouseButtonEvent::Type::eLeftButtonUp, points.x, points.y);
}

- (void)rightMouseUp:(NSEvent*)event {
    auto points = [self getMouseLocalPoint:event];
    Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eRightButton, MouseButtonEvent::Type::eRightButtonUp, points.x, points.y);
}

- (void)otherMouseUp:(NSEvent*)event {
    auto points = [self getMouseLocalPoint:event];
    Event::Dispatch<MouseButtonEvent>(MouseButtonEvent::Index::eMiddleButton, MouseButtonEvent::Type::eMiddleButtonUp, points.x, points.y);
}

- (void)scrollWheel:(NSEvent*)event {
    auto points      = [self getMouseLocalPoint:event];
    float wheelDelta = [event scrollingDeltaY];
    if ([event hasPreciseScrollingDeltas]) {
        wheelDelta *= 0.1;
    }
    Event::Dispatch<MouseWheelEvent>(wheelDelta, points.x, points.y);
}

- (void)mouseExited:(NSEvent*)event {
    window->m_IsMouseInFrame = false;
}

- (void)mouseEntered:(NSEvent*)event {
    window->m_IsMouseInFrame = true;
}

- (BOOL)windowShouldClose:(id)sender {
    window->SetOpen(false);
    return YES;
}

- (void)windowDidMiniaturize:(NSNotification*)notification {
    window->m_IsMinimized = true;
}

- (void)windowDidDeminiaturize:(NSNotification*)notification {
    window->m_IsMinimized = false;
}

- (void)windowDidChangeOcclusionState:(NSNotification*)notification {
    NSWindow* nsWindow = notification.object;
    if (nsWindow.occlusionState & NSWindowOcclusionStateVisible) {
        window->m_IsInFocus = true;
    } else {
        window->m_IsInFocus = false;
        Event::Dispatch<adh::WindowEvent>(adh::WindowEvent::Type::eKillfocus);
    }
}

- (NSSize)windowWillResize:(NSWindow*)sender toSize:(NSSize)frameSize {
    window->m_IsResizing = true;
    return frameSize;
}

- (void)windowDidResize:(NSNotification*)notification {
    window->m_IsResizing = false;
    if (!window->m_IsMinimized) {
        NSWindow* nsWindow       = notification.object;
        const NSRect contentRect = [nsWindow frame];
        const NSRect fbRect      = [nsWindow convertRectToBacking:contentRect];
        window->m_Width          = fbRect.size.width;
        window->m_Height         = fbRect.size.height;
        Event::Dispatch<WindowEvent>(WindowEvent::Type::eResized);
    }
}
@end

@interface PullController  : NSObject{
}
+ (void)GetKey: (BOOL)isPressed :
                (int*) status :
                (int) id :
                (int) index;
@end

@implementation PullController {
}
+ (void)GetKey: (BOOL)isPressed :
                (int*) status :
                (int) id :
                (int) index {
        if (isPressed && *status == 0) {
            Event::Dispatch<ControllerEvent>(ControllerEvent::Type::eButtonDown, static_cast<char>(id), index);
            *status = 1;
        } else if (isPressed && *status == 1) {
            Event::Dispatch<ControllerEvent>(ControllerEvent::Type::eButtonRepeat, static_cast<char>(id), index);
        } else if (*status == 1) {
            Event::Dispatch<ControllerEvent>(ControllerEvent::Type::eButtonUp, static_cast<char>(id), index);
            *status = 0;
        }
    }
@end


namespace adh {
    // void GetKey(bool isPressed, int& status, int id, int index) {
    //     if (isPressed && status == 0) {
    //         Event::Dispatch<ControllerEvent>(ControllerEvent::Type::eButtonDown, static_cast<char>(id), index);
    //         status = 1;
    //     } else if (isPressed && status == 1) {
    //         Event::Dispatch<ControllerEvent>(ControllerEvent::Type::eButtonRepeat, static_cast<char>(id), index);
    //     } else if (status == 1) {
    //         Event::Dispatch<ControllerEvent>(ControllerEvent::Type::eButtonUp, static_cast<char>(id), index);
    //         status = 0;
    //     }
    // }

    Window::Window() noexcept : m_WindowHandle{},
                                m_Name{},
                                m_IsOpen{},
                                m_IsMinimized{},
                                m_IsResizing{},
                                m_IsInFocus{},
                                m_IsPrepared{},
                                m_IsMouseInFrame{},
                                m_Width{},
                                m_Height{} {
    }

    Window::Window(const char* name, std::int32_t width, std::int32_t height, bool isPrepared, bool setFullscreen) {
        Create(name, width, height, isPrepared, setFullscreen);
    }

    Window::~Window() {
        Clear();
    }

    void Window::Create(const char* name, std::int32_t width, std::int32_t height, bool isPrepared, bool setFullscreen) {
        [NSApplication sharedApplication];

        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp setDelegate:[AppDelegate new]];
        [NSApp activateIgnoringOtherApps:YES];

        const auto kContentRect = NSMakeRect(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
        const auto kWindowStyle = NSWindowStyleMaskTitled |
                                  NSWindowStyleMaskClosable |
                                  NSWindowStyleMaskMiniaturizable |
                                  NSWindowStyleMaskResizable;

        auto window = [[NSWindow alloc] initWithContentRect:kContentRect
                                                  styleMask:kWindowStyle
                                                    backing:NSBackingStoreBuffered
                                                      defer:NO];

        if (setFullscreen) {
            [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
            [window toggleFullScreen:window];
        }

		auto screen = [window screen];
		auto rect = [screen frame];
		float x = rect.size.width;
		float y = rect.size.height;

        [window setTitle:@(name)];
        [window setAcceptsMouseMovedEvents:YES];
        [window center];
        [window makeKeyAndOrderFront:nil];

        auto view    = [[View alloc] initWithFrame:kContentRect];
        view->window = this;
        [view updateTrackingAreas];

        [window setDelegate:view];
        [window setContentView:view];
        m_WindowHandle = (__bridge void*)view;

        m_IsOpen         = true;
        m_IsMinimized    = false;
        m_IsResizing     = false;
        m_IsPrepared     = isPrepared;
        m_IsMouseInFrame = false;
        m_Width          = width;
        m_Height         = height;
		m_ScreenWidth    = x;
		m_ScreenHeight   = y;

        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp setPresentationOptions:NSApplicationPresentationDefault];
        [NSApp activateIgnoringOtherApps:YES];
        [NSApp finishLaunching];
    }

    void Window::Destroy() noexcept {
        Clear();
    }

    void* Window::GetHandle() const noexcept {
        return m_WindowHandle;
    }

    void Window::PollEvents() noexcept {
        @autoreleasepool {
            NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                                untilDate:nil // PeekMessage
                                                   inMode:NSDefaultRunLoopMode
                                                  dequeue:YES];
            if (event) {
                [NSApp sendEvent:event];
                [NSApp updateWindows];
            }
            //untilDate:[NSDate distantFuture] // GetMessage

            // TODO: temp
            NSArray<GCController*>* x = [GCController controllers];
            for (std::size_t i{}; i != [x count] && i < 4; ++i) {
                GCExtendedGamepad* profile = x[i].extendedGamepad;
                constexpr std::uint32_t max_controllers{ 4 };

                // Buttons
                static int xStatus[max_controllers];
                [PullController   GetKey:profile.buttonX.isPressed
                                        :&xStatus[i]
                                        : 0
                                        : i];
                static int yStatus[max_controllers];
                [PullController   GetKey:profile.buttonY.isPressed
                                        :&yStatus[i]
                                        : 1
                                        : i];
                static int bStatus[max_controllers];
                [PullController   GetKey:profile.buttonB.isPressed
                                        :&bStatus[i]
                                        : 2
                                        : i];
                static int aStatus[max_controllers];
                [PullController   GetKey:profile.buttonA.isPressed
                                        :&aStatus[i]
                                        : 3
                                        : i];
                // Dpad
                static int dpadUp[max_controllers];
                [PullController   GetKey:profile.dpad.up.isPressed
                                        :&dpadUp[i]
                                        : 4
                                        : i];
                static int dpadDown[max_controllers];
                [PullController   GetKey:profile.dpad.down.isPressed
                                        :&dpadDown[i]
                                        : 5
                                        : i];
                static int dpadLeft[max_controllers];
                [PullController   GetKey:profile.dpad.left.isPressed
                                        :&dpadLeft[i]
                                        : 6
                                        : i];
                static int dpadRight[max_controllers];
                [PullController   GetKey:profile.dpad.right.isPressed
                                        :&dpadRight[i]
                                        : 7
                                        : i];

                // Shoulders
                static int rShoulder[max_controllers];
                [PullController   GetKey:profile.rightShoulder.isPressed
                                        :&rShoulder[i]
                                        : 8
                                        : i];
                static int lShoulder[max_controllers];
                [PullController   GetKey:profile.leftShoulder.isPressed
                                        :&lShoulder[i]
                                        : 9
                                        : i];
                static int lTrigger[max_controllers];
                [PullController   GetKey:profile.leftTrigger.isPressed
                                        :&lTrigger[i]
                                        : 10
                                        : i];
                static int rTrigger[max_controllers];
                [PullController   GetKey:profile.rightTrigger.isPressed
                                        :&rTrigger[i]
                                        : 11
                                        : i];

                // Other
                static int start[max_controllers];
                [PullController   GetKey:profile.buttonMenu.isPressed
                                        :&start[i]
                                        : 12
                                        : i];
                static int back[max_controllers];
                [PullController   GetKey:profile.buttonOptions.isPressed
                                        :&back[i]
                                        : 13
                                        : i];
                static int lThumb[max_controllers];
                [PullController   GetKey:profile.leftThumbstickButton.isPressed
                                        :&lThumb[i]
                                        : 14
                                        : i];
                static int rThumb[max_controllers];
                [PullController   GetKey:profile.rightThumbstickButton.isPressed
                                        :&rThumb[i]
                                        : 15
                                        : i];

                // Thumbsticks
                // static int leftThumbstickUp[max_controllers];
                // [PullController   GetKey:profile.leftThumbstick.up.isPressed
                //                         :&leftThumbstickUp[i]
                //                         : 16
                //                         : i];
                // static int leftThumbstickDown[max_controllers];
                // [PullController   GetKey:profile.leftThumbstick.down.isPressed
                //                         :&leftThumbstickDown[i]
                //                         : 17
                //                         : i];
                // static int leftThumbstickRight[max_controllers];
                // [PullController   GetKey:profile.leftThumbstick.right.isPressed
                //                         :&leftThumbstickRight[i]
                //                         : 18
                //                         : i];
                // static int leftThumbstickLeft[max_controllers];
                // [PullController   GetKey:profile.leftThumbstick.left.isPressed
                //                         :&leftThumbstickLeft[i]
                //                         : 19
                //                         : i];

                // static int rightThumbstickUp[max_controllers];
                // [PullController   GetKey:profile.rightThumbstick.up.isPressed
                //                         :&rightThumbstickUp[i]
                //                         : 24
                //                         : i];
                // static int rightThumbstickDown[max_controllers];
                // [PullController   GetKey:profile.rightThumbstick.down.isPressed
                //                         :&rightThumbstickDown[i]
                //                         : 25
                //                         : i];
                // static int rightThumbstickRight[max_controllers];
                // [PullController   GetKey:profile.rightThumbstick.right.isPressed
                //                         :&rightThumbstickRight[i]
                //                         : 26
                //                         : i];
                // static int rightThumbstickLeft[max_controllers];
                // [PullController   GetKey:profile.rightThumbstick.left.isPressed
                //                         :&rightThumbstickLeft[i]
                //                         : 27
                //                         : i];

                //std::cout << profile.leftThumbstick.xAxis.value << std::endl;
                //std::cout << profile.leftThumbstick.yAxis.value << std::endl;

                // profile.valueChangedHandler = ^(GCExtendedGamepad* gamepad, GCControllerElement* element) {
                //   if (element == gamepad.buttonX) {
                //       if (gamepad.buttonX.isPressed) {
                //           Event::Dispatch<ControllerEvent>(ControllerEvent::Type::eButtonDown, static_cast<char>(0), i);
                //           std::cout<<"x down" <<std::endl;
                //       } else {
                //           Event::Dispatch<ControllerEvent>(ControllerEvent::Type::eButtonUp, static_cast<char>(0), i);
                //           std::cout<<"x up" <<std::endl;
                //       }
                //   }
                // };
                // if (profile.buttonX.isPressed) {
                //     Event::Dispatch<ControllerEvent>(ControllerEvent::Type::eButtonRepeat, static_cast<char>(0), i);
                // }
            }
        }
    }

    bool Window::IsOpen() const noexcept {
        return m_IsOpen;
    }

    bool Window::IsMinimized() const noexcept {
        return m_IsMinimized;
    }

    bool Window::IsResizing() const noexcept {
        return m_IsResizing;
    }

    bool Window::IsInFocus() const noexcept {
        return m_IsInFocus;
    }

    bool Window::IsMouseInFrame() const noexcept {
        return m_IsMouseInFrame;
    }

    void Window::SetOpen(bool isOpen) noexcept {
        m_IsOpen = isOpen;
    }

    std::int32_t Window::GetScreenWidth() const noexcept {
        return m_ScreenWidth;
    }

    std::int32_t Window::GetScreenHeight() const noexcept {
        return m_ScreenHeight;
    }

    float Window::GetScreenAspectRatio() const noexcept {
        return static_cast<float>(m_ScreenWidth) / static_cast<float>(m_ScreenHeight);
    }

    std::int32_t Window::GetWindowWidth() const noexcept {
        return m_Width;
    }

    std::int32_t Window::GetWindowHeight() const noexcept {
        return m_Height;
    }

    void Window::Clear() noexcept {
    }
} // namespace adh
