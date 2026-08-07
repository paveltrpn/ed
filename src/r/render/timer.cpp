
#include <chrono>
#include <limits>

#include "timer.h"

namespace tire {

Timer::Timer() {
    //
    lastTimePoint_ = timer_.now();
}

#define TO_SECOND 0.000001

/*
     * @return - frame time in microseconds
    **/
long long Timer::frameDuration() const {
    return frameDuration_;
}

float Timer::floatFrameDuration() const {
    return static_cast<float>( frameDuration_ ) * TO_SECOND;
};

double Timer::doubleFrameDuration() const {
    return static_cast<double>( frameDuration_ ) * TO_SECOND;
};

long long Timer::averageFrameDuration() const {
    return frameDurationAccumulator_ / frameCountAccumulator_;
}

float Timer::framesPerSecond() const {
    return static_cast<float>( frameCountAccumulator_ ) /
           ( static_cast<float>( frameDurationAccumulator_ ) * TO_SECOND );
}

std::array<long long, 2> Timer::minMaxFrameDuration() const {
    return { minFrameDuration_, maxFrameDuration_ };
}

void Timer::resetAccumulators() {
    //
    frameDurationAccumulator_ = frameCountAccumulator_ = 0;

    //
    minFrameDuration_ = std::numeric_limits<long long>::max();
    maxFrameDuration_ = 0;
}

// Called every frame
void Timer::update() {
    // Frame time calculation.
    // Time elapsed between two calls of this function.
    const auto newTimePoint_ = timer_.now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>( newTimePoint_ - lastTimePoint_ );
    frameDuration_ = duration.count();
    lastTimePoint_ = newTimePoint_;

    // Get minimal and maximal frametime
    if ( ( frameDuration_ - minFrameDuration_ ) < 0 ) {
        minFrameDuration_ = frameDuration_;
    } else if ( ( frameDuration_ - maxFrameDuration_ ) > 0 ) {
        maxFrameDuration_ = frameDuration_;
    }

    // Update accumulators.
    ++frameCountAccumulator_;
    // Will it overflow?
    frameDurationAccumulator_ += frameDuration_;
}

}  // namespace tire