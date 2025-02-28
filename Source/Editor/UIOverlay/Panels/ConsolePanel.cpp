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
                debugLog.insert(0, event->message);
                break;
            }
        case EditorLogEvent::Type::eError:
            {
                errorLog.insert(0, event->message);
                break;
            }
        }

        event->isHandled = true;
    }
} // namespace adh
