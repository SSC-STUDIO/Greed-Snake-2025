#pragma once

#include "../Gameplay/GameConfig.h"
#include <mutex>

enum class GameDifficulty {
    Easy,   // Easy
    Normal,  // Normal
    Hard     // Hard
};

class DifficultyManager {
private:
    GameDifficulty currentDifficulty;
    int difficulty;
    float aiAggression;
    float foodSpawnRate;
    int aiSnakeCount;
    float lavaWarningTime;
    mutable std::mutex difficultyMutex;

public:
    static DifficultyManager& Instance();
    
    // Difficulty settings
    GameDifficulty GetCurrentDifficulty() const;
    void SetDifficulty(GameDifficulty diff);
    
    int GetDifficultyLevel() const;
    void SetDifficultyLevel(int level);
    
    // Game parameters based on difficulty
    float GetAIAggression() const;
    void SetAIAggression(float aggression);
    
    float GetFoodSpawnRate() const;
    void SetFoodSpawnRate(float rate);
    
    int GetAISnakeCount() const;
    void SetAISnakeCount(int count);
    
    float GetLavaWarningTime() const;
    void SetLavaWarningTime(float time);
    
    // Initialize difficulty settings
    void Initialize();
    
private:
    void ApplyDifficultySettings();
    
    DifficultyManager() = default;
    ~DifficultyManager() = default;
    DifficultyManager(const DifficultyManager&) = delete;
    DifficultyManager& operator=(const DifficultyManager&) = delete;
};