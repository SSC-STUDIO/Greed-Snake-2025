#include <graphics.h>
#include <conio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <thread>
#include <windows.h>
#include <math.h>
#include <queue>
#include <algorithm>
#include <mutex>
#include <ctime>
#include <condition_variable>
#include <iostream>
#include <chrono>
#include <mmsystem.h>
#include "Gameplay/GameConfig.h"
#include "Core/Vector2.h"
#include "Gameplay/Snake.h"
#include "core/GameState.h"
#include "Utils/Rendering.h"
#include "core/Collisions.h"
#include "Core/Camera.h"
#include "Gameplay/Food.h"
#include "Utils/Setting.h"
#include "UI/Button.h"
#include "Utils/InputHandler.h"       
#include "Gameplay/GameInitializer.h"           
#include "UI/UI.h"                 
#pragma comment(lib, "winmm.lib")  // 多媒体函数库
#pragma warning(disable: 4996)      // 禁用_tcscpy和_stprintf安全警告

// GameConfig全局变量定义
bool GameConfig::SOUND_ON = true;      // 音效开关
bool GameConfig::ANIMATIONS_ON = true; // 动画开关

// 窗口尺寸常量
const int windowWidth = GameConfig::WINDOW_WIDTH; 
const int windowHeight = GameConfig::WINDOW_HEIGHT; 

// 游戏区域边界
const int playAreaWidth = GameConfig::WINDOW_WIDTH; 
const int playAreaHeight = GameConfig::WINDOW_HEIGHT; 
const int playAreaMarginX = GameConfig::PLAY_AREA_MARGIN; 
const int playAreaMarginY = GameConfig::PLAY_AREA_MARGIN; 

// 玩家状态
Vector2 playerPosition = GameConfig::PLAYER_DEFAULT_POS; 
Vector2 playerDirection(0, 1); 

// 游戏计时
float deltaTime = GameState::Instance().deltaTime; 

// 动画计时器
float animationTimer = 0.0f;

// 游戏对象集合
FoodItem foodList[GameConfig::MAX_FOOD_COUNT];
std::vector<SnakeSegment> snakeSegments(5); 
std::vector<AISnake> aiSnakeList;
Snake snake[1];  // 玩家蛇

// 游戏系统类 - 管理所有游戏对象和逻辑
class GameSystem {
private:
    std::vector<FoodItem> foodItems; // 食物集合
    PlayerSnake playerSnake;         // 玩家蛇
    std::vector<AISnake> aiSnakes;   // AI蛇集合
    Camera camera;                   // 相机
    
public:
    GameSystem() {
        InitializeGame(); // 初始化游戏
    }
    
    void InitializeGame() {
        // 初始化食物
        foodItems.resize(GameConfig::MAX_FOOD_COUNT);
        for (int i = 0; i < GameConfig::MAX_FOOD_COUNT; ++i) {
            InitFood(foodList, i, GameState::Instance().currentPlayerSpeed);
        }
        
        // 初始化玩家蛇
        InitializePlayerSnake();
        
        // 初始化AI蛇
        InitializeAISnakes();
        
        // 重置游戏状态
        GameState::Instance().Initial();
    }
    
    void Update(float deltaTime) {
        if (!GameState::Instance().isGameRunning) return; 
        
        UpdateCamera();              // 更新相机
        UpdatePlayerSnake(deltaTime); // 更新玩家蛇
        UpdateAISnakes(deltaTime);   // 更新AI蛇
        UpdateFoods(foodList, GameConfig::MAX_FOOD_COUNT); // 更新食物
        CheckCollisions();           // 检测碰撞
        GameState::Instance().UpdateGameTime(deltaTime);   // 更新游戏时间
    }
    
    void Draw() {
        if (!GameState::Instance().isGameRunning) return;
        
        BeginBatchDraw(); // 开始批量绘制
        
        DrawGameArea();   // 绘制游戏区域
        DrawFoods(foodItems.data(), foodItems.size()); // 绘制食物

        // 绘制AI蛇
        for (const auto& aiSnake : aiSnakes) {
            aiSnake.Draw(camera);
        }
        
        playerSnake.Draw(camera); // 绘制玩家蛇
        DrawUI();                 // 绘制UI
        
        EndBatchDraw(); // 结束批量绘制
    }
};

// 主绘制函数 - 确保线程安全的图形初始化
void Draw() {
    try {
        
while (GameState::Instance().GetIsGameRunning()) {
            auto& gameState = GameState::Instance();
            BeginBatchDraw();
            
            bool isPaused = gameState.GetIsPaused();
            
            // 游戏未暂停时更新游戏对象
            if (!isPaused) {
                UpdateCamera();                    // 更新相机
                UpdatePlayerSnake(gameState.deltaTime); // 更新玩家蛇
                UpdateAISnakes(gameState.deltaTime);   // 更新AI蛇
                UpdateFoods(foodList, GameConfig::MAX_FOOD_COUNT); // 更新食物
                CheckGameState(snake);             // 检查游戏状态
                CheckCollisions();                 // 检测碰撞
                
                // 更新动画计时器
                {
                    std::lock_guard<std::mutex> lock(gameState.stateMutex);
                    animationTimer += gameState.deltaTime;
                    if (animationTimer > 1000.0f) {
                        animationTimer = 0.0f;
                    }
                }
            }
            
            // 始终绘制当前游戏状态
            DrawGameArea();
            
            // 创建线程安全的蛇对象副本
            PlayerSnake playerSnakeObj;
            {
                std::lock_guard<std::mutex> lock(gameState.stateMutex);
                playerSnakeObj.position = snake[0].position;
                playerSnakeObj.direction = snake[0].direction;
                playerSnakeObj.radius = snake[0].radius;
                playerSnakeObj.color = snake[0].color;
                
                // 复制蛇身体段
                playerSnakeObj.segments.resize(snake[0].segments.size());
                for (size_t i = 0; i < snake[0].segments.size(); ++i) {
                    playerSnakeObj.segments[i] = snake[0].segments[i];
                }
            }
            
            DrawVisibleObjects(foodList, GameConfig::MAX_FOOD_COUNT, 
                              aiSnakeList.data(), 
                              static_cast<int>(aiSnakeList.size()), 
                              playerSnakeObj); // 绘制所有可见对象

            // 仅在未暂停时更新生长动画
            if (!isPaused) {
                UpdateGrowthAnimation(gameState.deltaTime);
            }
                           
            DrawUI(); // 绘制UI
            
            EndBatchDraw(); // 结束批量绘制
            Sleep(1000 / 60);  // 帧率控制，约60FPS
        }
    }
    catch (const std::exception& e) {
        // 捕获并记录严重错误
        OutputDebugStringA(e.what());
        MessageBox(GetHWnd(), _T("绘制线程遇到错误"), _T("错误"), MB_OK | MB_ICONERROR);
        
        // 确保游戏能正常退出
        GameState::Instance().SetIsGameRunning(false);
    }
}

int main() {
    // 初始化图形系统
    initgraph(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);

    PlayStartAnimation(); // 播放开场动画
    LoadButton();        // 加载按钮

    // 主程序循环 - 直到选择退出
    bool quitProgram = false;
    
    while (!quitProgram) {
        // 加载并缩放背景图片
        IMAGE backgroundImage;
        loadimage(&backgroundImage, _T(".\\Resource\\Greed-Snake-BG.png"));

        // 缩放背景以适应窗口
        IMAGE scaledBG;
        scaledBG.Resize(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
        StretchBlt(GetImageHDC(&scaledBG), 0, 0, GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT,
            GetImageHDC(&backgroundImage), 0, 0, backgroundImage.getwidth(), backgroundImage.getheight(), SRCCOPY);

        // 应用背景图片
        BeginBatchDraw();
        putimage(0, 0, &scaledBG);
        EndBatchDraw();

        // 菜单循环
        bool showMenu = true;
        bool startGame = false;
        
while (showMenu && !quitProgram) {
            int menuChoice = ShowGameMenu(); // 显示菜单并获取用户选择
            
            switch (menuChoice) {
                case StartGame:
                    cleardevice();
                    startGame = true;
                    showMenu = false;
                    break;
                    
                case Setting:
                    ShowSettings(windowWidth, windowHeight); // 显示设置菜单
                    putimage(0, 0, &scaledBG); // 重绘背景
                    break;
                    
                case About:
                    ShowAbout(); // 显示关于对话框
                    putimage(0, 0, &scaledBG); // 重绘背景
                    break;
                    
                case Exit:
                    quitProgram = true;
                    showMenu = false;
                    break;
                    
                default:
                    break;
            }
        }

// 游戏主循环
        while (startGame && !quitProgram) {
            GameState::Instance().Initial(); // 初始化游戏状态
            InitGlobal();                   // 初始化全局变量

            PlayBackgroundMusic();          // 播放背景音乐

            // 初始化玩家蛇
            Vector2 centerPos(windowWidth / 2, (windowHeight / 2));
            snake[0].position = centerPos;
            snake[0].direction = Vector2(1, 0);
            snake[0].radius = GameConfig::INITIAL_SNAKE_SIZE;
            snake[0].color = HSLtoRGB(255, 255, 255);

            // 初始化蛇身体段
            snake[0].segments.resize(4);
            for (int i = 0; i < snake[0].segments.size(); i++) {
                snake[0].segments[i].position = centerPos - Vector2(1, 0) * ((i+1) * GameConfig::SNAKE_SEGMENT_SPACING);
                snake[0].segments[i].direction = Vector2(1, 0);
                snake[0].segments[i].radius = GameConfig::INITIAL_SNAKE_SIZE;
                snake[0].segments[i].color = HSLtoRGB(255, 255, 255);
            }

            // 初始化食物位置
            for (int i = 0; i < GameConfig::MAX_FOOD_COUNT; ++i) {
                InitFood(foodList, i, GameState::Instance().currentPlayerSpeed);
            }

            InitializeAISnakes(); // 初始化AI蛇

            // 线程异常处理
            std::exception_ptr drawThreadException = nullptr;
            std::exception_ptr inputThreadException = nullptr;
            
// 启动异步线程并捕获异常
            std::thread draw([&drawThreadException]() {
                try {
                    Draw();
                } catch(...) {
                    drawThreadException = std::current_exception();
                }
            });
            
            std::thread input([&inputThreadException]() {
                try {
                    EnterChanges();
                } catch(...) {
                    inputThreadException = std::current_exception();
                }
            });

            // 游戏运行循环
            bool gameRunning = true;
            while (gameRunning) {
                // 检查线程异常
                if (drawThreadException) {
                    try {
                        std::rethrow_exception(drawThreadException);
                    } catch (const std::exception& e) {
                        MessageBox(GetHWnd(), _T("绘制线程错误"), _T("错误"), MB_OK | MB_ICONERROR);
                        OutputDebugStringA(e.what());
                    }
                    GameState::Instance().SetIsGameRunning(false);
                    gameRunning = false;
                    break;
                }
                
                if (inputThreadException) {
                    try {
                        std::rethrow_exception(inputThreadException);
                    } catch (const std::exception& e) {
                        MessageBox(GetHWnd(), _T("输入线程错误"), _T("错误"), MB_OK | MB_ICONERROR);
                        OutputDebugStringA(e.what());
                    }
                    GameState::Instance().SetIsGameRunning(false);
                    gameRunning = false;
                    break;
                }
                
// 检查ESC菜单退出请求
                bool shouldExit;
                {
                    std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex);
                    shouldExit = GameState::Instance().exitGame;
                }
                
                if (shouldExit) {
                    quitProgram = true;
                    gameRunning = false;
                    GameState::Instance().SetIsGameRunning(false);
                    break;
                }
                
                // 安全获取游戏状态
                bool isGameRunning, isPaused, showDeathMessage;
                {
                    std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex);
                    isGameRunning = GameState::Instance().isGameRunning;
                    isPaused = GameState::Instance().isPaused;
                    showDeathMessage = GameState::Instance().showDeathMessage;
                }
                
                // 仅在未暂停时更新游戏时间
                if (isGameRunning && !isPaused) {
                    GameState::Instance().UpdateGameTime(GameState::Instance().deltaTime);
                }
                
// 处理游戏结束条件
                if (!isGameRunning && showDeathMessage) {
                    // 停止绘制和输入线程以防止干扰
                    GameState::Instance().SetIsGameRunning(false);
                    
                    // 安全等待线程结束
                    if (input.joinable()) input.join();
                    if (draw.joinable()) draw.join();
                    
                    cleardevice(); // 清屏
                    
                    // 在主线程显示死亡消息
                    GameState::Instance().ShowDeathMessage();
                    {
                        std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex);
                        GameState::Instance().showDeathMessage = false;
                    }
                    
                    gameRunning = false; // 退出游戏循环
                }
                
                // 重新安全获取游戏状态
                {
                    std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex);
                    isGameRunning = GameState::Instance().isGameRunning;
                    showDeathMessage = GameState::Instance().showDeathMessage;
                }
                
                // 游戏结束且死亡消息已处理则继续
                if (!isGameRunning && !showDeathMessage) {
                    gameRunning = false;
                }
                
                Sleep(10); // 降低CPU使用率
            }

// 清理游戏资源
            {
                std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex);
                if (GameState::Instance().isGameRunning) {
                    GameState::Instance().isGameRunning = false;  // 确保绘制线程退出
                }
            }
            
            // 安全等待线程结束
            if (input.joinable()) input.join();
            if (draw.joinable()) draw.join();
            
            Sleep(500);  // 短暂延迟确保资源清理
            
            // 决定下一步操作
            if (GameState::Instance().returnToMenu) {
                GameState::Instance().returnToMenu = false;
                startGame = false;  // 退出游戏循环返回菜单
            } else if (GameState::Instance().exitGame) {
                quitProgram = true;  // 退出程序
            } else {
                // 保持startGame为true以重新开始游戏
            }
        }
    }

    // 退出前清理资源
    CleanupAudioResources(); // 清理音频资源
    closegraph();            // 关闭图形窗口
    return 0;                // 退出程序
}

