#pragma once

#include "../Gameplay/GameConfig.h"
#include <mutex>

class GameProgressManager {
private:
    int foodEatenCount;
    int finalScore;
    float gameStartTime;
    mutable std::mutex progressMutex;

public:
    static GameProgressManager& Instance();
    
    // Progress tracking
    int GetFoodEatenCount() const;
    void AddFoodEaten();
    void SetFoodEatenCount(int count);
    
    int GetFinalScore() const;
    void SetFinalScore(int score);
    
    float GetGameStartTime() const;
    void SetGameStartTime(float time);
    
    // Initialize progress
    void Initialize();
    
private:
    GameProgressManager() = default;
    ~GameProgressManager() = default;
    GameProgressManager(const GameProgressManager&) = delete;
    GameProgressManager& operator=(const GameProgressManager&) = delete;
};