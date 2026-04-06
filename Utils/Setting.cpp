#include "Setting.h"
#pragma comment(lib, "winmm.lib")

namespace {
int GetSnakeSpeedIndex(float speed) {
    if (speed <= (GameConfig::PLAYER_SLOW_SPEED + GameConfig::PLAYER_NORMAL_SPEED) * 0.5f) {
        return 0;
    }

    if (speed >= (GameConfig::PLAYER_NORMAL_SPEED + GameConfig::PLAYER_FAST_SPEED) * 0.5f) {
        return 2;
    }

    return 1;
}

GameSettings CaptureCurrentSettings() {
    const auto& gameState = GameState::Instance();

    return {
        GameConfig::DEFAULT_VOLUME,
        gameState.difficulty,
        GameConfig::SOUND_ON,
        GetSnakeSpeedIndex(gameState.currentPlayerSpeed),
        GameConfig::ANIMATIONS_ON
    };
}
}

// Set volume
void SetVolume(float volume) {
    // Ensure volume is between 0 and 1
    volume = max(0.0f, min(1.0f, volume));
    
    // Convert float volume to DWORD value (0-0xFFFF)
    DWORD dwVolume = static_cast<DWORD>(volume * 0xFFFF);
    
    // Set left and right channel volumes to the same value
    DWORD stereoVolume = (dwVolume << 16) | dwVolume;
    
    // Set volume
    waveOutSetVolume(0, stereoVolume);
}

// Apply settings
void ApplySettings(const GameSettings& settings) {
    // Apply various settings
    SetVolume(settings.volume);
    
    // Set difficulty level
    GameState::Instance().difficulty = settings.difficulty;
    
    // Adjust AI snake count and aggression based on difficulty
    switch (settings.difficulty) {
    case 0: // Easy
        GameState::Instance().aiSnakeCount = GameConfig::Difficulty::Easy::AI_SNAKE_COUNT;
        GameState::Instance().aiAggression = GameConfig::Difficulty::Easy::AI_AGGRESSION;
        break;
    case 1: // Normal
        GameState::Instance().aiSnakeCount = GameConfig::Difficulty::Normal::AI_SNAKE_COUNT;
        GameState::Instance().aiAggression = GameConfig::Difficulty::Normal::AI_AGGRESSION;
        break;
    case 2: // Hard
        GameState::Instance().aiSnakeCount = GameConfig::Difficulty::Hard::AI_SNAKE_COUNT;
        GameState::Instance().aiAggression = GameConfig::Difficulty::Hard::AI_AGGRESSION;
        break;
    }
    
    // Apply sound toggle
    GameConfig::SOUND_ON = settings.soundOn;
    
    // Apply animations toggle
    GameConfig::ANIMATIONS_ON = settings.animationsOn;
    
    // Set snake speed
    switch (settings.snakeSpeed) {
    case 0: // Slow
        GameState::Instance().currentPlayerSpeed = GameConfig::PLAYER_SLOW_SPEED;
        break;
    case 1: // Medium
        GameState::Instance().currentPlayerSpeed = GameConfig::PLAYER_NORMAL_SPEED;
        break;
    case 2: // Fast
        GameState::Instance().currentPlayerSpeed = GameConfig::PLAYER_FAST_SPEED;
        break;
    }
}

// Settings dialog
void ShowSettings(int windowWidth, int windowHeight) {
    // Create game settings object and initialize with current settings
    GameSettings currentSettings = CaptureCurrentSettings();
    const GameSettings originalSettings = currentSettings;
    
    // Create settings interface
    setfillcolor(RGB(30, 30, 30));
    solidrectangle(0, 0, windowWidth, windowHeight);
    
    settextstyle(36, 0, _T("Arial"));
    settextcolor(RGB(255, 255, 255));
    outtextxy(windowWidth / 2 - textwidth(_T("Settings")) / 2, 30, _T("Settings"));
    
    settextstyle(24, 0, _T("Arial"));
    
    // Volume slider
    int sliderX = 150;
    int sliderY = 120;
    int sliderWidth = windowWidth - 300;
    // SECURITY FIX: Ensure sliderWidth is positive to prevent division by zero
    if (sliderWidth <= 0) sliderWidth = 100;
    int sliderHeight = 10;
    
    outtextxy(sliderX, sliderY - 40, _T("Volume"));

    // Draw volume slider background
    setfillcolor(RGB(100, 100, 100));
    solidroundrect(sliderX, sliderY, sliderX + sliderWidth, sliderY + sliderHeight, 5, 5);
    
    // Draw volume slider current position
    int handleX = sliderX + static_cast<int>(currentSettings.volume * sliderWidth);
    int handleRadius = 15;
    
    setfillcolor(RGB(0, 150, 255));
    solidcircle(handleX, sliderY + sliderHeight / 2, handleRadius);

    // Difficulty selection
    int difficultyY = sliderY + 100;
    outtextxy(sliderX, difficultyY - 40, _T("Difficulty"));
    
    const TCHAR* difficultyLabels[] = { _T("Easy"), _T("Normal"), _T("Hard") };
    int buttonWidth = (sliderWidth) / 3;
    int buttonHeight = 40;
    
    for (int i = 0; i < 3; i++) {
        // Calculate button position
        int btnX = sliderX + i * buttonWidth;
        
        // Set color (selected button is highlighted)
        if (i == currentSettings.difficulty) {
            setfillcolor(RGB(0, 150, 255));
        } else {
            setfillcolor(RGB(80, 80, 80));
        }
        
        // Draw button
        solidroundrect(btnX, difficultyY, btnX + buttonWidth - 10, difficultyY + buttonHeight, 5, 5);
        
        // Draw text
        int textWidth = textwidth(difficultyLabels[i]);
        outtextxy(btnX + (buttonWidth - 10) / 2 - textWidth / 2, difficultyY + 8, difficultyLabels[i]);
    }
    
    // Sound toggle
    int soundY = difficultyY + 100;
    outtextxy(sliderX, soundY - 40, _T("Sound"));
    
    const TCHAR* soundLabels[] = { _T("Off"), _T("On") };
    int soundBtnWidth = (sliderWidth) / 2;
    
    for (int i = 0; i < 2; i++) {
        int btnX = sliderX + i * soundBtnWidth;
        
        if ((i == 0 && !currentSettings.soundOn) || (i == 1 && currentSettings.soundOn)) {
            setfillcolor(RGB(0, 150, 255));
        } else {
            setfillcolor(RGB(80, 80, 80));
        }
        
        solidroundrect(btnX, soundY, btnX + soundBtnWidth - 10, soundY + buttonHeight, 5, 5);
        
        int textWidth = textwidth(soundLabels[i]);
        outtextxy(btnX + (soundBtnWidth - 10) / 2 - textWidth / 2, soundY + 8, soundLabels[i]);
    }
    
    // Snake speed selection
    int speedY = soundY + 100;
    outtextxy(sliderX, speedY - 40, _T("Snake Speed"));
    
    const TCHAR* speedLabels[] = { _T("Slow"), _T("Normal"), _T("Fast") };
    
    for (int i = 0; i < 3; i++) {
        int btnX = sliderX + i * buttonWidth;
        
        if (i == currentSettings.snakeSpeed) {
            setfillcolor(RGB(0, 150, 255));
        } else {
            setfillcolor(RGB(80, 80, 80));
        }
        
        solidroundrect(btnX, speedY, btnX + buttonWidth - 10, speedY + buttonHeight, 5, 5);
        
        int textWidth = textwidth(speedLabels[i]);
        outtextxy(btnX + (buttonWidth - 10) / 2 - textWidth / 2, speedY + 8, speedLabels[i]);
    }
    
    // Animation toggle
    int animationY = speedY + 100;
    outtextxy(sliderX, animationY - 40, _T("Animations"));
    
    const TCHAR* animationLabels[] = { _T("Off"), _T("On") };
    
    for (int i = 0; i < 2; i++) {
        int btnX = sliderX + i * soundBtnWidth;
        
        if ((i == 0 && !currentSettings.animationsOn) || (i == 1 && currentSettings.animationsOn)) {
            setfillcolor(RGB(0, 150, 255));
        } else {
            setfillcolor(RGB(80, 80, 80));
        }
        
        solidroundrect(btnX, animationY, btnX + soundBtnWidth - 10, animationY + buttonHeight, 5, 5);
        
        int textWidth = textwidth(animationLabels[i]);
        outtextxy(btnX + (soundBtnWidth - 10) / 2 - textWidth / 2, animationY + 8, animationLabels[i]);
    }
    
    // Return button area
    int returnY = windowHeight - 100;
    int settingButtonWidth = 150;
    int settingButtonHeight = 50;
    int applyX = windowWidth / 2 - settingButtonWidth - 20;
    int cancelX = windowWidth / 2 + 20;
    
    // Apply button
    setfillcolor(RGB(0, 150, 80));
    solidroundrect(applyX, returnY, applyX + settingButtonWidth, returnY + settingButtonHeight, 10, 10);

    settextstyle(28, 0, _T("Arial"));
    outtextxy(applyX + settingButtonWidth / 2 - textwidth(_T("Apply")) / 2, returnY + 10, _T("Apply"));

    // Cancel button
    setfillcolor(RGB(150, 50, 50));
    solidroundrect(cancelX, returnY, cancelX + settingButtonWidth, returnY + settingButtonHeight, 10, 10);

    outtextxy(cancelX + settingButtonWidth / 2 - textwidth(_T("Cancel")) / 2, returnY + 10, _T("Cancel"));
    
    FlushBatchDraw();
    
    // Handle user input
    bool settingsOpen = true;
    while (settingsOpen) {
        ExMessage msg = getmessage(EX_MOUSE);
        if (msg.message == WM_LBUTTONDOWN) {
            // Check if on volume slider
            if (msg.y >= sliderY - handleRadius && msg.y <= sliderY + sliderHeight + handleRadius &&
                msg.x >= sliderX - handleRadius && msg.x <= sliderX + sliderWidth + handleRadius) {
                // Update volume
                float newVolume = static_cast<float>(msg.x - sliderX) / sliderWidth;
                newVolume = max(0.0f, min(1.0f, newVolume));
                currentSettings.volume = newVolume;
                
                // Apply volume in real-time
                SetVolume(newVolume);
                
                // Clear the volume area first
                setfillcolor(RGB(30, 30, 30));
                solidrectangle(sliderX-handleRadius, sliderY-handleRadius, sliderX+sliderWidth+handleRadius, sliderY+sliderHeight+handleRadius);
                
                // Redraw volume slider
                setfillcolor(RGB(100, 100, 100));
                solidroundrect(sliderX, sliderY, sliderX + sliderWidth, sliderY + sliderHeight, 5, 5);
                
                handleX = sliderX + static_cast<int>(currentSettings.volume * sliderWidth);
                setfillcolor(RGB(0, 150, 255));
                solidcircle(handleX, sliderY + sliderHeight / 2, handleRadius);
                
                FlushBatchDraw();
            }
            // Check difficulty buttons
            else if (msg.y >= difficultyY && msg.y <= difficultyY + buttonHeight) {
                for (int i = 0; i < 3; i++) {
                    int btnX = sliderX + i * buttonWidth;
                    if (msg.x >= btnX && msg.x <= btnX + buttonWidth - 10) {
                        currentSettings.difficulty = i;
                        
                        // Redraw difficulty buttons
                        for (int j = 0; j < 3; j++) {
                            int btnX2 = sliderX + j * buttonWidth;
                            if (j == currentSettings.difficulty) {
                                setfillcolor(RGB(0, 150, 255));
                            } else {
                                setfillcolor(RGB(80, 80, 80));
                            }
                            solidroundrect(btnX2, difficultyY, btnX2 + buttonWidth - 10, difficultyY + buttonHeight, 5, 5);
                            
                            int textWidth = textwidth(difficultyLabels[j]);
                            outtextxy(btnX2 + (buttonWidth - 10) / 2 - textWidth / 2, difficultyY + 8, difficultyLabels[j]);
                        }
                        
                        FlushBatchDraw();
                    break;
                    }
                }
            }
            // Check sound toggle buttons
            else if (msg.y >= soundY && msg.y <= soundY + buttonHeight) {
                for (int i = 0; i < 2; i++) {
                    int btnX = sliderX + i * soundBtnWidth;
                    if (msg.x >= btnX && msg.x <= btnX + soundBtnWidth - 10) {
                        currentSettings.soundOn = (i == 1);
                        
                        // Redraw sound buttons
                        for (int j = 0; j < 2; j++) {
                            int btnX2 = sliderX + j * soundBtnWidth;
                            if ((j == 0 && !currentSettings.soundOn) || (j == 1 && currentSettings.soundOn)) {
                                setfillcolor(RGB(0, 150, 255));
                            } else {
                                setfillcolor(RGB(80, 80, 80));
                            }
                            
                            solidroundrect(btnX2, soundY, btnX2 + soundBtnWidth - 10, soundY + buttonHeight, 5, 5);
                            
                            int textWidth = textwidth(soundLabels[j]);
                            outtextxy(btnX2 + (soundBtnWidth - 10) / 2 - textWidth / 2, soundY + 8, soundLabels[j]);
                        }
                        
                        FlushBatchDraw();
                        break;
                    }
                }
            }
            // Check speed buttons
            else if (msg.y >= speedY && msg.y <= speedY + buttonHeight) {
                for (int i = 0; i < 3; i++) {
                    int btnX = sliderX + i * buttonWidth;
                    if (msg.x >= btnX && msg.x <= btnX + buttonWidth - 10) {
                        currentSettings.snakeSpeed = i;
                        
                        // Redraw speed buttons
                        for (int j = 0; j < 3; j++) {
                            int btnX2 = sliderX + j * buttonWidth;
                            if (j == currentSettings.snakeSpeed) {
                                setfillcolor(RGB(0, 150, 255));
                            } else {
                                setfillcolor(RGB(80, 80, 80));
                            }
                            solidroundrect(btnX2, speedY, btnX2 + buttonWidth - 10, speedY + buttonHeight, 5, 5);
                            
                            int textWidth = textwidth(speedLabels[j]);
                            outtextxy(btnX2 + (buttonWidth - 10) / 2 - textWidth / 2, speedY + 8, speedLabels[j]);
                        }
                        
                        FlushBatchDraw();
                        break;
                    }
                }
            }
            // Check animation toggle buttons
            else if (msg.y >= animationY && msg.y <= animationY + buttonHeight) {
                for (int i = 0; i < 2; i++) {
                    int btnX = sliderX + i * soundBtnWidth;
                    if (msg.x >= btnX && msg.x <= btnX + soundBtnWidth - 10) {
                        currentSettings.animationsOn = (i == 1);
                        
                        // Redraw animation buttons
                        for (int j = 0; j < 2; j++) {
                            int btnX2 = sliderX + j * soundBtnWidth;
                            if ((j == 0 && !currentSettings.animationsOn) || (j == 1 && currentSettings.animationsOn)) {
                                setfillcolor(RGB(0, 150, 255));
                            } else {
                                setfillcolor(RGB(80, 80, 80));
                            }
                            
                            solidroundrect(btnX2, animationY, btnX2 + soundBtnWidth - 10, animationY + buttonHeight, 5, 5);
                            
                            int textWidth = textwidth(animationLabels[j]);
                            outtextxy(btnX2 + (soundBtnWidth - 10) / 2 - textWidth / 2, animationY + 8, animationLabels[j]);
                        }
                        
                        FlushBatchDraw();
                        break;
                    }
                }
            }
            // Check apply button
            else if (msg.x >= applyX && msg.x <= applyX + settingButtonWidth &&
                     msg.y >= returnY && msg.y <= returnY + settingButtonHeight) {
                // Apply settings
                ApplySettings(currentSettings);
                settingsOpen = false;
            }
            // Check cancel button
            else if (msg.x >= cancelX && msg.x <= cancelX + settingButtonWidth &&
                     msg.y >= returnY && msg.y <= returnY + settingButtonHeight) {
                ApplySettings(originalSettings);
                settingsOpen = false;
            }
        }
    }
}
