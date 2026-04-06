#pragma once
#include <random>
#include <mutex>
#include <atomic>

/**
 * 安全随机数生成器类
 * 使用现代C++11 <random> 库替代不安全的 rand()
 * 线程安全设计
 */
class SecureRandom {
public:
    // 获取单例实例
    static SecureRandom& Instance() {
        static SecureRandom instance;
        return instance;
    }

    // 生成指定范围的随机整数 [min, max]
    int NextInt(int min, int max) {
        if (min >= max) return min;
        std::uniform_int_distribution<int> dist(min, max - 1);
        std::lock_guard<std::mutex> lock(mutex_);
        return dist(engine_);
    }

    // 生成指定范围的随机整数 [0, max)
    int NextInt(int max) {
        if (max <= 0) return 0;
        return NextInt(0, max);
    }

    // 生成指定范围的随机浮点数 [min, max]
    float NextFloat(float min, float max) {
        if (min >= max) return min;
        std::uniform_real_distribution<float> dist(min, max);
        std::lock_guard<std::mutex> lock(mutex_);
        return dist(engine_);
    }

    // 生成 [0.0, 1.0) 范围的随机浮点数
    float NextFloat() {
        return NextFloat(0.0f, 1.0f);
    }

    // 生成指定范围的随机双精度浮点数 [min, max]
    double NextDouble(double min, double max) {
        if (min >= max) return min;
        std::uniform_real_distribution<double> dist(min, max);
        std::lock_guard<std::mutex> lock(mutex_);
        return dist(engine_);
    }

    // 生成随机布尔值
    bool NextBool() {
        std::lock_guard<std::mutex> lock(mutex_);
        return boolDist_(engine_);
    }

    // 生成指定百分比概率的布尔值
    bool NextBool(float probability) {
        if (probability <= 0.0f) return false;
        if (probability >= 1.0f) return true;
        return NextFloat() < probability;
    }

private:
    SecureRandom() 
        : engine_(std::random_device{}())
        , boolDist_(0, 1) {}
    
    ~SecureRandom() = default;
    SecureRandom(const SecureRandom&) = delete;
    SecureRandom& operator=(const SecureRandom&) = delete;

    std::mt19937 engine_;  // Mersenne Twister 引擎
    std::mutex mutex_;
    std::uniform_int_distribution<int> boolDist_;
};

// 便捷的宏定义
#define RANDOM_INT(min, max) SecureRandom::Instance().NextInt(min, max)
#define RANDOM_INT_MAX(max) SecureRandom::Instance().NextInt(max)
#define RANDOM_FLOAT(min, max) SecureRandom::Instance().NextFloat(min, max)
#define RANDOM_FLOAT_01() SecureRandom::Instance().NextFloat()
#define RANDOM_BOOL() SecureRandom::Instance().NextBool()
#define RANDOM_BOOL_PROB(prob) SecureRandom::Instance().NextBool(prob)
