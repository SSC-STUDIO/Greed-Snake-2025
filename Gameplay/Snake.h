#pragma once
#include "../Gameplay/GameConfig.h"
#include "../Core/GameState.h"
#include "../Core/Vector2.h"
#include "../Core/Camera.h"
#include "../Utils/Rendering.h"
#include "Food.h"
#include <queue>

/**
 * @file Snake.h
 * @brief 蛇类及相关结构的定义
 */

// FoodItem结构前向声明
struct FoodItem;

/**
 * @brief 蛇段结构体
 * 
 * 表示蛇的一个身体段，包含位置、方向和历史记录
 */
struct SnakeSegment {
    Vector2 position;                    // 蛇段位置
    Vector2 direction;                   // 蛇段方向
    std::queue<Vector2> positionHistory; // 位置历史记录
    int colorValue = HSLtoRGB(255, 255, 255); // 颜色值（默认白色）
    float collisionRadius = GameConfig::INITIAL_SNAKE_SIZE; // 碰撞半径
    float timeSinceLastRecord = 0;       // 距离上次记录的时间

    Vector2 GetVelocity() const;        // 获取速度
    bool CanRecordPosition() const;      // 检查是否可以记录位置
};

/**
 * @brief 蛇基类
 * 
 * 定义了蛇的基本属性和行为，包括位置、方向、绘制和碰撞检测
 */
class Snake {
public:
    Vector2 position;              // 位置
    Vector2 direction;             // 方向
    std::queue<Vector2> posRecords; // 位置记录
    int color = HSLtoRGB(255, 255, 255); // 颜色
    float radius = GameConfig::INITIAL_SNAKE_SIZE; // 半径
    float currentTime = 0;         // 当前时间
    std::vector<Snake> segments;   // 身体段

    virtual void Update(float deltaTime);                                     // 更新蛇
    Vector2 GetVelocity() const;                                             // 获取速度
    bool IsBeginRecord() const;                                               // 检查是否开始记录
    void RecordPos();                                                         // 记录位置
    Vector2 GetRecordTime() const;                                            // 获取记录时间
    void UpdateBody(const Snake& lastBody, Snake& currentBody);             // 更新身体
    virtual void Draw(const Camera& camera) const;                            // 绘制蛇
    virtual bool CheckCollisionWith(const Snake& other) const;               // 检测与另一条蛇的碰撞
    virtual bool CheckCollisionWithPoint(const Vector2& point, float pointRadius) const; // 检测与点的碰撞
};

/**
 * @brief 玩家蛇类，继承自Snake
 * 
 * 添加了玩家特有属性如无敌时间、生命值和分数
 */
class PlayerSnake : public Snake {
public:
    std::vector<Snake> segments;
    bool isInvincible = false;        // 是否无敌
    float invincibilityTimer = 0.0f; // 无敌计时器
    int livesRemaining = 3;           // 剩余生命
    int score = 0;                    // 玩家分数

    void Update(float deltaTime) override;                                 // 更新玩家蛇
    void Draw(const Camera& camera) const override;                        // 绘制玩家蛇
    bool CheckCollisionWith(const Snake& other) const override;            // 检测碰撞
};

/**
 * @brief AI蛇类，继承自Snake
 * 
 * 实现了AI行为，包括方向变化、死亡动画和攻击性设置
 */
class AISnake : public Snake {
public:
    float directionChangeTimer = 0.0f;      // 方向变化计时器
    float speedMultiplier = 1.0f;           // 速度倍数
    float aggressionFactor = GameConfig::Difficulty::Normal::AI_AGGRESSION; // 攻击性因子
    std::vector<Snake> segments;
    std::deque<Vector2> recordedPositions;
    bool isDying = false;                    // 是否正在死亡
    float deathTimer = 0.0f;                // 死亡动画计时器
    int dyingSegmentIndex = -1;             // 正在消失的段索引（-1表示头部）
    float segmentFadeTime = 0.2f;           // 每段消失时间
    int foodValueOnDeath = 0;               // 死亡时提供的食物价值

    AISnake() {
        Initialize(); // 构造时初始化
    }

    void Initialize();                                                       // 初始化AI蛇
    void Update(const std::vector<FoodItem>& foodItems, float deltaTime, const Vector2& playerHeadPos); // 更新AI蛇
    void Draw(const Camera& camera) const override;                          // 绘制AI蛇
    void UpdateDeathAnimation(float deltaTime);                              // 更新死亡动画
    void StartDying(int foodValue);                                          // 开始死亡过程
    bool CheckCollisionWith(const Snake& other) const override;              // 检测碰撞
};