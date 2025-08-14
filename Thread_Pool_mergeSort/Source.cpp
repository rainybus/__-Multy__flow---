#include "merge.h"
#include <iostream>
#include <atomic>
#include <vector>
#include <memory>
#include <future>
#include <chrono>

#define ARRAY_SIZE 1000

int main(int argc, char const* argv[])
{
    vector<int> array(ARRAY_SIZE);

    // cout << "\tBefore sort: " << endl;
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        int x = rand() % ARRAY_SIZE;
        array[i] = x;
        // cout << x << " ";
    }

    auto start = chrono::high_resolution_clock::now();

    mergeSort(array, 0, array.size() - 1);

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;

    cout << "\n\tsimple merge sort: " << elapsed.count() << " seconds" << endl;

    // ¬ã¬à¬Ù¬Õ¬Ñ¬ß¬Ú¬Ö ¬Õ¬Ú¬ß¬Ñ¬Þ¬Ú¬é¬Ö¬ã¬Ü¬à¬Ô¬à ¬Þ¬Ñ¬ã¬ã¬Ú¬Ó¬Ñ
    shared_ptr<vector<int>>
        arr = make_shared<vector<int>>(array);

    // ¬á¬å¬Ý ¬á¬à¬ä¬à¬Ü¬à¬Ó, ¬Ñ¬Ó¬ä¬à¬Þ¬Ñ¬ä¬Ú¬é¬Ö¬ã¬Ü¬Ú ¬ã¬à¬Ù¬Õ¬Ñ¬Ö¬ä ¬á¬à¬ä¬à¬Ü¬Ú
    shared_ptr<ThreadPool> pool = make_shared<ThreadPool>();

    // ¬ã¬é¬Ö¬ä¬é¬Ú¬Ü 
    shared_ptr<atomic<int>> counter = make_shared<atomic<int>>(1);

    promise<void> donePromise;
    future<void> doneFuture = donePromise.get_future();

    int low = 0,
        high = arr->size() - 1;

    try
    {
        start = chrono::high_resolution_clock::now();

        pool->addTask([=, &donePromise]() mutable
            { mergeSort(pool, arr, low, high, counter, [&donePromise]()
                {
                    // std::cout<<"\n\tdone sorting"<<endl;
                    donePromise.set_value(); }); });

        // ¬à¬Ø¬Ú¬Õ¬Ñ¬ß¬Ú¬Ö ¬à¬Ü¬à¬ß¬é¬Ñ¬ß¬Ú¬ñ ¬á¬à¬ä¬à¬Ü¬à¬Ó
        doneFuture.get();

        end = chrono::high_resolution_clock::now();

        chrono::duration<double> elapsed = end - start;

        cout << "\n\tthreaded merge sort: " << elapsed.count() << " seconds" << endl;
    }
    catch (const exception& e)
    {
        cerr << e.what() << endl;
    }

     cout << "\n\tAfter sort: \n" << endl;
     for (auto &x : (*arr))
     {
         cout << x << "\t ";
     }
     cout << endl;
    return 0;
}