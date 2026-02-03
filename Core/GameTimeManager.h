#pragma once

#include <windows.h>
#include <mutex>

class GameTimeManager {
private:
    float deltaTime;
    DWORD lastTime;
    bool exitGame;
    mutable std::mutex timeMutex;

public:
    static GameTimeManager& Instance();
    
    // Time control
    float GetDeltaTime() const;
    void SetDeltaTime(float dt);
    
    // Timing calculation
    void UpdateDeltaTime();
    
    // Game exit control
    bool GetExitGame() const;
    void SetExitGame(bool exit);
    
    // Initialize time
    void Initialize();
    
private:
    GameTimeManager() = default;
    ~GameTimeManager() = default;
    GameTimeManager(const GameTimeManager&) = delete;
    GameTimeManager& operator=(const GameTimeManager&) = delete;
};