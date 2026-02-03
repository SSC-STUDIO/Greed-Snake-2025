#pragma once

#include <mutex>

class UIManager {
private:
    float collisionFlashTimer;
    bool isCollisionFlashing;
    bool showDeathMessage;
    mutable std::mutex uiMutex;

public:
    static UIManager& Instance();
    
    // Visual effects
    float GetCollisionFlashTimer() const;
    void SetCollisionFlashTimer(float timer);
    
    bool GetIsCollisionFlashing() const;
    void SetIsCollisionFlashing(bool flashing);
    
    // Message display
    bool GetShowDeathMessage() const;
    void SetShowDeathMessage(bool show);
    
    // Initialize UI state
    void Initialize();
    
private:
    UIManager() = default;
    ~UIManager() = default;
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;
};