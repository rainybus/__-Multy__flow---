#include "merge.h"

void merge(vector<int>& arr, int low, int mid, int high)
{
    vector<int> temp;
    int i = low, j = mid + 1;

    while (i <= mid && j <= high)
    {
        if (arr[i] <= arr[j])
            temp.push_back(arr[i++]);
        else
            temp.push_back(arr[j++]);
    }

    while (i <= mid)
        temp.push_back(arr[i++]);
    while (j <= high)
        temp.push_back(arr[j++]);

    for (int k = low, t = 0; k <= high; ++k, ++t)
    {
        arr[k] = temp[t];
    }
}

void mergeSort(vector<int>& arr, int low, int high)
{

    if (low >= high)
        return;

    int mid = low + (high - low) / 2;

    mergeSort(arr, low, mid);
    mergeSort(arr, mid + 1, high);

    merge(arr, low, mid, high);
}

// ¬à¬Ò¬ì¬Ö¬Õ¬Ú¬ß¬Ö¬ß¬Ú¬Ö ¬á¬à¬ä¬à¬Ü¬à¬Ó
void merge(const shared_ptr<vector<int>> arr, int low, int mid, int high)
{
    vector<int> temp;

    int i = low, j = mid + 1;

    // ¬Ò¬Ö¬â¬Ö¬ä¬ã¬ñ ¬ß¬Ñ¬Ú¬Þ¬Ö¬ß¬î¬ê¬Ú¬Û 
    while (i <= mid && j <= high)
    {
        if ((*arr)[i] <= (*arr)[j])
        {
            temp.push_back((*arr)[i]);
            i++;
        }
        else
        {
            temp.push_back((*arr)[j]);
            j++;
        }
    }

    // ¬Ò¬Ö¬â¬Ö¬Þ ¬Ý¬Ö¬Ó¬í¬Û ¬ã¬Ý¬Ö¬Ó¬Ñ
    while (i <= mid)
    {
        temp.push_back((*arr)[i]);
        i++;
    }

    // ¬Ò¬Ö¬â¬Ö¬Þ ¬Ý¬Ö¬Ó¬í¬Û ¬ã¬á¬â¬Ñ¬Ó¬Ñ
    while (j <= high)
    {
        temp.push_back((*arr)[j]);
        j++;
    }

    // ¬Ó¬à¬Ù¬Ó¬â¬Ñ¬ë¬Ö¬ß¬Ú¬Ö ¬à¬Ò¬ì¬Ö¬Õ¬Ú¬ß¬Ö¬ß¬ß¬à¬Û ¬é¬Ñ¬ã¬ä¬Ú
    for (i = low, j = 0; i <= high; i++, j++)
    {
        (*arr)[i] = temp[j];
    }
}

// ¬ã¬à¬â¬ä¬Ú¬â¬à¬Ó¬Ü¬Ñ ¬Þ¬Ñ¬ã¬ã¬Ú¬Ó¬Ñ
void mergeSort(
    const shared_ptr<ThreadPool> pool,
    const shared_ptr<vector<int>> arr,
    int low,
    int high,
    shared_ptr<atomic<int>> parentCounter,
    const function<void()>& merger)
{
    //  2 ¬ï¬Ý¬Ö¬Þ¬Ö¬ß¬ä¬Ñ ¬Õ¬Ý¬ñ ¬ã¬à¬â¬ä¬Ú¬â¬à¬Ó¬Ü¬Ú
    if ((high - low + 1) <= 5000)
    {
        sort((*arr).begin() + low, (*arr).begin() + high + 1);

        // ¬å¬Þ¬Ö¬ß¬î¬ê¬Ö¬ß¬Ú¬Ö ¬ã¬é¬Ö¬ä¬é¬Ú¬Ü¬Ñ
        if (parentCounter->fetch_sub(1) == 1)
            merger();
        return;
    }

    int mid = low + (high - low) / 2;

    // ¬à¬ä¬ã¬Ý¬Ö¬Ø¬Ú¬Ó¬Ñ¬ß¬Ú¬Ö ¬ã¬à¬ã¬ä¬à¬ñ¬ß¬Ú¬Ö ¬ã¬Ý¬Ú¬ñ¬ß¬Ú¬ñ
    shared_ptr<atomic<int>> conter = make_shared<atomic<int>>(2);

    // ¬á¬à¬ã¬Ý¬Ö ¬Ù¬Ñ¬Ó¬Ö¬â¬ê¬Ö¬ß¬Ú¬ñ ¬ã¬à¬â¬ä¬Ú¬â¬à¬Ó¬Ü¬Ú ¬é¬Ñ¬ã¬ä¬Ö¬Û ¬ã¬à¬Ö¬Õ¬Ú¬ß¬Ö¬ß¬Ú¬Ö "¬Õ¬à¬é¬Ö¬â¬ß¬Ú¬ç",
    // ¬ã¬à¬Ö¬Õ¬Ú¬ß¬Ö¬ß¬Ú¬Ö "¬â¬à¬Õ¬Ú¬ä¬Ö¬Ý¬î¬ã¬Ü¬Ú¬ç" ¬é¬Ñ¬ã¬ä¬Ö¬Û
    auto bothDone = [=]()
        {
            
            merge(arr, low, mid, high);

            // ¬Õ¬Ý¬ñ ¬à¬ã¬ß¬à¬Ó¬ß¬í¬ç(¬Ý¬Ö¬Ó¬à¬Ô¬à/¬á¬â¬Ñ¬Ó¬à¬Ô¬à)
            if (parentCounter->fetch_sub(1) == 1)
            {
                merger();
            }
        };

    //  ¬ã¬à¬â¬ä¬Ú¬â¬à¬Ó¬Ü¬Ñ ¬Ú ¬à¬Ò¬ì¬Ö¬Õ¬Ú¬ß¬Ö¬ß¬Ú¬Ö ¬Ý¬Ö¬Ó¬í¬ç
    pool->addTask([=]()
        { mergeSort(
            pool,
            arr,
            low,
            mid,
            conter,
            bothDone); });

    //    ¬ã¬à¬â¬ä¬Ú¬â¬à¬Ó¬Ü¬Ñ ¬Ú ¬à¬Ò¬ì¬Ö¬Õ¬Ú¬ß¬Ö¬ß¬Ú¬Ö ¬á¬â¬Ñ¬Ó¬í¬ç
    pool->addTask([=]()
        { mergeSort(
            pool,
            arr,
            mid + 1,
            high,
            conter,
            bothDone); });
}