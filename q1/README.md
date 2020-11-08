# Report for Question-1  CONCURRENT MERGE SORT
---
In this question mergesort has been performed on an array of n integers but with a slight modification which is, if the size of array to be sorted becomes less that **5**, then selection sort is performed on it.

1. Normal MergeSort
* This normal mergesort is performed in the function 
``` 
void normal_mergesort(int *arr,int l,int r) 
```
where positions of the array to be used are passed in the function as arguments. This function is performed using recursion and finally both halves are merged using function
```
void merge(int *arr,int l,int m,int r)
```

2. Concurrent Mergesort
* This concurrent Mergesort is carried by making two child processes,one for the left half and other for the right half. The left process is calling itself resursively and similarly the right process is also calling itself resursively.
This is performed in the following function
```
void concurrent_mergesort(int *arr,int l, int r) 
```
and in the end both halves are merged using fucntion
```
void merge(int *arr,int l,int m,int r)
```

3. Threaded Mergesort
* In this two threads are created, one for the left half and other for right half.The left thread is calling itself recursively and right thread is also calling itself recursively and in the end it is merged using merge function.
``` 
void *threaded_mergesort(void* arr) 
```
``` 
void merge(int *arr,int l,int m,int r) 
```

## Performance Comparison
#### It was observed that for n=10 time taken by
- Normal mergesort - **0.000049**
- Concurrent mergesort - **0.001153**
- Threaded mergesort - **0.000807**

Therefore we observed that normalmergesort ran **23.687720** times faster than concurrent_mergesort and **16.578909** times faster than threaded_concurrent_mergesort.

## Reason of this behaviour
- The normal sort ran faster than concurrent sort because in concurrent sort when,say left child access the left array, the array is loaded into the cache of a processor. Now when the right array is accessed(because of concurrent accesses), there is a cache miss since the cache is filled with left segment and then right segment is copied to the cache memory. This to and fro process continues and it degrades the performance to such a level that it performs poorer than normal mergesort.