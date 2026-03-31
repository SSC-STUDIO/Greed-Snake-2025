#pragma once

#include <cstddef>
#include <new>
#include <memory>

namespace GreedSnake {

/**
 * @brief 对象池 - 减少动态内存分配
 * 
 * 使用 placement new 和显式析构提高性能
 */
template<typename T, size_t PoolSize>
class ObjectPool {
public:
    ObjectPool() noexcept {
        // 初始化自由列表
        for (size_t i = 0; i < PoolSize; ++i) {
            freeList_[i] = &storage_[i * sizeof(T)];
        }
        freeCount_ = PoolSize;
    }
    
    ~ObjectPool() noexcept {
        // 注意：使用对象池的对象必须在归还时手动调用析构函数
    }
    
    // 禁用拷贝和移动
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
    
    /**
     * @brief 从池中获取对象
     */
    template<typename... Args>
    T* Acquire(Args&&... args) {
        if (freeCount_ == 0) {
            return nullptr; // 池已满
        }
        
        void* mem = freeList_[--freeCount_];
        return new (mem) T(std::forward<Args>(args)...);
    }
    
    /**
     * @brief 将对象归还到池
     */
    void Release(T* obj) noexcept {
        if (obj == nullptr || freeCount_ >= PoolSize) {
            return;
        }
        
        // 显式调用析构函数
        obj->~T();
        
        // 归还内存
        freeList_[freeCount_++] = obj;
    }
    
    /**
     * @brief 获取池中可用对象数量
     */
    [[nodiscard]] size_t Available() const noexcept {
        return freeCount_;
    }
    
    /**
     * @brief 获取池容量
     */
    [[nodiscard]] constexpr size_t Capacity() const noexcept {
        return PoolSize;
    }
    
private:
    alignas(alignof(T)) unsigned char storage_[PoolSize * sizeof(T)];
    void* freeList_[PoolSize];
    size_t freeCount_ = 0;
};

/**
 * @brief 内存池分配器 - 用于 STL 容器
 */
template<typename T, size_t PoolSize>
class PoolAllocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;
    using is_always_equal = std::false_type;
    
    PoolAllocator() noexcept = default;
    explicit PoolAllocator(ObjectPool<T, PoolSize>* pool) noexcept : pool_(pool) {}
    
    template<typename U>
    PoolAllocator(const PoolAllocator<U, PoolSize>& other) noexcept : pool_(other.pool_) {}
    
    [[nodiscard]] T* allocate(size_type n) {
        if (n != 1 || pool_ == nullptr) {
            // 回退到标准分配器
            return static_cast<T*>(::operator new(n * sizeof(T)));
        }
        T* obj = pool_->Acquire();
        if (obj == nullptr) {
            throw std::bad_alloc();
        }
        return obj;
    }
    
    void deallocate(T* p, size_type n) noexcept {
        if (n != 1 || pool_ == nullptr) {
            ::operator delete(p);
            return;
        }
        pool_->Release(p);
    }
    
    template<typename U, size_t N>
    friend class PoolAllocator;
    
private:
    ObjectPool<T, PoolSize>* pool_ = nullptr;
};

/**
 * @brief 固定大小数组 - 替代动态 vector（当大小已知时）
 */
template<typename T, size_t N>
class FixedArray {
public:
    using value_type = T;
    using size_type = std::size_t;
    using reference = T&;
    using const_reference = const T&;
    using iterator = T*;
    using const_iterator = const T*;
    
    FixedArray() noexcept = default;
    
    // 支持初始化列表
    FixedArray(std::initializer_list<T> init) {
        size_type i = 0;
        for (const auto& val : init) {
            if (i >= N) break;
            data_[i++] = val;
        }
        size_ = i;
    }
    
    [[nodiscard]] constexpr size_type size() const noexcept { return size_; }
    [[nodiscard]] constexpr size_type capacity() const noexcept { return N; }
    [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; }
    
    [[nodiscard]] reference operator[](size_type index) noexcept { return data_[index]; }
    [[nodiscard]] const_reference operator[](size_type index) const noexcept { return data_[index]; }
    
    [[nodiscard]] reference at(size_type index) {
        if (index >= size_) {
            throw std::out_of_range("FixedArray::at");
        }
        return data_[index];
    }
    
    [[nodiscard]] const_reference at(size_type index) const {
        if (index >= size_) {
            throw std::out_of_range("FixedArray::at");
        }
        return data_[index];
    }
    
    void push_back(const T& value) {
        if (size_ < N) {
            data_[size_++] = value;
        }
    }
    
    void push_back(T&& value) {
        if (size_ < N) {
            data_[size_++] = std::move(value);
        }
    }
    
    void pop_back() noexcept {
        if (size_ > 0) {
            --size_;
        }
    }
    
    void clear() noexcept { size_ = 0; }
    
    [[nodiscard]] iterator begin() noexcept { return data_; }
    [[nodiscard]] iterator end() noexcept { return data_ + size_; }
    [[nodiscard]] const_iterator begin() const noexcept { return data_; }
    [[nodiscard]] const_iterator end() const noexcept { return data_ + size_; }
    
    [[nodiscard]] reference front() noexcept { return data_[0]; }
    [[nodiscard]] reference back() noexcept { return data_[size_ - 1]; }
    [[nodiscard]] const_reference front() const noexcept { return data_[0]; }
    [[nodiscard]] const_reference back() const noexcept { return data_[size_ - 1]; }
    
private:
    T data_[N];
    size_type size_ = 0;
};

} // namespace GreedSnake
