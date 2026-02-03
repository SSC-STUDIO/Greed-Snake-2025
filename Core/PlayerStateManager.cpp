#include "PlayerStateManager.h"

PlayerStateManager& PlayerStateManager::Instance() {
    static PlayerStateManager instance;
    return instance;
}

Vector2 PlayerStateManager::GetPlayerPosition() const {
    std::lock_guard<std::mutex> lock(stateMutex);
    return playerPosition;
}

void PlayerStateManager::SetPlayerPosition(const Vector2& position) {
    std::lock_guard<std::mutex> lock(stateMutex);
    playerPosition = position;
}

Vector2 PlayerStateManager::GetTargetDirection() const {
    std::lock_guard<std::mutex> lock(stateMutex);
    return targetDirection;
}

void PlayerStateManager::SetTargetDirection(const Vector2& direction) {
    std::lock_guard<std::mutex> lock(stateMutex);
    targetDirection = direction;
}

float PlayerStateManager::GetCurrentPlayerSpeed() const {
    std::lock_guard<std::mutex> lock(stateMutex);
    return currentPlayerSpeed;
}

void PlayerStateManager::SetCurrentPlayerSpeed(float speed) {
    std::lock_guard<std::mutex> lock(stateMutex);
    currentPlayerSpeed = speed;
}

float PlayerStateManager::GetOriginalSpeed() const {
    std::lock_guard<std::mutex> lock(stateMutex);
    return originalSpeed;
}

void PlayerStateManager::SetOriginalSpeed(float speed) {
    std::lock_guard<std::mutex> lock(stateMutex);
    originalSpeed = speed;
}

bool PlayerStateManager::GetIsMouseControlEnabled() const {
    std::lock_guard<std::mutex> lock(stateMutex);
    return isMouseControlEnabled;
}

void PlayerStateManager::SetIsMouseControlEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(stateMutex);
    isMouseControlEnabled = enabled;
}

float PlayerStateManager::GetTimeInLava() const {
    std::lock_guard<std::mutex> lock(stateMutex);
    return timeInLava;
}

void PlayerStateManager::SetTimeInLava(float time) {
    std::lock_guard<std::mutex> lock(stateMutex);
    timeInLava = time;
}

void PlayerStateManager::ResetLavaTimer() {
    std::lock_guard<std::mutex> lock(stateMutex);
    timeInLava = 0.0f;
}

bool PlayerStateManager::GetIsInLava() const {
    std::lock_guard<std::mutex> lock(stateMutex);
    return isInLava;
}

void PlayerStateManager::SetIsInLava(bool inLava) {
    std::lock_guard<std::mutex> lock(stateMutex);
    isInLava = inLava;
}

bool PlayerStateManager::GetIsInvulnerable() const {
    std::lock_guard<std::mutex> lock(stateMutex);
    return isInvulnerable;
}

void PlayerStateManager::SetIsInvulnerable(bool invulnerable) {
    std::lock_guard<std::mutex> lock(stateMutex);
    isInvulnerable = invulnerable;
}

void PlayerStateManager::Initialize() {
    std::lock_guard<std::mutex> lock(stateMutex);
    playerPosition = Vector2();
    targetDirection = Vector2(0, 1);
    currentPlayerSpeed = 250.0f;
    originalSpeed = 250.0f;
    isMouseControlEnabled = true;
    timeInLava = 0.0f;
    isInLava = false;
    isInvulnerable = true;
}