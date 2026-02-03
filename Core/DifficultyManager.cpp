#include "DifficultyManager.h"

DifficultyManager& DifficultyManager::Instance() {
    static DifficultyManager instance;
    return instance;
}

GameDifficulty DifficultyManager::GetCurrentDifficulty() const {
    std::lock_guard<std::mutex> lock(difficultyMutex);
    return currentDifficulty;
}

void DifficultyManager::SetDifficulty(GameDifficulty diff) {
    std::lock_guard<std::mutex> lock(difficultyMutex);
    currentDifficulty = diff;
    ApplyDifficultySettings();
}

int DifficultyManager::GetDifficultyLevel() const {
    std::lock_guard<std::mutex> lock(difficultyMutex);
    return difficulty;
}

void DifficultyManager::SetDifficultyLevel(int level) {
    std::lock_guard<std::mutex> lock(difficultyMutex);
    difficulty = level;
}

float DifficultyManager::GetAIAggression() const {
    std::lock_guard<std::mutex> lock(difficultyMutex);
    return aiAggression;
}

void DifficultyManager::SetAIAggression(float aggression) {
    std::lock_guard<std::mutex> lock(difficultyMutex);
    aiAggression = aggression;
}

float DifficultyManager::GetFoodSpawnRate() const {
    std::lock_guard<std::mutex> lock(difficultyMutex);
    return foodSpawnRate;
}

void DifficultyManager::SetFoodSpawnRate(float rate) {
    std::lock_guard<std::mutex> lock(difficultyMutex);
    foodSpawnRate = rate;
}

int DifficultyManager::GetAISnakeCount() const {
    std::lock_guard<std::mutex> lock(difficultyMutex);
    return aiSnakeCount;
}

void DifficultyManager::SetAISnakeCount(int count) {
    std::lock_guard<std::mutex> lock(difficultyMutex);
    aiSnakeCount = count;
}

float DifficultyManager::GetLavaWarningTime() const {
    std::lock_guard<std::mutex> lock(difficultyMutex);
    return lavaWarningTime;
}

void DifficultyManager::SetLavaWarningTime(float time) {
    std::lock_guard<std::mutex> lock(difficultyMutex);
    lavaWarningTime = time;
}

void DifficultyManager::Initialize() {
    std::lock_guard<std::mutex> lock(difficultyMutex);
    currentDifficulty = GameDifficulty::Normal;
    difficulty = 1; // Default to normal
    ApplyDifficultySettings();
}

void DifficultyManager::ApplyDifficultySettings() {
    switch (currentDifficulty) {
        case GameDifficulty::Easy:
            aiAggression = 0.3f;
            foodSpawnRate = 1.2f;
            aiSnakeCount = 10;
            lavaWarningTime = 7.0f;
            difficulty = 0;
            break;
            
        case GameDifficulty::Normal:
            aiAggression = 0.6f;
            foodSpawnRate = 1.0f;
            aiSnakeCount = 20;
            lavaWarningTime = 5.0f;
            difficulty = 1;
            break;
            
        case GameDifficulty::Hard:
            aiAggression = 0.9f;
            foodSpawnRate = 0.8f;
            aiSnakeCount = 30;
            lavaWarningTime = 3.0f;
            difficulty = 2;
            break;
    }
}