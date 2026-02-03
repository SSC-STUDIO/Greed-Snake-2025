#pragma once

#include <thread>
#include <mutex>
#include <memory>
#include <functional>
#include <exception>

class ThreadManager {
private:
    std::unique_ptr<std::thread> renderThread;
    std::unique_ptr<std::thread> inputThread;
    std::unique_ptr<std::thread> updateThread;
    
    std::exception_ptr renderThreadException;
    std::exception_ptr inputThreadException;
    std::exception_ptr updateThreadException;
    
    bool threadsRunning;
    mutable std::mutex threadMutex;
    
public:
    ThreadManager();
    ~ThreadManager();
    
    // Thread management
    void StartRenderThread(std::function<void()> renderFunction);
    void StartInputThread(std::function<void()> inputFunction);
    void StartUpdateThread(std::function<void()> updateFunction);
    
    void StopAllThreads();
    void JoinAllThreads();
    
    // Exception handling
    bool HasExceptions() const;
    void CheckAndRethrowExceptions();
    
    // Status
    bool AreThreadsRunning() const;
    
private:
    void SafeThreadExecution(std::function<void()> threadFunction, 
                           std::exception_ptr& exceptionPtr);
};