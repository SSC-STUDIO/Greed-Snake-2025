#pragma once
#include "../Gameplay/GameConfig.h"
#include "../Core/GameState.h"
#include "../Core/Vector2.h"
#include "../Core/Camera.h"
#include "../Utils/Rendering.h"
#include "Food.h"
#include <queue>

// Forward declaration of FoodItem structure
struct FoodItem;

// Structure representing a segment of the snake
struct SnakeSegment {
    Vector2 position; // Position of the snake segment
    Vector2 direction; // Direction of the snake segment
    std::queue<Vector2> positionHistory; // Record of position history
    int colorValue = HSLtoRGB(255, 255, 255); // Default white
    float collisionRadius = GameConfig::INITIAL_SNAKE_SIZE; // Default collision radius
    float timeSinceLastRecord = 0; // Time since the last record was made

    // Function to get the velocity of the snake segment
    Vector2 GetVelocity() const;

    // Function to check if the position can be recorded
    bool CanRecordPosition() const;
};

// Class representing the snake
class Snake {
public:
    Vector2 position;
    Vector2 direction;
    std::queue<Vector2> posRecords;
    int color = HSLtoRGB(255, 255, 255);
    float radius = GameConfig::INITIAL_SNAKE_SIZE;
    float currentTime = 0;
    std::vector<Snake> segments;

    // Function to update the snake
    virtual void Update(float deltaTime);

    // Function to get the velocity of the snake
    Vector2 GetVelocity() const;

    // Function to check if it's time to start recording the position
    bool IsBeginRecord() const;

    // Function to record the position
    void RecordPos();

    // Function to get the time of the position record
    Vector2 GetRecordTime() const;

    // Function to update the body of the snake
    void UpdateBody(const Snake& lastBody, Snake& currentBody);

    // Function to draw the snake
    virtual void Draw(const Camera& camera) const;

    // Function to check for collision with another snake
    virtual bool CheckCollisionWith(const Snake& other) const;

    // Function to check for collision with a point
    virtual bool CheckCollisionWithPoint(const Vector2& point, float pointRadius) const;
};

// Class representing the player's snake, inheriting from Snake
class PlayerSnake : public Snake {
public:
    std::vector<Snake> segments;
    bool isInvincible = false; // Whether the player is invincible
    float invincibilityTimer = 0.0f; // Invincibility timer
    int livesRemaining = 3; // Remaining lives
    int score = 0; // Player's score

    // Function to update the player's snake
    void Update(float deltaTime) override;

    // Function to draw the player's snake
    void Draw(const Camera& camera) const override;

    // Function to check for collision with another snake
    bool CheckCollisionWith(const Snake& other) const override;
};

// Class representing the AI snake, inheriting from Snake
class AISnake : public Snake {
public:
    float directionChangeTimer = 0.0f; // Timer for changing direction
    float speedMultiplier = 1.0f; // Speed multiplier
    float aggressionFactor = GameConfig::Difficulty::Normal::AI_AGGRESSION; // Aggression factor
    std::vector<Snake> segments;
    std::deque<Vector2> recordedPositions;
    bool isDying = false; // Whether the AI snake is dying
    float deathTimer = 0.0f; // Timer for the death animation
    int dyingSegmentIndex = -1; // Index of the segment currently disappearing, -1 for the head
    float segmentFadeTime = 0.2f; // Time for each segment to fade
    int foodValueOnDeath = 0; // Food value provided on death

    // Constructor to initialize the AI snake
    AISnake() {
        Initialize();
    }

    // Function to initialize the AI snake
    void Initialize();

    // Function to update the AI snake based on food items, time, and player's head position
    void Update(const std::vector<FoodItem>& foodItems, float deltaTime, const Vector2& playerHeadPos);

    // Function to draw the AI snake
    void Draw(const Camera& camera) const override;

    // Function to update the death animation of the AI snake
    void UpdateDeathAnimation(float deltaTime);

    // Function to start the death process of the AI snake
    void StartDying(int foodValue);

    // Function to check for collision with another snake
    bool CheckCollisionWith(const Snake& other) const override;
};