// GameEngine Core - manages game lifecycle and threading
#include "GameEngine.h"
#include "ThreadManager.h"
#include "ResourceManager.h"
#include "Collisions.h"
#include "../Utils/Rendering.h"
#include "../Utils/InputHandler.h"
#include "../Utils/Setting.h"
#include "../UI/UI.h"
#include "../Gameplay/GameInitializer.h"
#include <iostream>
#include <chrono>

GameEngine::GameEngine() 
    : animationTimer(0.0f), isInitialized(false),
      windowWidth(GameConfig::WINDOW_WIDTH),
      windowHeight(GameConfig::WINDOW_HEIGHT) {
    
    threadManager = std::make_unique<ThreadManager>();
}

GameEngine::~GameEngine() {
    CleanupGame();
}

void GameEngine::Run() {
    initgraph(windowWidth, windowHeight);
    
    PlayStartAnimation();
    
    bool quitProgram = false;
    
    while (!quitProgram) {
        HandleMenuLoop();
        
        {
            std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex);
            quitProgram = GameState::Instance().exitGame;
        }
    }
    
        CleanupGame();
        closegraph();

void GameEngine::HandleMenuLoop() {
        bool showMenu = true;
        bool startGame = false;
        
        while (showMenu) {
            DrawBackground();
            int menuChoice = ShowGameMenu();
        
        switch (menuChoice) {
            case 0:
                cleardevice();
                startGame = true;
                showMenu = false;
                break;
                
            case 1:
                ShowSettings(windowWidth, windowHeight);
                DrawBackground();
                break;
                
            case 2:
                ShowAbout();
                DrawBackground();
                break;
                
            case 3:
                GameState::Instance().exitGame = true;
                showMenu = false;
                break;
                
            default:
                break;
        }
    }
    
    if (startGame) {
        StartGameLoop();
    }
}

void GameEngine::StartGameLoop() {
    InitializeGame();
    
    threadManager->StartRenderThread([this]() { RenderGame(); });
    threadManager->StartInputThread([this]() { HandleInput(); });
    threadManager->StartUpdateThread([this]() { UpdateGameLoop(); });
    
    this->gameRunning = true;
    while (this->gameRunning) {
        if (threadManager->HasExceptions()) {
            threadManager->CheckAndRethrowExceptions();
            GameState::Instance().SetIsGameRunning(false);
            this->gameRunning = false;
            break;
        }
        
        bool shouldExit;
        {
            std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex);
            shouldExit = GameState::Instance().exitGame;
        }
        
        if (shouldExit) {
            gameRunning = false;
            GameState::Instance().SetIsGameRunning(false);
            break;
        }
        
        if (!CheckGameEndConditions()) {
            this->gameRunning = false;
        }
        
        Sleep(10);
    }
    
    threadManager->StopAllThreads();
    threadManager->JoinAllThreads();
    ProcessGameState();
    Sleep(500);
}

void GameEngine::InitializeGame() {
    if (!isInitialized) {
        InitializeGameObjects();
        isInitialized = true;
    }
    
    ResetGameState();
    PlayBackgroundMusic();
}

void GameEngine::InitializeGameObjects() {
    InitializePlayerSnake();
    InitializeFood();
    InitializeAISnakes();
}

void GameEngine::ResetGameState() {
    GameState::Instance().Initial();
    InitGlobal();
    animationTimer = 0.0f;
    
    Vector2 centerPos(windowWidth / 2, windowHeight / 2);
    snake[0].position = centerPos;
    snake[0].direction = Vector2(1, 0);
    snake[0].radius = GameConfig::INITIAL_SNAKE_SIZE;
    snake[0].color = HSLtoRGB(255, 255, 255);
    
    snake[0].segments.resize(4);
    for (int i = 0; i < snake[0].segments.size(); i++) {
        snake[0].segments[i].position = centerPos - Vector2(1, 0) * ((i + 1) * GameConfig::SNAKE_SEGMENT_SPACING);
        snake[0].segments[i].direction = Vector2(1, 0);
        snake[0].segments[i].radius = GameConfig::INITIAL_SNAKE_SIZE;
        snake[0].segments[i].color = HSLtoRGB(255, 255, 255);
    }
}

void GameEngine::InitializePlayerSnake() {
}

void GameEngine::InitializeFood() {
    for (int i = 0; i < GameConfig::MAX_FOOD_COUNT; ++i) {
        InitFood(foodList, i, GameState::Instance().currentPlayerSpeed);
    }
}

void GameEngine::InitializeAISnakes() {
    ::InitializeAISnakes();
}

void GameEngine::UpdateGameLoop() {
    while (GameState::Instance().GetIsGameRunning()) {
        bool isPaused;
        {
            std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex);
            isPaused = GameState::Instance().isPaused;
        }
        
        if (!isPaused) {
            UpdateGameObjects(GameState::Instance().deltaTime);
        }
        
        Sleep(16);
    }
}

void GameEngine::UpdateGameObjects(float deltaTime) {
    UpdateCamera();
    UpdatePlayerSnake(deltaTime);
    UpdateAISnakes(deltaTime);
    UpdateFoods(foodList, GameConfig::MAX_FOOD_COUNT);
    CheckCollisions();
    GameState::Instance().UpdateGameTime(deltaTime);
    
    {
        std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex);
        animationTimer += deltaTime;
        if (animationTimer > 1000.0f) {
            animationTimer = 0.0f;
        }
    }
}

void GameEngine::RenderGame() {
    try {
        while (GameState::Instance().GetIsGameRunning()) {
            BeginBatchDraw();
            DrawGameArea();
            
            PlayerSnake playerSnakeObj;
            {
                std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex);
                playerSnakeObj.position = snake[0].position;
                playerSnakeObj.direction = snake[0].direction;
                playerSnakeObj.radius = snake[0].radius;
                playerSnakeObj.color = snake[0].color;
                
                playerSnakeObj.segments.resize(snake[0].segments.size());
                for (size_t i = 0; i < snake[0].segments.size(); ++i) {
                    playerSnakeObj.segments[i] = snake[0].segments[i];
                }
            }
            
            DrawVisibleObjects(foodList, GameConfig::MAX_FOOD_COUNT,
                              aiSnakeList.data(),
                              static_cast<int>(aiSnakeList.size()),
                              playerSnakeObj);
            
            UpdateGrowthAnimation(GameState::Instance().deltaTime);
            DrawUI();
            EndBatchDraw();
            Sleep(1000 / 60);
        }
    } catch (const std::exception& e) {
        OutputDebugStringA(e.what());
        MessageBox(GetHWnd(), _T("Rendering error occurred"), _T("Error"), MB_OK | MB_ICONERROR);
        GameState::Instance().SetIsGameRunning(false);
    }
}

void GameEngine::HandleInput() {
    EnterChanges();
}

bool GameEngine::CheckGameEndConditions() {
    bool isGameRunning, showDeathMessage;
    {
        std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex);
        isGameRunning = GameState::Instance().isGameRunning;
        showDeathMessage = GameState::Instance().showDeathMessage;
    }
    
    return !(!isGameRunning && showDeathMessage);
}

void GameEngine::ProcessGameState() {
    bool showDeathMessage;
    {
        std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex);
        showDeathMessage = GameState::Instance().showDeathMessage;
    }
    
    if (showDeathMessage) {
        cleardevice();
        GameState::Instance().ShowDeathMessage();
        
        {
            std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex);
            GameState::Instance().showDeathMessage = false;
        }
    }
}

void GameEngine::CleanupGame() {
    GameState::Instance().SetIsGameRunning(false);
}

void GameEngine::StartGame() {
    ResetGameState();
    GameState::Instance().SetIsGameRunning(true);
}

void GameEngine::PauseGame() {
    GameState::Instance().SetIsPaused(true);
}

void GameEngine::ResumeGame() {
    GameState::Instance().SetIsPaused(false);
}

void GameEngine::StopGame() {
    GameState::Instance().SetIsGameRunning(false);
}

void GameEngine::ReturnToMenu() {
    GameState::Instance().returnToMenu = true;
}

void GameEngine::ExitGame() {
    GameState::Instance().exitGame = true;
}

bool GameEngine::IsRunning() const {
    return GameState::Instance().GetIsGameRunning();
}

bool GameEngine::IsInitialized() const {
    return isInitialized;
}