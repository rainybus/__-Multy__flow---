#include <algorithm> // inplace_merge
#include <vector>
#include <future>    // async, future, promise
#include <thread>    // hardware_concurrency
#include <iostream>
using namespace std;

//соединение подпотоков
template<typename Iter>
void merge(Iter begin, Iter mid, Iter end) {
    inplace_merge(begin, mid, end);
}

template<typename Iter>
void parallel_merge_sort(Iter begin, Iter end, unsigned int max_threads) {
    long long len = distance(begin, end);

    // порог, исходя из возможностей системы
    if (len <= 1024) {
        sort(begin, end);
        return;
    }

    Iter mid = next(begin, len / 2);

    // параллельный рекурсивный вызов потоков(по возможности)
    if (max_threads > 1) {
        // async - запуск разделенных лев. и правого
        future<void> future_left = async(launch::async,
            parallel_merge_sort<Iter>,
            begin, mid, max_threads / 2);

        // сортировка правой части в текущем потоке
        parallel_merge_sort(mid, end, max_threads - (max_threads / 2));

        // ожидание завершения левой части
        future_left.wait();
    }
    else {
        //при отсутствии возможности запуска потоков, последовательная сортиовка
        parallel_merge_sort(begin, mid, 0); // 0 
        parallel_merge_sort(mid, end, 0);
    }

    // соединение
    merge(begin, mid, end);
}



int main() {
    vector<int> data = { 38, 27, 43, 3, 9, 82, 10, 1, 5, 2, 7, 6, 4, 8, 0, 11 };

    // оценка кол-ва возможных потоков (hardware concurrency)
    unsigned int num_threads = thread::hardware_concurrency();
    if (num_threads == 0) { // 0 - неясность
        num_threads = 2;
    }

    //int num_threads = 2;
    cout << "Original array: ";
    for (int x : data) {
        cout << x << " ";
    }
    std::cout << std::endl;

    parallel_merge_sort(data.begin(), data.end(), num_threads);

    cout << "Sorted array: ";
    for (int x : data) {
        cout << x << " ";
    }
    cout << endl;

    return 0;
}

