#pragma once

#include <cstdint>

namespace GreedSnake {

/**
 * @brief 方向枚举 - 使用 enum class 替代 enum
 * 
 * 类型安全，不会隐式转换为整数
 */
enum class Direction : uint8_t {
    Up,
    Down,
    Left,
    Right
};

/**
 * @brief 菜单选项
 */
enum class MenuOption : uint8_t {
    StartGame,
    Settings,
    About,
    Exit
};

/**
 * @brief 游戏难度
 */
enum class Difficulty : uint8_t {
    Easy,
    Normal,
    Hard
};

/**
 * @brief 游戏状态
 */
enum class GameStatus : uint8_t {
    Menu,
    Playing,
    Paused,
    GameOver
};

/**
 * @brief 获取方向的反方向
 */
[[nodiscard]] constexpr Direction GetOppositeDirection(Direction dir) noexcept {
    switch (dir) {
        case Direction::Up:    return Direction::Down;
        case Direction::Down:  return Direction::Up;
        case Direction::Left:  return Direction::Right;
        case Direction::Right: return Direction::Left;
    }
    return dir; // 永远不会执行到这里
}

/**
 * @brief 将方向转换为字符串（用于调试）
 */
[[nodiscard]] constexpr const char* DirectionToString(Direction dir) noexcept {
    switch (dir) {
        case Direction::Up:    return "Up";
        case Direction::Down:  return "Down";
        case Direction::Left:  return "Left";
        case Direction::Right: return "Right";
    }
    return "Unknown";
}

} // namespace GreedSnake
