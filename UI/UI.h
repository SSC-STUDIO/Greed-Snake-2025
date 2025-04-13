#pragma once
#include "../Core/GameConfig.h"
#include "Button.h"
#include <graphics.h>
#include <windows.h>

// Button list
extern std::vector<Button> buttonList;

// Menu option enumeration
enum class MenuOption {
    START,
    SETTINGS,
    ABOUT,
    EXIT
};

// Load button resources for the menu
void LoadButton();

// Draw the main menu interface
void DrawMenu();

// Display "About" information screen
void ShowAbout();

// Display game main menu, returns user's menu selection
int ShowGameMenu();

// Sound & Animation Functions
// Play background music
void PlayBackgroundMusic();

// Stop background music
void StopBackgroundMusic();

// Play start animation music and animation
void PlayStartAnimation();

// Cleanup audio resources before exiting
void CleanupAudioResources();