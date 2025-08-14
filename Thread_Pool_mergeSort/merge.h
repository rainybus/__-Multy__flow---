
#pragma once
#include "pool.h"
#include <memory>
#include <vector>
#include <functional>
#include <algorithm>
#include <atomic>

void merge(vector<int>& arr, int low, int mid, int high);

void mergeSort(vector<int>& arr, int low, int high);

void merge(const shared_ptr<vector<int>> arr, int low, int mid, int high);

void mergeSort(const shared_ptr<ThreadPool> pool,
    const shared_ptr<vector<int>> arr,
    int low,
    int high,
    shared_ptr<atomic<int>> parentCounter,
    const function<void()>& merger);

