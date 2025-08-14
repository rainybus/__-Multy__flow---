#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <future>
#include <mutex>
#include <condition_variable>
#include <numeric>
#include <algorithm>
#include <random>
using namespace std;

//  структура для задач
struct Task {
    function<void()> func;
    shared_ptr<void> parent_task_ref; // Ссылка на родительскую задачу
};

// очередь work stealing
template <typename T>
class ConcurrentQueue {
public:
    void push(T value) {
        unique_lock<mutex> lock(mtx_);
        queue_.push(move(value));
        cv_.notify_one();
    }

    bool try_pop(T& value) {
        unique_lock<mutex> lock(mtx_);
        if (queue_.empty()) {
            return false;
        }
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    T pop() {
        unique_lock<mutex> lock(mtx_);
        cv_.wait(lock, [this] { return !queue_.empty(); });
        T value = move(queue_.front());
        queue_.pop();
        return value;
    }

private:
    queue<T> queue_;
    mutable mutex mtx_;
    condition_variable cv_;
};

class ThreadPool {
public:
    ThreadPool(size_t num_threads) : stop_(false) {
        for (size_t i = 0; i < num_threads; ++i) {
            local_queues_.emplace_back(std::make_unique<ConcurrentQueue<Task>>());
            threads_.emplace_back(&ThreadPool::worker_thread, this, i);
        }
    }

    ~ThreadPool() {
        stop_ = true;
        for (auto& queue : local_queues_) {
            queue->push({}); // ввод пустой(задачи) для разблокировки ожидающего потока
        }
        for (auto& thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> future<decltype(f(args...))> {
        auto task_ptr = make_shared<packaged_task<decltype(f(args...))()>>(
            bind(forward<F>(f), forward<Args>(args)...)
        );
        future<decltype(f(args...))> future = task_ptr->get_future();

        // packaged_task чтоб не спал
        Task task;
        task.func = [task_ptr]() { (*task_ptr)(); };
        task.parent_task_ref = task_ptr;

        
        size_t thread_idx = hash<thread::id>{}(this_thread::get_id()) % local_queues_.size();
        local_queues_[thread_idx]->push(move(task));// вставляем в текущую, либо случайную очередь

        return future;
    }

private:
    void worker_thread(size_t thread_id) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> distrib(0, local_queues_.size() - 1);

        while (!stop_) {
            Task task;
            if (local_queues_[thread_id]->try_pop(task)) {
                if (task.func) {
                    task.func();
                }
            }
            else {
                // Work stealing
                bool stolen = false;
                for (size_t i = 0; i < local_queues_.size(); ++i) {
                    size_t steal_from_idx = distrib(gen); // random перехват задачи
                    if (steal_from_idx == thread_id) continue;

                    if (local_queues_[steal_from_idx]->try_pop(task)) {
                        if (task.func) {
                            task.func();
                            stolen = true;
                            break;
                        }
                    }
                }
                if (!stolen) {
                    // если ничего не нашли - ждем свою очередь
                    if (!stop_) { // проверка stop_ перед остановкой
                        task = local_queues_[thread_id]->pop();
                        if (task.func) {
                            task.func();
                        }
                    }
                }
            }
        }
    }

    vector<thread> threads_;
    vector<unique_ptr<ConcurrentQueue<Task>>> local_queues_;
    atomic<bool> stop_;
};

//  реализация Merge Sort
void merge(std::vector<int>& arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    std::vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    int i = 0;
    int j = 0;
    int k = l;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void parallel_merge_sort(std::vector<int>& arr, int l, int r, ThreadPool& pool) {
    if (l < r) {
        int m = l + (r - l) / 2;

        if ((r - l) < 1024) { // для последовательной сортировки
            parallel_merge_sort(arr, l, m, pool);
            parallel_merge_sort(arr, m + 1, r, pool);
        }
        else {
            auto future1 = pool.submit([&]() { parallel_merge_sort(arr, l, m, pool); });
            auto future2 = pool.submit([&]() { parallel_merge_sort(arr, m + 1, r, pool); });
            future1.get();
            future2.get();
        }
        merge(arr, l, m, r);
    }
}

int main() {
    vector<int> data(100);
    iota(data.begin(), data.end(), 0);
    shuffle(data.begin(), data.end(), mt19937{ random_device{}() });
    cout << "Before sorting : " << endl;
    for (int x : data) {
        cout << x << " ";
    }
    cout << endl;

    ThreadPool pool(thread::hardware_concurrency());

    cout << "Sorting " << data.size() << " elements..." << endl;
    parallel_merge_sort(data, 0, data.size() - 1, pool);
    for (int x : data) {
                cout << x << " ";
            }
            cout << endl;
    cout << "Sorting complete." << endl;

    // Подтверждение 
    bool sorted = is_sorted(data.begin(), data.end());
    cout << "Array is sorted: " << (sorted ? "true" : "false") << endl;

    return 0;
}

