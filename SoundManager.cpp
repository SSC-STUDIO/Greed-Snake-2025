#include "SoundManager.h"
#include "GameConfig.h"
#include "Setting.h"
#include <tchar.h>
#include <mmsystem.h>
#include <graphics.h>
#pragma comment(lib, "winmm.lib")

void PlayBackgroundMusic() {
    // Check if music is already playing
    TCHAR statusBuff[256] = {0};
    if (mciSendString(_T("status Greed-Snake mode"), statusBuff, 256, NULL) == 0) {
        // If already open, close it first
        mciSendString(_T("close Greed-Snake"), NULL, 0, NULL);
    }
    
    // Open and play music
    mciSendString(_T("open ..\\Resource\\Greed-Snake-Start-Animation.mp3 alias Greed-Snake"), NULL, NULL, NULL); // Open music file
    mciSendString(_T("play Greed-Snake repeat"), NULL, NULL, NULL); // Play music
    SetVolume(GameConfig::DEFAULT_VOLUME);  // Set initial volume
}

void StopBackgroundMusic() {
    // Check if music is playing before trying to stop it
    TCHAR statusBuff[256] = {0};
    if (mciSendString(_T("status Greed-Snake mode"), statusBuff, 256, NULL) == 0) {
        mciSendString(_T("stop Greed-Snake"), NULL, NULL, NULL); // Stop music
        mciSendString(_T("close Greed-Snake"), NULL, NULL, NULL); // Close music
    }
}

void PlayStartAnimation() {
    // Define frame file path and file extension
    const TCHAR* path = _T(".\\Resource\\Greed-Snake-Start-Animation-Frames\\"); // Frame file path
    const TCHAR* ext = _T(".bmp"); // File extension

    // Check if animation audio is already playing and close it if needed
    TCHAR statusBuff[256] = {0};
    if (mciSendString(_T("status Start-Animation mode"), statusBuff, 256, NULL) == 0) {
        mciSendString(_T("close Start-Animation"), NULL, 0, NULL);
    }

    // Always play the sound even if animations are off
    mciSendString(_T("open .\\Resource\\Greed-Snake-Start-Animation.mp3 alias Start-Animation"), NULL, NULL, NULL); // Open animation music
    mciSendString(_T("play Start-Animation"), NULL, NULL, NULL); // Play animation music
    
    if (!GameConfig::ANIMATIONS_ON) {
        // If animations are disabled, just wait a moment and return
        Sleep(1000);
        return;
    }
    
    Sleep(6000); // Wait 6 seconds

    // Declare IMAGE structure variable
    IMAGE FImg;    

    // Use loop to load and display each frame
    for (int i = 0; i <= GameConfig::NUM_FRAMES; i++) {
        TCHAR frameFileName[MAX_PATH];  // Ensure buffer is large enough to store full path
        _stprintf_s(frameFileName, MAX_PATH, _T("%sframe_%d%s"), path, i, ext); // Format frame filename

        // Load image and draw it
        loadimage(&FImg, frameFileName);
        IMAGE scaledG;
        scaledG.Resize(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
        StretchBlt(GetImageHDC(&scaledG), 0, 0, GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT,
            GetImageHDC(&FImg), 0, 0, FImg.getwidth(), FImg.getheight(), SRCCOPY);
        putimage(0, 0, &scaledG);
        
        // Sleep without holding the mutex
        Sleep(GameConfig::FRAME_DELAY);
    }
}

// Add a new function to properly clean up audio resources when closing the game
void CleanupAudioResources() {
    // Close all audio devices and resources
    mciSendString(_T("close all"), NULL, 0, NULL);
} 