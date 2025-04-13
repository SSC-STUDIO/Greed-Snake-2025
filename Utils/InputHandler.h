#pragma once
#include "../Core/Vector2.h"
#include "../Gameplay/GameState.h"
#include "../Gameplay/Snake.h"
#include <graphics.h>

// Handle keyboard and mouse input during gameplay
void EnterChanges();

// Handle mouse input in menu
void HandleMouseInput();

// Check if position is in safe area
bool IsInSafeArea(const Vector2& pos); 