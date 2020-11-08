#define _POSIX_C_SOURCE 199309L //required for clock
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>
typedef long long int ll;

int * shareMem(size_t size){
     key_t mem_key = IPC_PRIVATE;
     int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
     return (int*)shmat(shm_id, NULL, 0);
}

void printarr(int *arr,ll size)
{
    for(ll i=0;i<size;i++) printf("%d ",arr[i]); 
    printf("\n");
}
void swap(int *x, int *y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

void merge(int *arr,int l,int m,int r)
{
    //merging two already sorted arrays
    ll size1 = m-l+1;
    ll size2 = r-m;
    int left[size1],right[size2];
    for(ll i=0;i<size1;i++) left[i] = arr[l+i];
    for(ll j=0;j<size2;j++) right[j] = arr[m+1+j];
    ll i=0,j=0,k=l;
    while (i<size1 && j<size2)
    {
        if(left[i]<=right[j]){
            arr[k]=left[i]; i++;
        }
        else{
            arr[k]=right[j]; j++;
        }
        k++;
    }
    while (i<size1)
    {
        arr[k]=left[i];
        i++; k++;
    }
    while (j<size2)
    {
        arr[k]=right[j];
        j++; k++;
    }
}

void normal_mergesort(int *arr,int l,int r)
{
    if(l<r)
    {
        // implementing normal merge sort using recursion
        if((r-l+1)<5)                           
        {               
            //perform selection sort for no. of elements less than 5
            for(ll i=l;i<r;i++)
            {
                int minind = i;
                for(ll j=i+1;j<=r;j++)
                    if(arr[j]< arr[minind])
                        minind = j;
                swap(&arr[i],&arr[minind]);
            }
            return;
        }
        int m = l+(r-l)/2;
        normal_mergesort(arr,l,m);
        normal_mergesort(arr,m+1,r);
        merge(arr,l,m,r); 
    }
}

void concurrent_mergesort(int *arr,int l, int r)
{
    if(l<r)
    {
        //implementing merge sort by making two concurrent child processes
        if((r-l+1)<5)                           
        {               
            //perform selection sort for no. of elements less than 5
            for(ll i=l;i<r;i++)
            {
                int minind = i;
                for(ll j=i+1;j<=r;j++)
                    if(arr[j]< arr[minind])
                        minind = j;
                swap(&arr[i],&arr[minind]);
            }
            return;
        }
        int m = l+(r-l)/2;
        pid_t pid1,pid2;
        pid1 = fork();
        if(pid1==0)
        {
            concurrent_mergesort(arr,l,m);
            _exit(1);
        }
        else{
            pid2 = fork();
            if(pid2==0){
                concurrent_mergesort(arr,m+1,r);
                _exit(1);
            }
            else{
                int status;
                waitpid(pid1,&status,0);
                waitpid(pid2,&status,0);
            }
        }
        merge(arr,l,m,r);
        return;
    }
}

struct arg{
    int l,r;
    int* arr;
};

void *threaded_mergesort(void* arr){
    struct arg *args = (struct arg*) arr;
    int l = args->l;
    int r = args->r;
    int *a = args->arr;
   if(l<r)
   {
       //implementing merge sort by sorting two halves of array by making two corresponding threads
       if((r-l+1)<5){
            //perform selection sort for no. of elements less than 5
            for(ll i=l;i<r;i++)
            {
                int minind = i;
                for(ll j=i+1;j<=r;j++)
                    if(a[j]< a[minind])
                        minind = j;
                swap(&a[i],&a[minind]);
            }
            return NULL;    
       }
       int m = l+(r-l)/2;
       //sorting left half of array
       struct arg leftarr;
       leftarr.l = l;
       leftarr.r = m;
       leftarr.arr = a;
       pthread_t lefttid;
       pthread_create(&lefttid,NULL,threaded_mergesort,&leftarr);

       //sorting right half of array
       struct arg rightarr;
       rightarr.l = m+1;
       rightarr.r = r;
       rightarr.arr = a;
       pthread_t righttid;
       pthread_create(&righttid,NULL,threaded_mergesort,&rightarr);

       //waiting for two threads to end
       pthread_join(lefttid,NULL);
       pthread_join(righttid,NULL);

       merge(a,l,m,r);
   }

}
void runsorts(ll n)
{
    struct timespec ts;

    // to get share memory
    int *arr_conc = shareMem(sizeof(int)*(n+1));
    for(ll i=0;i<n;i++) scanf("%d",arr_conc+i);
    int arr_norm[n+1],arr_thread[n+1];
    for(ll i=0;i<n;i++) { arr_norm[i] = arr_conc[i]; arr_thread[i]=arr_conc[i];} 

    // printf("The given array is: ");
    // printarr(arr_conc,n);

    clock_gettime(CLOCK_MONOTONIC_RAW,&ts);    //fetching start time before running normal mergesort
    long double startt = ts.tv_nsec/(1e9)+ts.tv_sec;
    //running normal merge sort
    normal_mergesort(arr_norm,0,n-1);
    printf("\nAfter running normal mergesort:\n");
    printarr(arr_norm,n);
    clock_gettime(CLOCK_MONOTONIC_RAW,&ts);     //fetching end time after running normal mergesort
    long double endt = ts.tv_nsec/(1e9)+ts.tv_sec;
    long double nor_time = endt - startt;
    printf("time taken by normal mergesort = %Lf\n\n",nor_time);             


    clock_gettime(CLOCK_MONOTONIC_RAW,&ts);
    startt = ts.tv_nsec/(1e9)+ts.tv_sec;   
    //running concurrent merge sort
    concurrent_mergesort(arr_conc,0,n-1);
    printf("After running concurrent mergesort:\n");
    printarr(arr_conc,n);
    clock_gettime(CLOCK_MONOTONIC_RAW,&ts);
    endt = ts.tv_nsec/(1e9)+ts.tv_sec;
    long double con_time = endt - startt;
    printf("time taken by concurrent mergesort = %Lf\n\n",con_time);             

    
    clock_gettime(CLOCK_MONOTONIC_RAW,&ts);
    startt = ts.tv_nsec/(1e9)+ts.tv_sec; 
    //running threaded merge sort
    pthread_t tid;
    struct arg array;
    array.l =0; array.r = n-1; array.arr=arr_thread; 
    pthread_create(&tid,NULL,threaded_mergesort,&array);
    pthread_join(tid,NULL);
    printf("After running threaded mergesort:\n");
    printarr(array.arr,n);
    clock_gettime(CLOCK_MONOTONIC_RAW,&ts);
    endt = ts.tv_nsec/(1e9)+ts.tv_sec;
    long double thrd_time = endt - startt;
    printf("time taken by threaded mergesort = %Lf\n\n",thrd_time);
    
    printf("normal_mergesort ran:\n\t[ %Lf ] times faster than concurrent_mergesort\n\t[ %Lf ] times faster than threaded_concurrent_mergesort\n\n", con_time/nor_time, thrd_time/nor_time);
    shmdt(arr_conc);

    return;
}

int main()
{
    ll n;
    scanf("%lld",&n);
    runsorts(n);     // to run three types of sorts
    return 0;
}