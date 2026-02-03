#pragma once

#include "Camera.h"
#include <mutex>

class WorldManager {
private:
    Camera camera;
    unsigned int worldSeed;
    mutable std::mutex worldMutex;

public:
    static WorldManager& Instance();
    
    // Camera management
    Camera GetCamera() const;
    void SetCamera(const Camera& camera);
    
    // World seed
    unsigned int GetWorldSeed() const;
    void SetWorldSeed(unsigned int seed);
    
    // Initialize world state
    void Initialize();
    
private:
    WorldManager() = default;
    ~WorldManager() = default;
    WorldManager(const WorldManager&) = delete;
    WorldManager& operator=(const WorldManager&) = delete;
};