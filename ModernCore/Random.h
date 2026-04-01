#pragma once

#include <random>
#include <chrono>

namespace GreedSnake {

/**
 * @brief 现代C++随机数生成器（替代rand()）
 * 
 * 使用C++11 <random> 库，提供高质量随机数生成
 * 线程安全（每个线程使用独立引擎）
 */
class Random {
public:
    // 获取全局随机数生成器实例
    static Random& GetInstance() {
        thread_local static Random instance;
        return instance;
    }
    
    // 生成 [min, max] 范围内的整数
    static int Int(int min, int max) {
        return GetInstance().GenerateInt(min, max);
    }
    
    // 生成 [0, max) 范围内的整数
    static int Int(int max) {
        return GetInstance().GenerateInt(0, max - 1);
    }
    
    // 生成 [min, max] 范围内的浮点数
    static float Float(float min, float max) {
        return GetInstance().GenerateFloat(min, max);
    }
    
    // 生成 [0.0, 1.0] 范围内的浮点数
    static float Float() {
        return GetInstance().GenerateFloat(0.0f, 1.0f);
    }
    
    // 生成布尔值（50%概率）
    static bool Bool() {
        return GetInstance().GenerateBool();
    }
    
    // 生成布尔值（指定概率）
    static bool Bool(float probability) {
        return GetInstance().GenerateBool(probability);
    }

private:
    std::mt19937 engine;  // Mersenne Twister引擎
    
    Random() {
        // 使用高分辨率时钟种子
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        engine.seed(static_cast<unsigned int>(seed));
    }
    
    int GenerateInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(engine);
    }
    
    float GenerateFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(engine);
    }
    
    bool GenerateBool() {
        std::uniform_int_distribution<int> dist(0, 1);
        return dist(engine) == 1;
    }
    
    bool GenerateBool(float probability) {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(engine) < probability;
    }
};

} // namespace GreedSnake
