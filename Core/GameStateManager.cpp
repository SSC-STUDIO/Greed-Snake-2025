#include "GameStateManager.h"

GameStateManager& GameStateManager::Instance() {
    static GameStateManager instance;
    return instance;
}

bool GameStateManager::GetIsGameRunning() {
    std::lock_guard<std::mutex> lock(stateMutex);
    return isGameRunning;
}

void GameStateManager::SetIsGameRunning(bool running) {
    std::lock_guard<std::mutex> lock(stateMutex);
    isGameRunning = running;
}

bool GameStateManager::GetIsPaused() {
    std::lock_guard<std::mutex> lock(stateMutex);
    return isPaused;
}

void GameStateManager::SetIsPaused(bool paused) {
    std::lock_guard<std::mutex> lock(stateMutex);
    isPaused = paused;
}

bool GameStateManager::GetIsMenuShowing() {
    std::lock_guard<std::mutex> lock(stateMutex);
    return isMenuShowing;
}

void GameStateManager::SetIsMenuShowing(bool showing) {
    std::lock_guard<std::mutex> lock(stateMutex);
    isMenuShowing = showing;
}

void GameStateManager::Initialize() {
    std::lock_guard<std::mutex> lock(stateMutex);
    isGameRunning = true;
    isPaused = false;
    isMenuShowing = false;
}