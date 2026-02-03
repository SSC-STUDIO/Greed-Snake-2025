#include "GameProgressManager.h"

GameProgressManager& GameProgressManager::Instance() {
    static GameProgressManager instance;
    return instance;
}

int GameProgressManager::GetFoodEatenCount() const {
    std::lock_guard<std::mutex> lock(progressMutex);
    return foodEatenCount;
}

void GameProgressManager::AddFoodEaten() {
    std::lock_guard<std::mutex> lock(progressMutex);
    foodEatenCount++;
}

void GameProgressManager::SetFoodEatenCount(int count) {
    std::lock_guard<std::mutex> lock(progressMutex);
    foodEatenCount = count;
}

int GameProgressManager::GetFinalScore() const {
    std::lock_guard<std::mutex> lock(progressMutex);
    return finalScore;
}

void GameProgressManager::SetFinalScore(int score) {
    std::lock_guard<std::mutex> lock(progressMutex);
    finalScore = score;
}

float GameProgressManager::GetGameStartTime() const {
    std::lock_guard<std::mutex> lock(progressMutex);
    return gameStartTime;
}

void GameProgressManager::SetGameStartTime(float time) {
    std::lock_guard<std::mutex> lock(progressMutex);
    gameStartTime = time;
}

void GameProgressManager::Initialize() {
    std::lock_guard<std::mutex> lock(progressMutex);
    foodEatenCount = 0;
    finalScore = 0;
    gameStartTime = 0.0f;
}