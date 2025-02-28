#pragma once
#include <string>

#include <Std/Array.hpp>

#include <Event/Event.hpp>

namespace adh {
    class ConsolePanel {
      public:
        ConsolePanel();

        ConsolePanel(const ConsolePanel& rhs);

        ConsolePanel& operator=(const ConsolePanel& rhs);

        ConsolePanel(ConsolePanel&& rhs) noexcept;

        ConsolePanel& operator=(ConsolePanel&& rhs) noexcept;

        ~ConsolePanel();

        void Draw();

        void OnLogEvent(EditorLogEvent* event);

      public:
        bool isOpen{ true };

      private:
        std::string errorLog;
        std::string debugLog;

        EventListener listener{ null_listener };
    };
} // namespace adh
