#include "pool.h"

ThreadPool::ThreadPool()
{
    cout << "\n\tNo. threads: " << THREAD_COUNT << endl;

    // ¬Õ¬à¬Ò¬Ñ¬Ó¬Ý¬Ö¬ß¬Ú¬Ö ¬á¬à¬ä¬à¬Ü¬à¬Ó
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        this->flows.emplace_back([this]()
            { this->flowsLoop(); });
    }

    this->stop = false;
}

// ¬Õ¬à¬Ò¬Ñ¬Ó¬Ý¬Ö¬ß¬Ú¬Ö ¬Ù¬Ñ¬Õ¬Ñ¬é ¬á¬à¬ä¬à¬Ü¬Ñ¬Þ
void ThreadPool::addTask(const function<void()>& task)
{
    // ¬Ó¬à ¬Ú¬Ù¬Ò¬Ö¬Ø¬Ñ¬ß¬Ú¬Ö "¬Ô¬à¬ß¬Ü¬Ú"
    lock_guard<mutex> lock(this->mutex_);

    // ¬Õ¬à¬Ò¬Ñ¬Ó¬Ý¬Ö¬ß¬Ú¬Ö ¬Ù¬Ñ¬Õ¬Ñ¬é ¬á¬à¬ä¬à¬Ü¬Ñ¬Þ
    this->taskQueue.push(move(task));

    // ¬à¬á¬à¬Ó¬Ö¬ë¬Ö¬ß¬Ú¬Ö ¬á¬à¬ä¬à¬Ü¬Ñ ¬à ¬Ù¬Ñ¬Õ¬Ñ¬é¬Ö
    this->cond.notify_one();
}

// ¬Ó¬à¬Ù¬Ó¬â¬Ñ¬ë¬Ñ¬Ö¬ä ¬á¬Ö¬â¬Ó¬å¬ð ¬Ù¬Ñ¬Õ¬Ñ¬é¬å
function<void()> ThreadPool::popTask()
{
    lock_guard<mutex> lock(this->mutex_);

    if (this->taskQueue.empty())
        return nullptr;

    // ¬Ù¬Ñ'b'¬Ý¬à¬é¬Ú¬Ó¬Ñ¬ß¬Ú¬Ö ¬Ó¬à ¬Ú¬Ù¬Ò¬Ö¬Ø¬Ñ¬ß¬Ú¬Ö "¬Ô¬à¬ß¬Ü¬Ú"

    // ¬Ò¬Ö¬â¬Ö¬ä¬ã¬ñ ¬á¬Ö¬â¬Ó¬Ñ¬ñ ¬Ù¬Ñ¬Õ¬Ñ¬é¬Ñ
    auto task = move(this->taskQueue.front());

    // ¬å¬Õ¬Ñ¬Ý¬Ö¬ß¬Ú¬Ö ¬Ú¬Ù ¬à¬é¬Ö¬â¬Ö¬Õ¬Ú ¬Ù¬Ñ¬Õ¬Ñ¬é
    this->taskQueue.pop();

    return task;
}

// ¬è¬Ú¬Ü¬Ý ¬á¬à¬ä¬à¬Ü¬Ñ(¬à¬Ø¬Ú¬Õ¬Ñ¬ß¬Ú¬Ö - ¬Ó¬í¬á¬à¬Ý¬ß¬Ö¬ß¬Ú¬Ö)
void ThreadPool::flowsLoop()
{
    while (1)
    {
        function<void()> task = nullptr;
        {
            // ¬Ù¬Ñ'b'¬Ý¬à¬é¬Ú¬Ó¬Ñ¬ß¬Ú¬Ö
            unique_lock<mutex> lock(this->mutex_);

            // ¬à¬Ø¬Ú¬Õ¬Ñ¬ß¬Ú¬Ö ¬Ù¬Ñ¬Õ¬Ñ¬é¬Ú
            this->cond.wait(lock, [this]()
                { return !taskQueue.empty() || this->stop; });

            // ¬Ù¬Ñ¬Ü¬â¬í¬ä¬Ú¬Ö ¬á¬à¬ä¬à¬Ü¬Ñ ¬á¬â¬Ú ¬à¬ä¬ã¬å¬ä¬ã¬ä¬Ó¬Ú¬Ú ¬Ù¬Ñ¬Õ¬Ñ¬é¬Ú
            if (this->stop && this->taskQueue.empty())
            {
                cout << "\n\tcut thread: " << this_thread::get_id() << endl;
                break;
            }

            //¬â¬Ñ¬Ù'b'¬Ý¬à¬é¬Ú¬Ó¬Ñ¬ß¬Ú¬Ö
            lock.unlock();

            // ¬Ò¬Ö¬â¬Ö¬Þ ¬á¬Ö¬â¬Ó¬å¬ð ¬Ù¬Ñ¬Õ¬Ñ¬é¬å
            task = this->popTask();
        }
        // ¬Ó¬í¬á¬à¬Ý¬ß¬Ö¬ß¬Ú¬Ö ¬Ù¬Ñ¬Õ¬Ñ¬é¬Ú
        if (task)
        {
            task();
        }
    }
}
ThreadPool::~ThreadPool()
{
    // std::cout <<\n\tthread pool being destroyed" << endl;
    {
        unique_lock<mutex> lock(this->mutex_);
        this->stop = true;
    }

    // ¬à¬á¬à¬Ó¬Ö¬ë¬Ö¬ß¬Ú¬Ö : ¬Ó¬ã¬Ö¬Þ ¬ß¬Ñ ¬Ó¬í¬ç¬à¬Õ
    this->cond.notify_all();

    for (auto& thread : this->flows)
    {
        if (thread.joinable())
        {
            thread.join();
            //cout << "thread joined" << this_thread::get_id() << endl;
        }
    }
}

size_t ThreadPool::getTotalTasks()
{
    return this->taskQueue.size();
}