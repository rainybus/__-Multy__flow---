#include "pool.h"

ThreadPool::ThreadPool()
{
    cout << "\n\tNo. threads: " << THREAD_COUNT << endl;

    // �լ�ҬѬӬݬ֬߬ڬ� �����ܬ��
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        this->flows.emplace_back([this]()
            { this->flowsLoop(); });
    }

    this->stop = false;
}

// �լ�ҬѬӬݬ֬߬ڬ� �٬ѬլѬ� �����ܬѬ�
void ThreadPool::addTask(const function<void()>& task)
{
    // �Ӭ� �ڬ٬Ҭ֬جѬ߬ڬ� "�Ԭ�߬ܬ�"
    lock_guard<mutex> lock(this->mutex_);

    // �լ�ҬѬӬݬ֬߬ڬ� �٬ѬլѬ� �����ܬѬ�
    this->taskQueue.push(move(task));

    // ����Ӭ֬�֬߬ڬ� �����ܬ� �� �٬ѬլѬ��
    this->cond.notify_one();
}

// �Ӭ�٬Ӭ�Ѭ�Ѭ֬� ��֬�Ӭ�� �٬ѬլѬ��
function<void()> ThreadPool::popTask()
{
    lock_guard<mutex> lock(this->mutex_);

    if (this->taskQueue.empty())
        return nullptr;

    // �٬�'b'�ݬ��ڬӬѬ߬ڬ� �Ӭ� �ڬ٬Ҭ֬جѬ߬ڬ� "�Ԭ�߬ܬ�"

    // �Ҭ֬�֬��� ��֬�ӬѬ� �٬ѬլѬ��
    auto task = move(this->taskQueue.front());

    // ��լѬݬ֬߬ڬ� �ڬ� ���֬�֬լ� �٬ѬլѬ�
    this->taskQueue.pop();

    return task;
}

// ��ڬܬ� �����ܬ�(��جڬլѬ߬ڬ� - �Ӭ���ݬ߬֬߬ڬ�)
void ThreadPool::flowsLoop()
{
    while (1)
    {
        function<void()> task = nullptr;
        {
            // �٬�'b'�ݬ��ڬӬѬ߬ڬ�
            unique_lock<mutex> lock(this->mutex_);

            // ��جڬլѬ߬ڬ� �٬ѬլѬ��
            this->cond.wait(lock, [this]()
                { return !taskQueue.empty() || this->stop; });

            // �٬Ѭܬ���ڬ� �����ܬ� ���� ��������Ӭڬ� �٬ѬլѬ��
            if (this->stop && this->taskQueue.empty())
            {
                cout << "\n\tcut thread: " << this_thread::get_id() << endl;
                break;
            }

            //��Ѭ�'b'�ݬ��ڬӬѬ߬ڬ�
            lock.unlock();

            // �Ҭ֬�֬� ��֬�Ӭ�� �٬ѬլѬ��
            task = this->popTask();
        }
        // �Ӭ���ݬ߬֬߬ڬ� �٬ѬլѬ��
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

    // ����Ӭ֬�֬߬ڬ� : �Ӭ�֬� �߬� �Ӭ����
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