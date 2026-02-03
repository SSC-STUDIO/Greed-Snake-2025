#pragma once

#include <mutex>

class GameStateManager {
private:
    bool isGameRunning;
    bool isPaused;
    bool isMenuShowing;
    mutable std::mutex stateMutex;

public:
    static GameStateManager& Instance();
    
    // Game state control
    bool GetIsGameRunning();
    void SetIsGameRunning(bool running);
    
    bool GetIsPaused();
    void SetIsPaused(bool paused);
    
    bool GetIsMenuShowing();
    void SetIsMenuShowing(bool showing);
    
    // Initialize all states
    void Initialize();
    
private:
    GameStateManager() = default;
    ~GameStateManager() = default;
    GameStateManager(const GameStateManager&) = delete;
    GameStateManager& operator=(const GameStateManager&) = delete;
};