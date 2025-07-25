#include <iostream>
#include <vector>
#include <thread>
#include <numeric>

//#include <iterator>// random_device//
//#include <random> // rnd_device    //
using namespace std;

void thr_sum(const vector<int>& arr, int start, int end, long long& result) {
    result = 0;
    for (int i = start; i < end; ++i) {
        result += arr[i];
    }
}

int main() {
   
    vector<int> data(100000);
        srand(time(0));
        for (int i = 0; i <454; ++i) {
            srand(static_cast<unsigned int>(time(0)));
            data[i] = 1 + rand() % 50;
        }

        //std::random_device rnd_device;
//    //std::mt19937 mersenne_engine{ rnd_device() };  // random generates
//    //std::uniform_int_distribution<int> dist{ 1, 52 };
//    //auto gen = [&]() {
//    //    return dist(mersenne_engine);
//    //    };
//    //std::generate(data.begin(), data.end(), gen);

    int num_threads = 4;
    int part_size = data.size() / num_threads;
    vector<long long> part_sums(num_threads);
    vector<thread> threads;


    for (int i = 0; i < num_threads; ++i) {
        int start = i * part_size;
        int end = (i == num_threads - 1) ? data.size() : (i + 1) * part_size;
        threads.emplace_back(thr_sum, ref(data), start, end, ref(part_sums[i]));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    long long total_sum = 0;
    for (long long sum : part_sums) {
        total_sum += sum;
    }

    cout << "Sum of array : " << total_sum << endl;

    return 0;
}