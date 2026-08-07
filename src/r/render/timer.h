#pragma once

#include <chrono>
#include <limits>

namespace tire {

struct Timer final {
    Timer();

    Timer( const Timer& other ) = delete;
    Timer( Timer&& other ) = delete;

    Timer& operator=( const Timer& other ) = delete;
    Timer& operator=( Timer&& other ) = delete;

    ~Timer() = default;

#define TO_SECOND 0.000001

    /*
     * @return - frame time in microseconds
    **/
    [[nodiscard]]
    long long frameDuration() const ;
    [[nodiscard]]
    float floatFrameDuration() const ;

    [[nodiscard]]
    double doubleFrameDuration() const ;

    template <typename T>
        requires std::is_same_v<float, T> || std::is_same_v<double, T>
    [[nodiscard]]
    T frameDuration() {
        return static_cast<T>( frameDuration_ ) * static_cast<T>( TO_SECOND );
    }

    [[nodiscard]]
    long long averageFrameDuration() const;

    [[nodiscard]]
    float framesPerSecond() const ;

    [[nodiscard]]
    std::array<long long, 2> minMaxFrameDuration() const ;

    void resetAccumulators() ;

    // Called every frame
    void update();

private:
    std::chrono::steady_clock timer_;
    std::chrono::time_point<decltype( timer_ )> lastTimePoint_{};
    // In microseconds - 1/1000000s
    long long frameDuration_{};

    long long frameCountAccumulator_{};
    long long frameDurationAccumulator_{};
    long long minFrameDuration_{ std::numeric_limits<long long>::max() };
    long long maxFrameDuration_{};
};

}  // namespace tire