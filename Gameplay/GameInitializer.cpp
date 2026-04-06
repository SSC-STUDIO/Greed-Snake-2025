#include "GameInitializer.h"
#include "../Gameplay/GameConfig.h"
#include "../Gameplay/Snake.h"
#include "../Core/GameRuntime.h"
#include "../Core/SecureRandom.h"
#include <algorithm>
#include <cmath>

void InitializeAISnakes() {
    auto& runtime = GameRuntime();
    auto& gameState = GameState::Instance();
    runtime.aiSnakeList.resize(gameState.aiSnakeCount);

    for (auto& aiSnake : runtime.aiSnakeList) {
        aiSnake.Initialize();
        aiSnake.aggressionFactor = gameState.aiAggression;
        aiSnake.speedMultiplier = 1.0f;
    }

    for (int i = 0; i < gameState.aiSnakeCount; ++i) {
        // SECURITY FIX: Check for valid aiSnakeCount to prevent division by zero
        if (gameState.aiSnakeCount <= 0) gameState.aiSnakeCount = 1;
        
        const float angle = (static_cast<float>(i) * 360.0f / gameState.aiSnakeCount) * 3.14159f / 180.0f;
        // SECURITY FIX: Use secure random number generator instead of rand()
        const float distance = RANDOM_FLOAT(0.0f, static_cast<float>(GameConfig::AI_SPAWN_RADIUS));

        float x = static_cast<float>(GameConfig::WINDOW_WIDTH) / 2.0f + cos(angle) * distance;
        float y = static_cast<float>(GameConfig::WINDOW_HEIGHT) / 2.0f + sin(angle) * distance;

        x = (std::max)(GameConfig::PLAY_AREA_LEFT + 100.0f, (std::min)(GameConfig::PLAY_AREA_RIGHT - 100.0f, x));
        y = (std::max)(GameConfig::PLAY_AREA_TOP + 100.0f, (std::min)(GameConfig::PLAY_AREA_BOTTOM - 100.0f, y));

        // SECURITY FIX: Use secure random number generator instead of rand()
        const float dirAngle = RANDOM_FLOAT(0.0f, 360.0f) * 3.14159f / 180.0f;
        const Vector2 direction(cos(dirAngle), sin(dirAngle));

        Direction aiDir = RIGHT;
        if (abs(direction.x) > abs(direction.y)) {
            aiDir = direction.x > 0 ? RIGHT : LEFT;
        } else {
            aiDir = direction.y > 0 ? DOWN : UP;
        }

        runtime.aiSnakeList[i].position = Vector2(x, y);
        runtime.aiSnakeList[i].direction = direction;
        runtime.aiSnakeList[i].currentDir = aiDir;
        runtime.aiSnakeList[i].nextDir = aiDir;
        runtime.aiSnakeList[i].radius = GameConfig::INITIAL_SNAKE_SIZE * 0.8f;
        // SECURITY FIX: Use secure random number generator instead of rand()
        runtime.aiSnakeList[i].color = HSLtoRGB(RANDOM_FLOAT(0.0f, 360.0f), 200.0f, 200.0f);
        runtime.aiSnakeList[i].gridSnake = true;
        runtime.aiSnakeList[i].moveTimer = 0.0f;

        for (int j = 0; j < 5; j++) {
            runtime.aiSnakeList[i].segments[j].position = runtime.aiSnakeList[i].position -
                direction * (static_cast<float>(j + 1) * GameConfig::SNAKE_SEGMENT_SPACING);
            runtime.aiSnakeList[i].segments[j].direction = direction;
            runtime.aiSnakeList[i].segments[j].radius = runtime.aiSnakeList[i].radius;
            runtime.aiSnakeList[i].segments[j].color = runtime.aiSnakeList[i].color;
        }

        runtime.aiSnakeList[i].recordedPositions.clear();
        for (int j = 0; j < 20; j++) {
            runtime.aiSnakeList[i].recordedPositions.push_back(
                runtime.aiSnakeList[i].position - direction * static_cast<float>(j) * (GameConfig::SNAKE_SEGMENT_SPACING / 4.0f));
        }
    }
}
