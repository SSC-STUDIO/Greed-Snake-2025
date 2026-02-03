#pragma once

#include <graphics.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "../Gameplay/GameConfig.h"
#include "GameState.h"
#include "../Gameplay/Snake.h"
#include "Camera.h"
#include "../Gameplay/Food.h"

/**
 * @brief 游戏引擎类 - 管理整个游戏的生命周期
 * 
 * 负责游戏初始化、主循环、状态管理和资源清理
 */
class GameEngine {
private:
    std::unique_ptr<ThreadManager> threadManager;
    
    // 游戏对象
    Snake snake[1];
    FoodItem foodList[GameConfig::MAX_FOOD_COUNT];
    std::vector<AISnake> aiSnakeList;
    Camera camera;
    
    // 计时相关
    float animationTimer;
    bool isInitialized;
    
    // 窗口属性
    int windowWidth;
    int windowHeight;
    
    // 游戏状态
    bool gameRunning;
    
    // 私有方法
    void InitializeGameObjects();  // 初始化游戏对象
    void ResetGameState();          // 重置游戏状态
    void UpdateGameLoop();          // 更新游戏循环
    void HandleMenuLoop();          // 处理菜单循环
    void StartGameLoop();           // 开始游戏循环
    void CleanupGame();             // 清理游戏资源
    
public:
    GameEngine();   // 构造函数
    ~GameEngine();  // 析构函数
    
    // 主接口
    void Run();  // 运行游戏引擎
    
    // 游戏状态控制
    void InitializeGame();  // 初始化游戏
    void StartGame();       // 开始游戏
    void PauseGame();       // 暂停游戏
    void ResumeGame();      // 恢复游戏
    void StopGame();        // 停止游戏
    void ReturnToMenu();    // 返回菜单
    void ExitGame();        // 退出游戏
    
    // 获取器
    bool IsRunning() const;    // 检查是否正在运行
    bool IsInitialized() const; // 检查是否已初始化
    
private:
    // 线程安全操作
    void UpdateGameObjects(float deltaTime); // 更新游戏对象
    void RenderGame();                       // 渲染游戏
    void HandleInput();                      // 处理输入
    
    // 初始化辅助函数
    void InitializePlayerSnake(); // 初始化玩家蛇
    void InitializeFood();        // 初始化食物
    void InitializeAISnakes();    // 初始化AI蛇
    
    // 游戏循环辅助函数
    bool CheckGameEndConditions(); // 检查游戏结束条件
    void ProcessGameState();      // 处理游戏状态
};