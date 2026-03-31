#pragma once

#include <stdexcept>
#include <string>
#include <source_location>

namespace GreedSnake {

/**
 * @brief 异常类层次结构
 */
class GameException : public std::runtime_error {
public:
    explicit GameException(const char* message,
                          std::source_location loc = std::source_location::current())
        : std::runtime_error(message), location_(loc) {}
    
    [[nodiscard]] const std::source_location& Where() const noexcept {
        return location_;
    }
    
private:
    std::source_location location_;
};

class ResourceLoadException : public GameException {
public:
    explicit ResourceLoadException(const char* message,
                                    std::source_location loc = std::source_location::current())
        : GameException(message, loc) {}
};

class RenderException : public GameException {
public:
    explicit RenderException(const char* message,
                             std::source_location loc = std::source_location::current())
        : GameException(message, loc) {}
};

/**
 * @brief 结果类型 - 函数式错误处理
 * 
 * 替代异常，用于预期内的错误
 */
template<typename T, typename E = const char*>
class Result {
public:
    Result(T value) : value_(std::move(value)), hasValue_(true) {}
    Result(E error) : error_(std::move(error)), hasValue_(false) {}
    
    [[nodiscard]] bool HasValue() const noexcept { return hasValue_; }
    [[nodiscard]] bool HasError() const noexcept { return !hasValue_; }
    
    [[nodiscard]] T& Value() & {
        if (!hasValue_) {
            throw std::logic_error("Accessing value of error Result");
        }
        return value_;
    }
    
    [[nodiscard]] const T& Value() const & {
        if (!hasValue_) {
            throw std::logic_error("Accessing value of error Result");
        }
        return value_;
    }
    
    [[nodiscard]] T&& Value() && {
        if (!hasValue_) {
            throw std::logic_error("Accessing value of error Result");
        }
        return std::move(value_);
    }
    
    [[nodiscard]] E& Error() & {
        if (hasValue_) {
            throw std::logic_error("Accessing error of value Result");
        }
        return error_;
    }
    
    [[nodiscard]] const E& Error() const & {
        if (hasValue_) {
            throw std::logic_error("Accessing error of value Result");
        }
        return error_;
    }
    
    [[nodiscard]] T ValueOr(T defaultValue) const & {
        return hasValue_ ? value_ : defaultValue;
    }
    
    template<typename F>
    [[nodiscard]] auto Map(F&& f) -> Result<std::invoke_result_t<F, T>, E> {
        if (hasValue_) {
            return Result<std::invoke_result_t<F, T>, E>(f(value_));
        }
        return Result<std::invoke_result_t<F, T>, E>(error_);
    }
    
    template<typename F>
    [[nodiscard]] auto AndThen(F&& f) -> std::invoke_result_t<F, T> {
        if (hasValue_) {
            return f(value_);
        }
        return Result<typename std::invoke_result_t<F, T>::value_type, E>(error_);
    }
    
private:
    union {
        T value_;
        E error_;
    };
    bool hasValue_;
};

/**
 * @brief 可选值类型 - 轻量级 std::optional 替代
 */
template<typename T>
class Optional {
public:
    constexpr Optional() noexcept : hasValue_(false) {}
    constexpr Optional(std::nullptr_t) noexcept : hasValue_(false) {}
    constexpr Optional(T value) : value_(std::move(value)), hasValue_(true) {}
    
    ~Optional() {
        if (hasValue_) {
            value_.~T();
        }
    }
    
    // 拷贝构造
    Optional(const Optional& other) : hasValue_(other.hasValue_) {
        if (hasValue_) {
            new (&value_) T(other.value_);
        }
    }
    
    // 移动构造
    Optional(Optional&& other) noexcept : hasValue_(other.hasValue_) {
        if (hasValue_) {
            new (&value_) T(std::move(other.value_));
            other.hasValue_ = false;
        }
    }
    
    Optional& operator=(const Optional& other) {
        if (this != &other) {
            Reset();
            hasValue_ = other.hasValue_;
            if (hasValue_) {
                new (&value_) T(other.value_);
            }
        }
        return *this;
    }
    
    Optional& operator=(Optional&& other) noexcept {
        if (this != &other) {
            Reset();
            hasValue_ = other.hasValue_;
            if (hasValue_) {
                new (&value_) T(std::move(other.value_));
                other.hasValue_ = false;
            }
        }
        return *this;
    }
    
    [[nodiscard]] constexpr bool HasValue() const noexcept { return hasValue_; }
    [[nodiscard]] constexpr explicit operator bool() const noexcept { return hasValue_; }
    
    [[nodiscard]] T& Value() & {
        if (!hasValue_) {
            throw std::logic_error("Accessing empty Optional");
        }
        return value_;
    }
    
    [[nodiscard]] const T& Value() const & {
        if (!hasValue_) {
            throw std::logic_error("Accessing empty Optional");
        }
        return value_;
    }
    
    [[nodiscard]] T&& Value() && {
        if (!hasValue_) {
            throw std::logic_error("Accessing empty Optional");
        }
        return std::move(value_);
    }
    
    [[nodiscard]] T ValueOr(T defaultValue) const & {
        return hasValue_ ? value_ : defaultValue;
    }
    
    template<typename... Args>
    T& Emplace(Args&&... args) {
        Reset();
        new (&value_) T(std::forward<Args>(args)...);
        hasValue_ = true;
        return value_;
    }
    
    void Reset() noexcept {
        if (hasValue_) {
            value_.~T();
            hasValue_ = false;
        }
    }
    
private:
    alignas(alignof(T)) unsigned char storage_[sizeof(T)];
    bool hasValue_;
    
    T& value_() { return *reinterpret_cast<T*>(storage_); }
    const T& value_() const { return *reinterpret_cast<const T*>(storage_); }
};

} // namespace GreedSnake
