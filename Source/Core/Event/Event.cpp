#include "Event.hpp"

namespace adh {
    Event& Event::GetInstance() noexcept {
        static Event event;
        return event;
    }
} // namespace adh
