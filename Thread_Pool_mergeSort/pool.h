
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

// a queue �լݬ� �٬ѬլѬ�
// a mutex �լݬ� �٬�'b'�ݬ��ڬӬѬ߬ڬ� queue

class ThreadPool
{
private:
    queue<function<void()>> taskQueue;
    vector<thread> flows;

    // mutex �� var �լݬ� �ެ�լڬ�ڬܬѬ�ڬ� ���֬�֬լ� �٬ѬլѬ�
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

