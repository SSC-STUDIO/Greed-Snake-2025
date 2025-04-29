#include "Rendering.h"
#include <cmath> // For using sin function
#pragma warning(disable: 4996)	 // Disable security warnings for _tcscpy and _stprintf

// Add extern declaration for the animation timer defined in Main.cpp
extern float animationTimer; // Animation timer for visual effects

// Function to draw the game area
void DrawGameArea() {
    // Update animation timer - increment by a small amount each frame
    animationTimer += 0.05f; // Increment the animation timer
    
    // Draw background
    setbkcolor(RGB(30, 30, 30)); // Set background color
    cleardevice(); // Clear the device

    // Get camera position
    Vector2 cameraPos = GameState::Instance().camera.position; // Get the current camera position

    // Calculate visible area
    float screenLeft = cameraPos.x; // Left boundary of the visible area
    float screenRight = cameraPos.x + GameConfig::WINDOW_WIDTH; // Right boundary of the visible area
    float screenTop = cameraPos.y; // Top boundary of the visible area
    float screenBottom = cameraPos.y + GameConfig::WINDOW_HEIGHT; // Bottom boundary of the visible area

    // Draw lava area outside game boundaries - dark red color
    // First check which boundaries are in view
    setfillcolor(RGB(80, 20, 20)); // Dark red

    // Check and draw top boundary lava area
    if (screenTop < GameConfig::PLAY_AREA_TOP) {
        solidrectangle(0, 0,
            GameConfig::WINDOW_WIDTH,
            GameConfig::PLAY_AREA_TOP - cameraPos.y); // Draw lava area above the play area
    }

    // Check and draw bottom boundary lava area
    if (screenBottom > GameConfig::PLAY_AREA_BOTTOM) {
        solidrectangle(0,
            GameConfig::PLAY_AREA_BOTTOM - cameraPos.y,
            GameConfig::WINDOW_WIDTH,
            GameConfig::WINDOW_HEIGHT); // Draw lava area below the play area
    }

    // Check and draw left boundary lava area
    if (screenLeft < GameConfig::PLAY_AREA_LEFT) {
        solidrectangle(0, 0,
            GameConfig::PLAY_AREA_LEFT - cameraPos.x,
            GameConfig::WINDOW_HEIGHT); // Draw lava area to the left of the play area
    }

    // Check and draw right boundary lava area
    if (screenRight > GameConfig::PLAY_AREA_RIGHT) {
        solidrectangle(GameConfig::PLAY_AREA_RIGHT - cameraPos.x,
            0,
            GameConfig::WINDOW_WIDTH,
            GameConfig::WINDOW_HEIGHT); // Draw lava area to the right of the play area
    }

    // Draw game boundary lines
    setlinecolor(RGB(150, 50, 50)); // Boundary line color
    Vector2 topLeft(GameConfig::PLAY_AREA_LEFT, GameConfig::PLAY_AREA_TOP); // Top left corner of the play area
    Vector2 bottomRight(GameConfig::PLAY_AREA_RIGHT, GameConfig::PLAY_AREA_BOTTOM); // Bottom right corner of the play area

    rectangle(topLeft.x - cameraPos.x, topLeft.y - cameraPos.y,
        bottomRight.x - cameraPos.x, bottomRight.y - cameraPos.y); // Draw the boundary rectangle
}

// Function to draw food items
void DrawFoods(const FoodItem* foodList, int foodCount) {
    for (int i = 0; i < foodCount; ++i) {
        if (foodList[i].collisionRadius > 0) { // Check if the food item is active
            Vector2 screenPos = foodList[i].position - GameState::Instance().camera.position; // Calculate screen position
            
            // Use the enhanced food drawing function for consistent appearance
            if (GameConfig::ANIMATIONS_ON) {
                DrawEnhancedFood(screenPos, foodList[i].collisionRadius, foodList[i].colorValue, i); // Draw food with animations
            } else {
                // Simple drawing for when animations are off
                DrawCircleWithCamera(screenPos, foodList[i].collisionRadius, foodList[i].colorValue); // Draw food without animations
            }
        }
    }
}

// Function to draw visible objects in the game
void DrawVisibleObjects(const FoodItem* foodList, int foodCount, 
                        const AISnake* aiSnakes, int aiSnakeCount,
                        const PlayerSnake& playerSnake) {
    // Calculate visible area
    Vector2 cameraPos = GameState::Instance().camera.position; // Get camera position
    float screenLeft = cameraPos.x; // Left boundary of the visible area
    float screenRight = cameraPos.x + GameConfig::WINDOW_WIDTH; // Right boundary of the visible area
    float screenTop = cameraPos.y; // Top boundary of the visible area
    float screenBottom = cameraPos.y + GameConfig::WINDOW_HEIGHT; // Bottom boundary of the visible area

    // Expand visible area to consider partially visible large objects
    float margin = 100.0f; // Margin for visibility
    screenLeft -= margin; // Expand left boundary
    screenRight += margin; // Expand right boundary
    screenTop -= margin; // Expand top boundary
    screenBottom += margin; // Expand bottom boundary

    // Only draw food within the visible area
    for (int i = 0; i < foodCount; ++i) {
        const auto& food = foodList[i]; // Get food item
        if (food.collisionRadius <= 0) continue; // Skip if food is not active
        
        if (food.position.x >= screenLeft && food.position.x <= screenRight &&
            food.position.y >= screenTop && food.position.y <= screenBottom) {
            
            Vector2 foodScreenPos = food.position - cameraPos; // Calculate food screen position
            
            // Use enhanced food drawing function
            if (GameConfig::ANIMATIONS_ON) {
                DrawEnhancedFood(foodScreenPos, food.collisionRadius, food.colorValue, i); // Draw food with animations
            } else {
                DrawCircleWithCamera(foodScreenPos, food.collisionRadius, food.colorValue); // Draw food without animations
            }
        }
    }

    // Draw AI snakes
    for (int i = 0; i < aiSnakeCount; ++i) {
        const auto& snake = aiSnakes[i]; // Get AI snake
        if (snake.radius <= 0) continue; // Skip removed snakes
        
        // Draw AI snake body - from tail to head
        for (size_t j = 0; j < snake.segments.size(); ++j) {
            const auto& segment = snake.segments[j]; // Get snake segment
            if (segment.position.x >= screenLeft && segment.position.x <= screenRight &&
                segment.position.y >= screenTop && segment.position.y <= screenBottom) {
                Vector2 segmentPos = segment.position - cameraPos; // Calculate segment screen position
                DrawSnakeSegment(segmentPos, segment.radius, segment.color, j); // Draw each segment
            }
        }
        
        // Draw AI snake head
        if (snake.position.x >= screenLeft && snake.position.x <= screenRight &&
            snake.position.y >= screenTop && snake.position.y <= screenBottom) {
            Vector2 windowPos = snake.position - cameraPos; // Calculate head screen position
            DrawCircleWithCamera(windowPos, snake.radius, snake.color); // Draw head
            DrawSnakeEyes(windowPos, snake.direction, snake.radius); // Draw eyes
        }
    }

    // Draw player snake's body - first draw body, then head
    for (size_t i = 0; i < playerSnake.segments.size(); ++i) {
        const auto& segment = playerSnake.segments[i]; // Get player snake segment
        if (segment.position.x >= screenLeft && segment.position.x <= screenRight &&
            segment.position.y >= screenTop && segment.position.y <= screenBottom) {
            Vector2 segmentPos = segment.position - cameraPos; // Calculate segment screen position
            DrawSnakeSegment(segmentPos, segment.radius, segment.color, i); // Draw each segment
        }
    }
    
    // Draw player snake head
    if (playerSnake.position.x >= screenLeft && playerSnake.position.x <= screenRight &&
        playerSnake.position.y >= screenTop && playerSnake.position.y <= screenBottom) {
        Vector2 headPos = playerSnake.position - cameraPos; // Calculate head screen position
        DrawCircleWithCamera(headPos, playerSnake.radius, playerSnake.color); // Draw head
        DrawSnakeEyes(headPos, playerSnake.direction, playerSnake.radius); // Draw eyes
    }
}

void DrawCircleWithCamera(const Vector2& screenPos, float r, int c) {
    if (!IsCircleInScreen(screenPos, r)) {
        return; // Not in screen, skip drawing
    }
    setlinecolor(c); // Set circle outline color
    setfillcolor(c); // Set circle fill color
    fillcircle(screenPos.x, screenPos.y, r); // Draw circle
}

void DebugDrawText(const std::wstring& text, int x, int y, int color) {
    settextcolor(color); // Set text color
    outtextxy(x, y, text.c_str()); // Draw text
}

// 添加蛇身体段渲染函数
void DrawSnakeSegment(const Vector2& screenPos, float radius, int color, int segmentIndex) {
    // 提取颜色分量
    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = color & 0xFF;
    
    // 为不同的身体段添加颜色变化
    // 根据段索引逐渐改变颜色亮度
    float brightnessFactor = 1.0f - (segmentIndex * 0.03f);
    brightnessFactor = max(0.6f, brightnessFactor);  // 不要太暗
    
    int segR = static_cast<int>(r * brightnessFactor);
    int segG = static_cast<int>(g * brightnessFactor);
    int segB = static_cast<int>(b * brightnessFactor);
    
    // 基于动画计时器添加轻微的大小变化
    float pulseFactor = 1.0f + sin(animationTimer * 2 + segmentIndex * 0.3f) * 0.05f;
    float pulseRadius = radius * pulseFactor;
    
    // 添加光晕效果
    if (GameConfig::ANIMATIONS_ON) {
        // 外层光晕
        setfillcolor(RGB(segR/3, segG/3, segB/3));
        fillcircle(screenPos.x, screenPos.y, pulseRadius * 1.2f);
    }
    
    // 主体色
    setfillcolor(RGB(segR, segG, segB));
    fillcircle(screenPos.x, screenPos.y, pulseRadius);
    
    // 添加高光，增加立体感
    if (GameConfig::ANIMATIONS_ON) {
        setfillcolor(RGB(min(255, segR+40), min(255, segG+40), min(255, segB+40)));
        fillcircle(screenPos.x - pulseRadius * 0.3f, screenPos.y - pulseRadius * 0.3f, pulseRadius * 0.2f);
    }
}

void DrawSnakeEyes(const Vector2& position, const Vector2& direction, float radius) {
    // 确保总是先绘制眼白
    float eyeRadius = radius * 0.3f;  // 眼睛大小

    // 标准化方向计算
    Vector2 normalizedDir = direction.GetNormalize();
    // 计算垂直方向用于眼睛分布
    Vector2 perpDir(-normalizedDir.y, normalizedDir.x);

    // 计算眼睛位置
    Vector2 eyeOffset = normalizedDir * (radius * 0.25f);
    Vector2 sideOffset = perpDir * (radius * 0.4f);

    // 实际眼睛位置
    Vector2 leftEyePos = position + eyeOffset + sideOffset;
    Vector2 rightEyePos = position + eyeOffset - sideOffset;

    // 绘制眼白 - 先绝对确保这一步
    setfillcolor(RGB(255, 255, 255));
    fillcircle(leftEyePos.x, leftEyePos.y, eyeRadius);
    fillcircle(rightEyePos.x, rightEyePos.y, eyeRadius);

    // 瞳孔
    float pupilRadius = eyeRadius * 0.65f;  // 稍微小一点
    setfillcolor(RGB(0, 0, 0));

    // 瞳孔偏移系数
    float pupilOffsetFactor = 1.3f;  

    // 计算瞳孔位置，确保不超出眼白
    float maxPupilOffset = eyeRadius - pupilRadius;
    Vector2 pupilOffset = normalizedDir * (maxPupilOffset * pupilOffsetFactor);

    // 确保瞳孔不超出眼白范围
    float pupilOffsetLength = pupilOffset.GetLength();
    if (pupilOffsetLength > maxPupilOffset) {
        pupilOffset = pupilOffset * (maxPupilOffset / pupilOffsetLength);
    }

    // 绘制瞳孔
    fillcircle(leftEyePos.x + pupilOffset.x, leftEyePos.y + pupilOffset.y, pupilRadius);
    fillcircle(rightEyePos.x + pupilOffset.x, rightEyePos.y + pupilOffset.y, pupilRadius);

    // 添加眼睛高光增加深度感
    setfillcolor(RGB(255, 255, 255));
    float highlightRadius = pupilRadius * 0.3f;
    Vector2 highlightOffset = normalizedDir * (-pupilRadius * 0.3f);

    fillcircle(leftEyePos.x + pupilOffset.x + highlightOffset.x,
        leftEyePos.y + pupilOffset.y + highlightOffset.y,
        highlightRadius);
    fillcircle(rightEyePos.x + pupilOffset.x + highlightOffset.x,
        rightEyePos.y + pupilOffset.y + highlightOffset.y,
        highlightRadius);
}

bool IsCircleInScreen(const Vector2& center, float r) {
    Vector2 minPoint = Vector2(center.x - r, center.y - r); // Min corner
    Vector2 maxPoint = Vector2(center.x + r, center.y + r); // Max corner

    return !(maxPoint.x < 0 || minPoint.x > GameConfig::WINDOW_WIDTH || maxPoint.y < 0 || minPoint.y > GameConfig::WINDOW_HEIGHT); // Check if visible on screen
}

void DrawUI() {
    auto& gameState = GameState::Instance();

    // 使用单个互斥锁获取所有需要的状态变量
    bool isInvulnerable, isInLava;
    float gameStartTime, timeInLava;
    int foodEatenCount;
    
    {
        std::lock_guard<std::mutex> lock(gameState.stateMutex);
        isInvulnerable = gameState.isInvulnerable;
        isInLava = gameState.isInLava;
        gameStartTime = gameState.gameStartTime;
        timeInLava = gameState.timeInLava;
        foodEatenCount = gameState.foodEatenCount;
    }

    // 绘制分数
    settextstyle(24, 0, _T("Arial"));
    settextcolor(WHITE);
    TCHAR scoreText[50];
    _stprintf(scoreText, _T("Score: %d"), foodEatenCount);
    outtextxy(10, 10, scoreText);

    // 显示无敌时间
    if (isInvulnerable) {
        settextcolor(RGB(255, 215, 0));  // 金色表示无敌状态
        TCHAR invulnerableText[50];
        float timeLeft = GameConfig::COLLISION_GRACE_PERIOD - gameStartTime;
        _stprintf(invulnerableText, _T("INVULNERABLE: %.1f"), timeLeft);
        
        // 添加闪烁效果
        if (fmod(animationTimer, 0.5f) > 0.25f) {
            outtextxy(GameConfig::WINDOW_WIDTH / 2 - 100, 40, invulnerableText);
        }
    }

    // 显示岩浆警告
    if (isInLava) {
        settextcolor(RGB(255, 0, 0));
        TCHAR warningText[50];
        float timeLeft = GameConfig::LAVA_WARNING_TIME - timeInLava;
        _stprintf(warningText, _T("WARNING! Return to play area! %.1f"), timeLeft);
        outtextxy(GameConfig::WINDOW_WIDTH / 2 - 150, 10, warningText);
    }
    
    // 绘制暂停菜单
    if (gameState.GetIsPaused() && gameState.GetIsMenuShowing()) {
        DrawPauseMenu();
    }
}

// 将暂停菜单绘制抽取为单独函数，去除重复代码
void DrawPauseMenu() {
    // 绘制半透明覆盖层
    setfillcolor(RGB(0, 0, 0)); // 黑色背景
    setfillstyle(BS_SOLID);
    solidrectangle(0, 0, GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
    
    // 暂停菜单框
    int pauseBoxWidth = 400;
    int pauseBoxHeight = 350; // 增大高度以容纳三个按钮
    int pauseBoxX = (GameConfig::WINDOW_WIDTH - pauseBoxWidth) / 2;
    int pauseBoxY = (GameConfig::WINDOW_HEIGHT - pauseBoxHeight) / 2;
    
    // 绘制暂停菜单框
    setfillcolor(RGB(40, 40, 40));
    solidroundrect(pauseBoxX, pauseBoxY, pauseBoxX + pauseBoxWidth, pauseBoxY + pauseBoxHeight, 15, 15);
    
    // 绘制菜单标题
    settextstyle(32, 0, _T("Arial"));
    settextcolor(WHITE);
    const TCHAR* pauseTitle = _T("GAME PAUSED");
    int titleWidth = textwidth(pauseTitle);
    outtextxy(pauseBoxX + (pauseBoxWidth - titleWidth) / 2, pauseBoxY + 30, pauseTitle);
    
    // 绘制菜单按钮
    int btnWidth = 280;
    int btnHeight = 50;
    int btnX = pauseBoxX + (pauseBoxWidth - btnWidth) / 2;
    int btnY = pauseBoxY + 100;
    int btnSpacing = 20;
    
    // 恢复游戏按钮
    setfillcolor(RGB(60, 120, 60));
    solidroundrect(btnX, btnY, btnX + btnWidth, btnY + btnHeight, 10, 10);
    settextstyle(24, 0, _T("Arial"));
    const TCHAR* resumeText = _T("Resume Game (R)");
    int resumeWidth = textwidth(resumeText);
    outtextxy(btnX + (btnWidth - resumeWidth) / 2, btnY + (btnHeight - textheight(resumeText)) / 2, resumeText);
    
    // 返回主菜单按钮
    btnY += btnHeight + btnSpacing;
    setfillcolor(RGB(80, 80, 150));
    solidroundrect(btnX, btnY, btnX + btnWidth, btnY + btnHeight, 10, 10);
    const TCHAR* menuText = _T("Main Menu (M)");
    int menuWidth = textwidth(menuText);
    outtextxy(btnX + (btnWidth - menuWidth) / 2, btnY + (btnHeight - textheight(menuText)) / 2, menuText);
    
    // 退出游戏按钮
    btnY += btnHeight + btnSpacing;
    setfillcolor(RGB(150, 60, 60));
    solidroundrect(btnX, btnY, btnX + btnWidth, btnY + btnHeight, 10, 10);
    const TCHAR* exitText = _T("Exit Game (Q)");
    int exitWidth = textwidth(exitText);
    outtextxy(btnX + (btnWidth - exitWidth) / 2, btnY + (btnHeight - textheight(exitText)) / 2, exitText);
    
    // 绘制按键提示
    settextstyle(16, 0, _T("Arial"));
    const TCHAR* promptText = _T("Press key or click button to select");
    int promptWidth = textwidth(promptText);
    outtextxy(pauseBoxX + (pauseBoxWidth - promptWidth) / 2, 
            pauseBoxY + pauseBoxHeight - 30,
            promptText);
}

// Add a function for drawing food with enhanced visual effects
void DrawEnhancedFood(const Vector2& screenPos, float radius, int color, int index) {
    // Extract color components
    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = color & 0xFF;
    
    // Calculate animation effects
    float pulse = sin(animationTimer + index * 0.5f) * 0.2f + 1.0f;
    float sparklePhase = (animationTimer * 2 + index) * 3.14159f;
    
    // Create pulsing radius
    float animatedRadius = radius * pulse;
    
    // Outer glow
    setfillcolor(RGB(r/3, g/3, b/3));
    setfillstyle(BS_SOLID, NULL, NULL);
    fillcircle(screenPos.x, screenPos.y, animatedRadius * 2.0f);
    
    // Middle layer
    setfillcolor(RGB(r/2, g/2, b/2));
    fillcircle(screenPos.x, screenPos.y, animatedRadius * 1.5f);
    
    // Inner circle (main food)
    setfillcolor(RGB(r, g, b));
    fillcircle(screenPos.x, screenPos.y, animatedRadius);
    
    // Add highlight (small white circle in upper left)
    setfillcolor(RGB(255, 255, 255));
    fillcircle(screenPos.x - animatedRadius * 0.3f, 
               screenPos.y - animatedRadius * 0.3f, 
               animatedRadius * 0.25f);
    
    // Draw sparkles around the food
    if (GameConfig::ANIMATIONS_ON) {
        int numSparkles = 4;
        for (int i = 0; i < numSparkles; i++) {
            float angle = sparklePhase + i * (2 * 3.14159f / numSparkles);
            float sparkleX = screenPos.x + cos(angle) * animatedRadius * 1.8f;
            float sparkleY = screenPos.y + sin(angle) * animatedRadius * 1.8f;
            
            // Make sparkle size pulse in counterphase to the main food
            float sparkleSize = animatedRadius * 0.2f * (1.2f - 0.2f * sin(animationTimer + index * 0.5f));
            
            // Draw sparkle
            setfillcolor(RGB(255, 255, 200)); // Slightly yellow-ish white
            fillcircle(sparkleX, sparkleY, sparkleSize);
        }
    }
}
