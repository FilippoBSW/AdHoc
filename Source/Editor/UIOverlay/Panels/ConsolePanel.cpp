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

#include <iostream>

#include "ConsolePanel.hpp"
#include <ImGui/imgui.h>

namespace adh {
    ConsolePanel::ConsolePanel() {
        listener = Event::CreateListener();
        Event::AddListener<EditorLogEvent>(listener, &ConsolePanel::OnLogEvent, this);
    }

    ConsolePanel::ConsolePanel(const ConsolePanel& rhs) {
        listener = Event::CreateListener();
        Event::AddListener<EditorLogEvent>(listener, &ConsolePanel::OnLogEvent, this);
    }

    ConsolePanel& ConsolePanel::operator=(const ConsolePanel& rhs) {
        return *this;
    }

    ConsolePanel::ConsolePanel(ConsolePanel&& rhs) noexcept {
        listener = Event::CreateListener();
        Event::AddListener<EditorLogEvent>(listener, &ConsolePanel::OnLogEvent, this);
        Event::DestroyListener(rhs.listener);
    }

    ConsolePanel& ConsolePanel::operator=(ConsolePanel&& rhs) noexcept {
        Event::DestroyListener(rhs.listener);
        return *this;
    }

    ConsolePanel::~ConsolePanel() {
        Event::DestroyListener(listener);
    }

    void ConsolePanel::Draw() {
        if (isOpen) {
            if (!ImGui::Begin("Console", &isOpen)) {
                ImGui::End();
            } else {
                if (ImGui::SmallButton("Clear")) {
                    debugLog = {};
                    errorLog = {};
                }
                if (!debugLog.empty()) {
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), debugLog.data());
                }
                if (!errorLog.empty()) {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), errorLog.data());
                }
                ImGui::End();
            }
        }
    }

    void ConsolePanel::OnLogEvent(EditorLogEvent* event) {
        switch (event->type) {
        case EditorLogEvent::Type::eLog:
            {
                debugLog += event->message;
                break;
            }
        case EditorLogEvent::Type::eError:
            {
                errorLog += event->message;
                break;
            }
        }

        event->isHandled = true;
    }
} // namespace adh
