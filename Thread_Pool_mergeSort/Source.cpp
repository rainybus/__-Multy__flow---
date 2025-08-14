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

    // ���٬լѬ߬ڬ� �լڬ߬Ѭެڬ�֬�ܬ�Ԭ� �ެѬ��ڬӬ�
    shared_ptr<vector<int>>
        arr = make_shared<vector<int>>(array);

    // ���� �����ܬ��, �ѬӬ��ެѬ�ڬ�֬�ܬ� ���٬լѬ֬� �����ܬ�
    shared_ptr<ThreadPool> pool = make_shared<ThreadPool>();

    // ���֬��ڬ� 
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

        // ��جڬլѬ߬ڬ� ��ܬ�߬�Ѭ߬ڬ� �����ܬ��
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