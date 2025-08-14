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

// ��Ҭ�֬լڬ߬֬߬ڬ� �����ܬ��
void merge(const shared_ptr<vector<int>> arr, int low, int mid, int high)
{
    vector<int> temp;

    int i = low, j = mid + 1;

    // �Ҭ֬�֬��� �߬Ѭڬެ֬߬��ڬ� 
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

    // �Ҭ֬�֬� �ݬ֬Ӭ�� ��ݬ֬Ӭ�
    while (i <= mid)
    {
        temp.push_back((*arr)[i]);
        i++;
    }

    // �Ҭ֬�֬� �ݬ֬Ӭ�� ����ѬӬ�
    while (j <= high)
    {
        temp.push_back((*arr)[j]);
        j++;
    }

    // �Ӭ�٬Ӭ�Ѭ�֬߬ڬ� ��Ҭ�֬լڬ߬֬߬߬�� ��Ѭ���
    for (i = low, j = 0; i <= high; i++, j++)
    {
        (*arr)[i] = temp[j];
    }
}

// �����ڬ��Ӭܬ� �ެѬ��ڬӬ�
void mergeSort(
    const shared_ptr<ThreadPool> pool,
    const shared_ptr<vector<int>> arr,
    int low,
    int high,
    shared_ptr<atomic<int>> parentCounter,
    const function<void()>& merger)
{
    //  2 ��ݬ֬ެ֬߬�� �լݬ� �����ڬ��Ӭܬ�
    if ((high - low + 1) <= 5000)
    {
        sort((*arr).begin() + low, (*arr).begin() + high + 1);

        // ��ެ֬߬��֬߬ڬ� ���֬��ڬܬ�
        if (parentCounter->fetch_sub(1) == 1)
            merger();
        return;
    }

    int mid = low + (high - low) / 2;

    // ����ݬ֬جڬӬѬ߬ڬ� �������߬ڬ� ��ݬڬ�߬ڬ�
    shared_ptr<atomic<int>> conter = make_shared<atomic<int>>(2);

    // ����ݬ� �٬ѬӬ֬��֬߬ڬ� �����ڬ��Ӭܬ� ��Ѭ��֬� ���֬լڬ߬֬߬ڬ� "�լ��֬�߬ڬ�",
    // ���֬լڬ߬֬߬ڬ� "���լڬ�֬ݬ��ܬڬ�" ��Ѭ��֬�
    auto bothDone = [=]()
        {
            
            merge(arr, low, mid, high);

            // �լݬ� ���߬�Ӭ߬��(�ݬ֬Ӭ�Ԭ�/���ѬӬ�Ԭ�)
            if (parentCounter->fetch_sub(1) == 1)
            {
                merger();
            }
        };

    //  �����ڬ��Ӭܬ� �� ��Ҭ�֬լڬ߬֬߬ڬ� �ݬ֬Ӭ��
    pool->addTask([=]()
        { mergeSort(
            pool,
            arr,
            low,
            mid,
            conter,
            bothDone); });

    //    �����ڬ��Ӭܬ� �� ��Ҭ�֬լڬ߬֬߬ڬ� ���ѬӬ��
    pool->addTask([=]()
        { mergeSort(
            pool,
            arr,
            mid + 1,
            high,
            conter,
            bothDone); });
}