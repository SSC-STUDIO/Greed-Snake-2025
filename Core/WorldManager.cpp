#include "WorldManager.h"

WorldManager& WorldManager::Instance() {
    static WorldManager instance;
    return instance;
}

Camera WorldManager::GetCamera() const {
    std::lock_guard<std::mutex> lock(worldMutex);
    return camera;
}

void WorldManager::SetCamera(const Camera& camera) {
    std::lock_guard<std::mutex> lock(worldMutex);
    this->camera = camera;
}

unsigned int WorldManager::GetWorldSeed() const {
    std::lock_guard<std::mutex> lock(worldMutex);
    return worldSeed;
}

void WorldManager::SetWorldSeed(unsigned int seed) {
    std::lock_guard<std::mutex> lock(worldMutex);
    worldSeed = seed;
}

void WorldManager::Initialize() {
    std::lock_guard<std::mutex> lock(worldMutex);
    camera = Camera();
    worldSeed = 0;
}