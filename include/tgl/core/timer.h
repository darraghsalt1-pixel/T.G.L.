#ifndef TGL_CORE_TIMER_H
#define TGL_CORE_TIMER_H

#include <chrono>

namespace tgl {

class Timer {
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;

    Timer() : start_(Clock::now()), last_(start_) {}

    // Returns time since last reset in seconds
    float elapsed() const {
        auto now = Clock::now();
        return std::chrono::duration<float>(now - start_).count();
    }

    // Returns delta time since last call to delta() in seconds
    float delta() {
        auto now = Clock::now();
        float dt = std::chrono::duration<float>(now - last_).count();
        last_ = now;
        return dt;
    }

    void reset() {
        start_ = Clock::now();
        last_ = start_;
    }

private:
    TimePoint start_;
    TimePoint last_;
};

// Fixed timestep accumulator for physics
class FixedTimestep {
public:
    explicit FixedTimestep(float step = 1.0f / 60.0f) : step_(step) {}

    // Feed frame delta, returns number of fixed steps to execute
    int update(float dt) {
        accumulator_ += dt;
        int steps = 0;
        while (accumulator_ >= step_) {
            accumulator_ -= step_;
            steps++;
        }
        return steps;
    }

    float step() const { return step_; }
    float alpha() const { return accumulator_ / step_; } // interpolation factor

private:
    float step_;
    float accumulator_ = 0.0f;
};

} // namespace tgl

#endif // TGL_CORE_TIMER_H
