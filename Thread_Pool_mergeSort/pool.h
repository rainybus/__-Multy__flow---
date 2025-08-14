
#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <iostream>
#include <functional>
using namespace std;
#define THREAD_COUNT 4

// a queue ¬Õ¬Ý¬ñ ¬Ù¬Ñ¬Õ¬Ñ¬é
// a mutex ¬Õ¬Ý¬ñ ¬Ù¬Ñ'b'¬Ý¬à¬é¬Ú¬Ó¬Ñ¬ß¬Ú¬ñ queue

class ThreadPool
{
private:
    queue<function<void()>> taskQueue;
    vector<thread> flows;

    // mutex ¬Ú var ¬Õ¬Ý¬ñ ¬Þ¬à¬Õ¬Ú¬æ¬Ú¬Ü¬Ñ¬è¬Ú¬Ú ¬à¬é¬Ö¬â¬Ö¬Õ¬Ú ¬Ù¬Ñ¬Õ¬Ñ¬é
    mutex mutex_;
    condition_variable cond;
    bool stop;
    void flowsLoop();

public:
    ThreadPool();
    ~ThreadPool();
    void addTask(const function<void()>& task);
    function<void()> popTask();
    size_t getTotalTasks();
};

