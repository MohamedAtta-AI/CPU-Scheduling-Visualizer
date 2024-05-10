#include "process.c"

int CompareReturn(const void *a, const void *b)
{
    Process *procA = (Process *)a;
    Process *procB = (Process *)b;

    if (procA->return_time < procB->return_time)
        return -1;
    else if (procA->return_time > procB->return_time)
        return 1;
    else
        return 0;
}

void QuickSortByReturn(Process procs[], int len)
{
    qsort(procs, len, sizeof(Process), CompareReturn);
}

void merge(Process arr[], int left, int mid, int right)
{
    int fIdx = left;
    int rIdx = mid + 1;
    int i;
    Process *sortArr = (Process *)malloc(sizeof(Process) * (right + 1));
    int sIdx = left;

    while (fIdx <= mid && rIdx <= right)
    {
        if (arr[fIdx].arrive_time <= arr[rIdx].arrive_time)
            sortArr[sIdx] = arr[fIdx++];
        else
            sortArr[sIdx] = arr[rIdx++];
        sIdx++;
    }

    if (fIdx > mid)
    {
        for (i = rIdx; i <= right; i++, sIdx++)
            sortArr[sIdx] = arr[i];
    }
    else
    {
        for (i = fIdx; i <= mid; i++, sIdx++)
            sortArr[sIdx] = arr[i];
    }

    for (i = left; i <= right; i++)
        arr[i] = sortArr[i];

    free(sortArr);
}

void MergeSortByArrival(Process arr[], int left, int right)
{
    int mid;

    if (left < right)
    {
        mid = (left + right) / 2;
        MergeSortByArrival(arr, left, mid);
        MergeSortByArrival(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}