#include "GameTimeManager.h"

GameTimeManager& GameTimeManager::Instance() {
    static GameTimeManager instance;
    return instance;
}

float GameTimeManager::GetDeltaTime() const {
    std::lock_guard<std::mutex> lock(timeMutex);
    return deltaTime;
}

void GameTimeManager::SetDeltaTime(float dt) {
    std::lock_guard<std::mutex> lock(timeMutex);
    deltaTime = dt;
}

void GameTimeManager::UpdateDeltaTime() {
    std::lock_guard<std::mutex> lock(timeMutex);
    DWORD currentTime = GetTickCount();
    deltaTime = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;
}

bool GameTimeManager::GetExitGame() const {
    std::lock_guard<std::mutex> lock(timeMutex);
    return exitGame;
}

void GameTimeManager::SetExitGame(bool exit) {
    std::lock_guard<std::mutex> lock(timeMutex);
    exitGame = exit;
}

void GameTimeManager::Initialize() {
    std::lock_guard<std::mutex> lock(timeMutex);
    deltaTime = 1.0f / 30.0f;
    lastTime = GetTickCount();
    exitGame = false;
}