# Modern C++ 改进

本项目已进行 C++ 现代化改进，新增 `ModernCore` 目录包含改进后的代码。

## 改进内容

### 1. 新增核心类型
- **Vector2.h** - constexpr/noexcept 优化的 2D 向量
- **Direction.h** - 类型安全的 enum class
- **TimeUtils.h** - std::chrono 时间工具
- **ThreadPool.h** - std::jthread 线程池
- **MemoryPool.h** - 对象池和内存优化
- **ErrorHandling.h** - 现代化异常处理

### 2. 主要改进点

| 特性 | 原代码 | 改进后 |
|------|--------|--------|
| 向量运算 | 普通函数 | constexpr noexcept |
| 方向枚举 | enum | enum class |
| 时间管理 | GetTickCount | std::chrono |
| 线程管理 | std::thread | std::jthread |
| 内存分配 | new/delete | 对象池 |
| 错误处理 | 原始异常 | 结构化异常 |

### 3. 使用方法

```cpp
#include "ModernCore/Vector2.h"
#include "ModernCore/Direction.h"
#include "ModernCore/TimeUtils.h"

using namespace GreedSnake;

// 向量运算（编译期计算）
constexpr Vector2 pos{100.0f, 200.0f};
constexpr Vector2 offset{10.0f, 20.0f};
constexpr Vector2 newPos = pos + offset;

// 类型安全的枚举
Direction dir = Direction::Right;
Direction opposite = GetOppositeDirection(dir);

// 现代化计时器
Timer timer;
timer.Start();
// ... 游戏逻辑
float elapsed = timer.Elapsed();

// 高精度帧率计算
FrameRateCalculator fps;
float deltaTime = fps.Update();
```

### 4. 性能提升
- 向量运算编译期优化
- 减少动态内存分配
- 更精确的计时器
- 更安全的并发

### 5. 后续迁移建议

1. 逐步替换 Core/Vector2.h → ModernCore/Vector2.h
2. 更新游戏逻辑使用 enum class
3. 替换 GetTickCount 为 std::chrono
4. 使用 ThreadPool 管理线程
5. 考虑使用 MemoryPool 优化频繁分配的对象

## 编译要求

- C++20 或更高版本
- 支持 constexpr 和 std::jthread 的编译器

## 作者

自动化代码审查与现代化工具生成
