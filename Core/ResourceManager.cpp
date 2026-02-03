#include "ResourceManager.h"

ResourceManager& ResourceManager::Instance() {
    static ResourceManager instance;
    return instance;
}

bool ResourceManager::LoadAllResources() {
    // Simple implementation without EasyX-specific functions
    return true;
}

void ResourceManager::UnloadAllResources() {
}

bool ResourceManager::IsLoaded() const {
    return true;
}