#include "UIManager.h"

UIManager& UIManager::Instance() {
    static UIManager instance;
    return instance;
}

float UIManager::GetCollisionFlashTimer() const {
    std::lock_guard<std::mutex> lock(uiMutex);
    return collisionFlashTimer;
}

void UIManager::SetCollisionFlashTimer(float timer) {
    std::lock_guard<std::mutex> lock(uiMutex);
    collisionFlashTimer = timer;
}

bool UIManager::GetIsCollisionFlashing() const {
    std::lock_guard<std::mutex> lock(uiMutex);
    return isCollisionFlashing;
}

void UIManager::SetIsCollisionFlashing(bool flashing) {
    std::lock_guard<std::mutex> lock(uiMutex);
    isCollisionFlashing = flashing;
}

bool UIManager::GetShowDeathMessage() const {
    std::lock_guard<std::mutex> lock(uiMutex);
    return showDeathMessage;
}

void UIManager::SetShowDeathMessage(bool show) {
    std::lock_guard<std::mutex> lock(uiMutex);
    showDeathMessage = show;
}

void UIManager::Initialize() {
    std::lock_guard<std::mutex> lock(uiMutex);
    collisionFlashTimer = 0.0f;
    isCollisionFlashing = false;
    showDeathMessage = false;
}