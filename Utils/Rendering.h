#pragma once
#include "../Core/Vector2.h"
#include "../Gameplay/GameConfig.h"
#include "../Core/GameState.h"
#include "../Core/Camera.h"
#include "../Gameplay/Food.h"
#include "../Gameplay/Snake.h"

// Forward declarations of classes
class Snake;
class PlayerSnake;
class AISnake;
struct FoodItem;

// Function declarations for rendering game elements
void DrawGameArea(); // Draw the main game area
void DrawFoods(const FoodItem* foodList, int foodCount); // Draw food items
void DrawVisibleObjects(const FoodItem* foodList, int foodCount, 
                        const AISnake* aiSnakes, int aiSnakeCount,
                        const PlayerSnake& playerSnake); // Draw all visible game objects
void DrawCircleWithCamera(const Vector2& screenPos, float r, int c); // Draw a circle considering camera position
void DebugDrawText(const std::wstring& text, int x, int y, int color); // Draw debug text
void DrawSnakeEyes(const Vector2& position, const Vector2& direction, float radius); // Draw snake eyes
bool IsCircleInScreen(const Vector2& center, float r); // Check if a circle is within the screen bounds
void DrawUI(); // Draw the user interface elements
void DrawEnhancedFood(const Vector2& screenPos, float radius, int color, int index); // Draw food with enhanced visual effects
void DrawSnakeSegment(const Vector2& screenPos, float radius, int color, int segmentIndex); // Draw a segment of the snake
void DrawPauseMenu();