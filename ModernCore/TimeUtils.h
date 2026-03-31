#pragma once

#include <chrono>
#include <ratio>

namespace GreedSnake {

/**
 * @brief 时间管理工具 - 使用 std::chrono 替代 GetTickCount
 * 
 * 更精确、跨平台、类型安全
 */

// 类型别名
using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;
using Duration = Clock::duration;
using Seconds = std::chrono::duration<float>;
using Milliseconds = std::chrono::milliseconds;
using Microseconds = std::chrono::microseconds;

/**
 * @brief 获取当前时间点
 */
[[nodiscard]] inline TimePoint Now() noexcept {
    return Clock::now();
}

/**
 * @brief 将持续时间转换为秒
 */
[[nodiscard]] inline float ToSeconds(Duration duration) noexcept {
    return std::chrono::duration<float>(duration).count();
}

/**
 * @brief 将持续时间转换为毫秒
 */
[[nodiscard]] inline float ToMilliseconds(Duration duration) noexcept {
    return std::chrono::duration<float, std::milli>(duration).count();
}

/**
 * @brief 计算两个时间点之间的差值（秒）
 */
[[nodiscard]] inline float DeltaSeconds(TimePoint start, TimePoint end) noexcept {
    return ToSeconds(end - start);
}

/**
 * @brief 计算从某个时间点到现在经过的秒数
 */
[[nodiscard]] inline float ElapsedSeconds(TimePoint start) noexcept {
    return DeltaSeconds(start, Now());
}

/**
 * @brief 时间间隔计时器
 */
class Timer {
public:
    Timer() noexcept = default;
    
    // 开始计时
    void Start() noexcept {
        startTime_ = Now();
        isRunning_ = true;
    }
    
    // 停止计时
    void Stop() noexcept {
        if (isRunning_) {
            elapsed_ += Now() - startTime_;
            isRunning_ = false;
        }
    }
    
    // 重置计时器
    void Reset() noexcept {
        elapsed_ = Duration::zero();
        isRunning_ = false;
    }
    
    // 重新开始
    void Restart() noexcept {
        Reset();
        Start();
    }
    
    // 获取经过的时间（秒）
    [[nodiscard]] float Elapsed() const noexcept {
        if (isRunning_) {
            return ToSeconds(elapsed_ + (Now() - startTime_));
        }
        return ToSeconds(elapsed_);
    }
    
    // 是否正在运行
    [[nodiscard]] bool IsRunning() const noexcept {
        return isRunning_;
    }
    
private:
    TimePoint startTime_;
    Duration elapsed_ = Duration::zero();
    bool isRunning_ = false;
};

/**
 * @brief 帧率计算器
 */
class FrameRateCalculator {
public:
    [[nodiscard]] float Update() noexcept {
        auto now = Now();
        if (lastTime_.time_since_epoch().count() != 0) {
            deltaTime_ = DeltaSeconds(lastTime_, now);
        }
        lastTime_ = now;
        return deltaTime_;
    }
    
    [[nodiscard]] float GetDeltaTime() const noexcept {
        return deltaTime_;
    }
    
    [[nodiscard]] float GetFPS() const noexcept {
        if (deltaTime_ > 0.0f) {
            return 1.0f / deltaTime_;
        }
        return 0.0f;
    }
    
private:
    TimePoint lastTime_;
    float deltaTime_ = 1.0f / 60.0f; // 默认60fps
};

} // namespace GreedSnake
