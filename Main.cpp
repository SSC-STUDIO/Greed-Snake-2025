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
#pragma comment(lib, "winmm.lib") // Required for multimedia functions
#pragma warning(disable: 4996)	 // Disable security warnings for _tcscpy and _stprintf

// Define the extern variables declared in GameConfig.h
bool GameConfig::SOUND_ON = true; // Sound toggle
bool GameConfig::ANIMATIONS_ON = true; // Animation toggle

// Screen dimensions
const int windowWidth = GameConfig::WINDOW_WIDTH; // Width of the game window
const int windowHeight = GameConfig::WINDOW_HEIGHT; // Height of the game window

// Game area boundaries
const int playAreaWidth = GameConfig::WINDOW_WIDTH; // Width of the play area
const int playAreaHeight = GameConfig::WINDOW_HEIGHT; // Height of the play area
const int playAreaMarginX = GameConfig::PLAY_AREA_MARGIN; // Margin for the play area on the X-axis
const int playAreaMarginY = GameConfig::PLAY_AREA_MARGIN; // Margin for the play area on the Y-axis

// Player status
Vector2 playerPosition = GameConfig::PLAYER_DEFAULT_POS; // Initial position of the player
Vector2 playerDirection(0, 1); // Initial direction of the player (downward)

// Game timing
float deltaTime = GameState::Instance().deltaTime; // Time increment for game updates

// Animation timer for visual effects
float animationTimer = 0.0f; // Timer for animations

// Arrays and collections
FoodItem foodList[GameConfig::MAX_FOOD_COUNT]; // Array to hold food items
std::vector<SnakeSegment> snakeSegments(5); // Vector to hold snake segments
  
// AI snake container
std::vector<AISnake> aiSnakeList; // Vector to hold AI snakes

// Player snake
Snake snake[1]; // Array to hold the player snake

// Game system class
class GameSystem {
private:
    std::vector<FoodItem> foodItems; // Vector to hold food items
    PlayerSnake playerSnake; // Player snake instance
    std::vector<AISnake> aiSnakes; // Vector to hold AI snakes
    Camera camera; // Camera instance
    
public:
    GameSystem() {
        InitializeGame(); // Initialize the game
    }
    
    void InitializeGame() {
        // Initialize food
        foodItems.resize(GameConfig::MAX_FOOD_COUNT); // Resize food items vector
        for (int i = 0; i < GameConfig::MAX_FOOD_COUNT; ++i) {
            InitFood(foodList, i, GameState::Instance().currentPlayerSpeed); // Initialize each food item
        }
        
        // Initialize player snake
        InitializePlayerSnake(); // Set up the player snake
        
        // Initialize AI snakes
        InitializeAISnakes(); // Set up AI snakes
        
        // Reset game state
        GameState::Instance().Initial(); // Initialize game state
    }
    
    void Update(float deltaTime) {
        if (!GameState::Instance().isGameRunning) return; // Exit if the game is not running
        
        // Update camera
        UpdateCamera(); // Update camera position
        
        // Update player snake
        UpdatePlayerSnake(deltaTime); // Update player snake position
        
        // Update AI snakes
        UpdateAISnakes(deltaTime); // Update AI snakes
        
        // Update food
        UpdateFoods(foodList, GameConfig::MAX_FOOD_COUNT); // Update food items
        
        // Check collisions
        CheckCollisions(); // Check for collisions
        
        // Update game state
        GameState::Instance().UpdateGameTime(deltaTime); // Update game time
    }
    
    void Draw() {
        if (!GameState::Instance().isGameRunning) return; // Exit if the game is not running
        
        BeginBatchDraw(); // Start batch drawing
        
        // Draw game area
        DrawGameArea(); // Draw the game area
        
        // Draw food - using correct function signature
        DrawFoods(foodItems.data(), foodItems.size());  // Pass correct parameters if needed

        // Draw AI snakes
        for (const auto& aiSnake : aiSnakes) {
            aiSnake.Draw(camera); // Draw each AI snake
        }
        
        // Draw player snake
        playerSnake.Draw(camera); // Draw the player snake
        
        // Draw UI
        DrawUI(); // Draw UI elements
        
        EndBatchDraw(); // End batch drawing
    }
};

// Modify the Draw function to ensure thread safety when initializing graphics
void Draw() {
    // Error handling
    try {
        // The rest of the Draw function...
        
        while (GameState::Instance().GetIsGameRunning()) {
            auto& gameState = GameState::Instance(); // Get the current game state
            BeginBatchDraw(); // Start batch drawing
            
            // Safely get pause state
            bool isPaused = gameState.GetIsPaused(); // Check if the game is paused
            
            // Skip all updates if the game is paused, just maintain the current display
            if (!isPaused) {
                // Update camera
                UpdateCamera(); // Update camera position
                
                // Update game objects only when not paused
                UpdatePlayerSnake(gameState.deltaTime); // Update player snake
                UpdateAISnakes(gameState.deltaTime); // Update AI snakes
                UpdateFoods(foodList, GameConfig::MAX_FOOD_COUNT); // Update food items
                
                // Check collision and game state only when not paused
                CheckGameState(snake);  
                CheckCollisions(); // Check for collisions
                
                // Update animation timer only when not paused
                {
                    std::lock_guard<std::mutex> lock(gameState.stateMutex); // Lock the game state mutex
                    animationTimer += gameState.deltaTime; // Increment animation timer
                    if (animationTimer > 1000.0f) {
                        animationTimer = 0.0f; // Reset timer if it exceeds 1000ms
                    }
                }
            }
            
            // Always draw the current state of the game
            DrawGameArea(); // Draw the game area
            
            // Create thread-safe copy of snake object
            PlayerSnake playerSnakeObj; // Create a copy of the player snake
            {
                std::lock_guard<std::mutex> lock(gameState.stateMutex); // Lock the game state mutex
                playerSnakeObj.position = snake[0].position; // Copy position
                playerSnakeObj.direction = snake[0].direction; // Copy direction
                playerSnakeObj.radius = snake[0].radius; // Copy radius
                playerSnakeObj.color = snake[0].color; // Copy color
                
                // Copy snake body segments
                playerSnakeObj.segments.resize(snake[0].segments.size()); // Resize segments vector
                for (size_t i = 0; i < snake[0].segments.size(); ++i) {
                    playerSnakeObj.segments[i] = snake[0].segments[i]; // Copy each segment
                }
            }
            
            DrawVisibleObjects(foodList, GameConfig::MAX_FOOD_COUNT, 
                              aiSnakeList.data(), 
                              static_cast<int>(aiSnakeList.size()), 
                              playerSnakeObj); // Draw all visible objects

            // Update growth animation effect only when not paused
            if (!isPaused) {
                UpdateGrowthAnimation(gameState.deltaTime); // Update growth animation
            }
                          
            // Draw UI elements
            DrawUI(); // Draw UI elements
            
            EndBatchDraw(); // End batch drawing
            // Add frame rate control
            Sleep(1000 / 60);  // Limit to approximately 60 FPS
        }
    }
    catch (const std::exception& e) {
        // Catch and log serious errors
        OutputDebugStringA(e.what()); // Output error message
        MessageBox(GetHWnd(), _T("Drawing thread encountered an error"), _T("Error"), MB_OK | MB_ICONERROR);
        
        // Ensure the game can exit properly
        GameState::Instance().SetIsGameRunning(false); // Set game running state to false
    }
}

int main() {
    // Initialize graphics first in the main thread
    initgraph(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT); // Initialize graphics window

    // Play start animation
    PlayStartAnimation(); // Play the start animation

    LoadButton(); // Load buttons

    // Main program loop - keep running until exit is selected
    bool quitProgram = false; // Flag to control the main loop
    
    while (!quitProgram) {
        // Load and scale background image before each menu display
        IMAGE backgroundImage; // Declare background image
        loadimage(&backgroundImage, _T(".\\Resource\\Greed-Snake-BG.png")); // Load background image

        // Scale background to fit window
        IMAGE scaledBG; // Declare scaled background image
        scaledBG.Resize(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT); // Resize background
        StretchBlt(GetImageHDC(&scaledBG), 0, 0, GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT,
            GetImageHDC(&backgroundImage), 0, 0, backgroundImage.getwidth(), backgroundImage.getheight(), SRCCOPY); // Stretch the background image

        // Apply the background image
        BeginBatchDraw(); // Start batch drawing
        putimage(0, 0, &scaledBG); // Draw the scaled background
        EndBatchDraw(); // End batch drawing

        // Menu loop
        bool showMenu = true; // Flag to control the menu loop
        bool startGame = false; // Flag to control game start
        
        while (showMenu && !quitProgram) {
            // Use ShowGameMenu function from UI.h to display menu
            int menuChoice = ShowGameMenu(); // Get the user's menu choice
            
            switch (menuChoice) {
                case StartGame:
                    cleardevice(); // Clear the device
                    startGame = true; // Set flag to start game
                    showMenu = false; // Exit menu loop
                    break;
                    
                case Setting:
                    ShowSettings(windowWidth, windowHeight); // Show settings menu
                    // Redraw background after settings
                    putimage(0, 0, &scaledBG); // Redraw the background
                    break;
                    
                case About:
                    ShowAbout(); // Show about dialog
                    // Redraw background after about screen
                    putimage(0, 0, &scaledBG); // Redraw the background
                    break;
                    
                case Exit:
                    quitProgram = true; // Set flag to exit program
                    showMenu = false; // Exit menu loop
                    break;
                    
                default:
                    break; // Do nothing for unrecognized choice
            }
        }

        // Game main loop
        while (startGame && !quitProgram) {
            GameState::Instance().Initial(); // Initialize game state
            InitGlobal(); // Initialize global variables

            PlayBackgroundMusic(); // Play background music

            // Initialize player snake
            Vector2 centerPos(windowWidth / 2, (windowHeight / 2)); // Calculate center position
            snake[0].position = centerPos; // Set player snake position
            snake[0].direction = Vector2(1, 0); // Set player snake direction
            snake[0].radius = GameConfig::INITIAL_SNAKE_SIZE; // Set player snake radius
            snake[0].color = HSLtoRGB(255, 255, 255); // Set player snake color

            // Initialize snake body segments
            snake[0].segments.resize(4); // Resize segments vector
            for (int i = 0; i < snake[0].segments.size(); i++) {
                snake[0].segments[i].position = centerPos - Vector2(1, 0) * ((i+1) * GameConfig::SNAKE_SEGMENT_SPACING); // Set segment position
                snake[0].segments[i].direction = Vector2(1, 0); // Set segment direction
                snake[0].segments[i].radius = GameConfig::INITIAL_SNAKE_SIZE; // Set segment radius
                snake[0].segments[i].color = HSLtoRGB(255, 255, 255); // Set segment color
            }

            // Initialize food positions
            for (int i = 0; i < GameConfig::MAX_FOOD_COUNT; ++i) {
                InitFood(foodList, i, GameState::Instance().currentPlayerSpeed); // Initialize each food item
            }

            // Initialize AI snakes
            InitializeAISnakes(); // Set up AI snakes

            // 添加线程异常处理
            std::exception_ptr drawThreadException = nullptr; // Pointer for draw thread exceptions
            std::exception_ptr inputThreadException = nullptr; // Pointer for input thread exceptions
            
            // 使用异步函数调用，可以捕获异常
            std::thread draw([&drawThreadException]() {
                try {
                    Draw(); // Call draw function
                } catch(...) {
                    drawThreadException = std::current_exception(); // Capture exception
                }
            });
            
            std::thread input([&inputThreadException]() {
                try {
                    EnterChanges(); // Call input handling function
                } catch(...) {
                    inputThreadException = std::current_exception(); // Capture exception
                }
            });

            // Game running loop
            bool gameRunning = true; // Flag to control game running state
            while (gameRunning) {
                // 检查线程是否发生异常
                if (drawThreadException) {
                    try {
                        std::rethrow_exception(drawThreadException); // Rethrow exception
                    } catch (const std::exception& e) {
                        MessageBox(GetHWnd(), _T("Drawing thread error occurred"), _T("Error"), MB_OK | MB_ICONERROR); // Show error message
                        OutputDebugStringA(e.what()); // Output error message
                    }
                    GameState::Instance().SetIsGameRunning(false); // Set game running state to false
                    gameRunning = false; // Exit game loop
                    break;
                }
                
                if (inputThreadException) {
                    try {
                        std::rethrow_exception(inputThreadException); // Rethrow exception
                    } catch (const std::exception& e) {
                        MessageBox(GetHWnd(), _T("Input thread error occurred"), _T("Error"), MB_OK | MB_ICONERROR); // Show error message
                        OutputDebugStringA(e.what()); // Output error message
                    }
                    GameState::Instance().SetIsGameRunning(false); // Set game running state to false
                    gameRunning = false; // Exit game loop
                    break;
                }
                
                // 检查是否通过ESC菜单选择退出游戏
                bool shouldExit; // Flag to check if exit is requested
                {
                    std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex); // Lock the game state mutex
                    shouldExit = GameState::Instance().exitGame; // Check exit flag
                }
                
                if (shouldExit) {
                    quitProgram = true; // Set flag to exit program
                    gameRunning = false; // Exit game loop
                    GameState::Instance().SetIsGameRunning(false); // Set game running state to false
                    break;
                }
                
                // 安全地获取游戏状态
                bool isGameRunning, isPaused, showDeathMessage; // Flags for game state
                {
                    std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex); // Lock the game state mutex
                    isGameRunning = GameState::Instance().isGameRunning; // Get game running state
                    isPaused = GameState::Instance().isPaused; // Get pause state
                    showDeathMessage = GameState::Instance().showDeathMessage; // Get death message state
                }
                
                // Update game time - only happens when not paused
                if (isGameRunning && !isPaused) {
                    GameState::Instance().UpdateGameTime(GameState::Instance().deltaTime); // Update game time
                }
                
                // Handle game end conditions
                if (!isGameRunning && showDeathMessage) {
                    // Stop drawing and input threads to prevent interference
                    GameState::Instance().SetIsGameRunning(false); // Set game running state to false
                    
                    // 安全地等待线程结束
                    if (input.joinable()) input.join(); // Join input thread
                    if (draw.joinable()) draw.join(); // Join draw thread
                    
                    cleardevice(); // Clear the device
                    
                    // Show death message after game ends, ensure in main thread
                    GameState::Instance().ShowDeathMessage(); // Show death message
                    {
                        std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex); // Lock the game state mutex
                        GameState::Instance().showDeathMessage = false; // Reset death message state
                    }
                    
                    // Set flag to exit game loop
                    gameRunning = false; // Exit game loop
                }
                
                // 重新安全地获取游戏状态
                {
                    std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex); // Lock the game state mutex
                    isGameRunning = GameState::Instance().isGameRunning; // Get game running state
                    showDeathMessage = GameState::Instance().showDeathMessage; // Get death message state
                }
                
                // Continue game if still running without death message
                if (!isGameRunning && !showDeathMessage) {
                    gameRunning = false;  // Exit game loop if game ended and death message processed
                }
                
                Sleep(10);  // Reduce CPU usage
            }

            // Clean up game resources
            {
                std::lock_guard<std::mutex> lock(GameState::Instance().stateMutex); // Lock the game state mutex
                if (GameState::Instance().isGameRunning) {
                    GameState::Instance().isGameRunning = false;  // Ensure drawing thread exits
                }
            }
            
            // 安全地等待线程结束
            if (input.joinable()) input.join(); // Join input thread
            if (draw.joinable()) draw.join(); // Join draw thread
            
            Sleep(500);  // Short delay to ensure resource cleanup
            
            // Determine what to do next
            if (GameState::Instance().returnToMenu) {
                GameState::Instance().returnToMenu = false; // Reset return to menu flag
                startGame = false;  // Exit game loop and return to menu loop
            } else if (GameState::Instance().exitGame) {
                quitProgram = true;  // If exit is selected, set flag to exit the program
            } else {
                // Keep startGame true to restart game
            }
        }
    }

    // Clean up audio resources before exiting
    CleanupAudioResources(); // Clean up audio resources
    closegraph(); // Close graphics window
    return 0; // Exit program
}

