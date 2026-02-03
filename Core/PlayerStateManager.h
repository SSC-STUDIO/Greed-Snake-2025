#pragma once

#include "Vector2.h"
#include <mutex>

class PlayerStateManager {
private:
    Vector2 playerPosition;
    Vector2 targetDirection;
    float currentPlayerSpeed;
    float originalSpeed;
    bool isMouseControlEnabled;
    float timeInLava;
    bool isInLava;
    bool isInvulnerable;
    mutable std::mutex stateMutex;

public:
    static PlayerStateManager& Instance();
    
    // Player position and movement
    Vector2 GetPlayerPosition() const;
    void SetPlayerPosition(const Vector2& position);
    
    Vector2 GetTargetDirection() const;
    void SetTargetDirection(const Vector2& direction);
    
    // Speed control
    float GetCurrentPlayerSpeed() const;
    void SetCurrentPlayerSpeed(float speed);
    float GetOriginalSpeed() const;
    void SetOriginalSpeed(float speed);
    
    // Control settings
    bool GetIsMouseControlEnabled() const;
    void SetIsMouseControlEnabled(bool enabled);
    
    // Lava and vulnerability
    float GetTimeInLava() const;
    void SetTimeInLava(float time);
    void ResetLavaTimer();
    
    bool GetIsInLava() const;
    void SetIsInLava(bool inLava);
    
    bool GetIsInvulnerable() const;
    void SetIsInvulnerable(bool invulnerable);
    
    // Initialize all player states
    void Initialize();
    
private:
    PlayerStateManager() = default;
    ~PlayerStateManager() = default;
    PlayerStateManager(const PlayerStateManager&) = delete;
    PlayerStateManager& operator=(const PlayerStateManager&) = delete;
};