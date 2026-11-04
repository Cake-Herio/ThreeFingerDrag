#include <algorithm>
#include <cassert>
#include <iostream>
#include "../ThreeFingerDrag/gesture/event_listeners.h"

// Exercise the production listener without sending mouse input to the desktop.
static bool held = true;
static int moves = 0;
void Cursor::LeftMouseUp() { held = false; }
void Cursor::LeftMouseDown() { held = true; }
bool Cursor::IsLeftMouseDown() { return held; }
void Cursor::MoveCursor(double, double) { ++moves; }
Logger& Logger::GetInstance() { static Logger logger("unused"); return logger; }
Logger::Logger(const std::string&) {}
Logger::~Logger() = default;
void Logger::Debug(const std::string&) {}

struct Scenario {
    EventListeners::TouchActivityListener listener;
    GlobalConfig* config = GlobalConfig::GetInstance();
    std::vector<TouchContact> previous;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    Scenario() {
        held = true; moves = 0;
        config->SetLogDebug(false);
        config->SetGestureSpeed(15.0); // Keep movement-threshold scenarios independent of user defaults.
        config->SetGestureStarted(true);
        config->SetCancellationStarted(true);
        config->SetCancellationTime(start);
        config->SetCancellationDelayMs(500);
        config->SetLastEvent(start);
        config->SetLastContactCount(0);
        config->SetLastOneFingerSwitchTime(start);
    }
    void event(int ms, int count, int x = 100, int maximum = 0) {
        TouchInputData data;
        for (int i = 0; i < count; ++i)
            data.contacts.push_back(TouchContact{i, x + i * 10, 100, true});
        for (auto& contact : data.contacts) {
            contact.has_x_bounds = maximum > 0;
            contact.has_y_bounds = maximum > 0;
            contact.maximum_x = maximum;
            contact.maximum_y = maximum;
        }
        data.can_perform_gesture = count == 3;
        data.contact_count = count;
        auto time = start + std::chrono::milliseconds(ms);
        listener.OnTouchActivity(TouchActivityEventArgs(time, &data, previous));
        previous = data.contacts;
        config->SetLastEvent(time);
    }
};

int main() {
    _set_error_mode(_OUT_TO_STDERR);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    {
        Scenario s; s.event(10,1); s.event(20,1,110);
        assert(!held && !s.config->IsGestureStarted() && moves == 0);
    }
    {
        Scenario s; s.event(10,1); s.event(20,2); s.event(30,2,110);
        assert(!held && moves == 0);
    }
    {
        Scenario s; s.event(10,1); s.event(20,2); s.event(30,3);
        assert(held && s.config->IsCancellationStarted() && moves == 0);
        s.event(60,3); s.event(90,3); s.event(100,3,120);
        assert(held && !s.config->IsCancellationStarted() && moves == 1);
    }
    {
        Scenario s; s.event(10,3); s.event(40,1); s.event(50,1,110);
        assert(!held && moves == 0);
    }
    {
        Scenario s; s.event(10,1); s.event(510,1);
        assert(!held && !s.config->IsCancellationStarted());
    }
    {
        Scenario s; s.event(10,3); s.event(40,3); s.event(80,3); s.event(510,3);
        assert(!held && moves == 0);
    }
    {
        Scenario s; s.event(510,3);
        assert(!held && moves == 0);
    }
    {
        Scenario s; s.event(10,3); s.event(40,3); s.previous.clear();
        s.event(80,3); s.event(100,3,120);
        assert(held && s.config->IsCancellationStarted() && moves == 0);
    }
    {
        Scenario s; s.event(10,1); s.event(20,2); s.event(30,3);
        s.event(60,3); s.event(90,3); s.event(100,3,101);
        assert(s.config->IsCancellationStarted() && moves == 0);
        s.event(110,3,120);
        assert(held && !s.config->IsCancellationStarted() && moves == 1);
    }
    {
        Scenario s; s.event(10,1,100,10000); s.event(30,1,102,10000);
        s.event(60,2,104,10000);
        assert(held && s.config->IsCancellationStarted() && moves == 0);
        // Movement reported with the third landing must not cancel re-contact.
        s.event(80,3,180,10000); s.event(110,3,180,10000);
        s.event(140,3,180,10000); s.event(150,3,220,10000);
        assert(held && !s.config->IsCancellationStarted() && moves == 1);
    }
    {
        Scenario s; s.event(10,1,100,10000); s.event(20,1,110,10000);
        s.event(30,1,120,10000); s.event(40,1,140,10000);
        assert(held && s.config->IsCancellationStarted() && moves == 0);
        s.event(50,1,151,10000);
        assert(!held && moves == 0);
    }
    std::cout << "PASS: 11 recontact scenarios\n";
}
