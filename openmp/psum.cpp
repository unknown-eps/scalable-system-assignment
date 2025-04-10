#include <iostream>
#include <omp.h>
#include <chrono>
#include <stdio.h>
using namespace std;

void fill_array_random(int arr[],int n)
{
    for(int i=0;i<n;i++)
    {
        arr[i]=rand() % 1000;
    }
    return;
}
int main(int argc,char *argv[]) // Num of Processors and length of input array as arguments respectively.
{
    int num_of_threads_to_run=stoi(argv[1]);
    int length_of_array_to_create=stoi(argv[2]);
    // int arr[length_of_array_to_create];
    int *arr=new int[length_of_array_to_create];
    fill_array_random(arr,length_of_array_to_create);
    // for(int i=0;i<length_of_array_to_create;i++)
    // {
    //     cout<<arr[i]<<" ";
    // }
    // int sequential_psum[length_of_array_to_create];
    int *sequential_psum=new int[length_of_array_to_create];
    int *parallel_psum=new int[length_of_array_to_create];
    int parallel_communicate[num_of_threads_to_run];
    omp_set_num_threads(num_of_threads_to_run);
    auto sequential_start=std::chrono::high_resolution_clock::now();
    sequential_psum[0]=arr[0];
    for(int i=1;i<length_of_array_to_create;i++)
    {
        sequential_psum[i]=sequential_psum[i-1]+arr[i];
    }
    auto sequential_end=std::chrono::high_resolution_clock::now();
    auto sequential_time_taken=(std::chrono::duration_cast<std::chrono::microseconds>(sequential_end-sequential_start)).count();
    // Beginning the parallel part of the code.
    // int parallel_psum[length_of_array_to_create];
    auto parallel_start=std::chrono::high_resolution_clock::now();
    #pragma omp parallel default(none) shared(num_of_threads_to_run,length_of_array_to_create,arr,parallel_psum,parallel_communicate) proc_bind(close)
    {
        int thread_id = omp_get_thread_num();
        int chunk_size=(length_of_array_to_create)/num_of_threads_to_run; // ceil of length_of_array_to_create/num_of_threads_to_run
        int first=thread_id*chunk_size;
        int last=first+chunk_size-1;
        // if the number of elemnets is not divisible by the number of threads assign the last thread has extra elements.
        if(thread_id==num_of_threads_to_run-1) 
        {
            last=length_of_array_to_create-1;
        }
        int thread_local_sum=0;
        for(int i=first;i<=last;i++)
        {
            thread_local_sum+=arr[i];
        }
        parallel_communicate[thread_id]=thread_local_sum;
        #pragma omp barrier // Wait for each thread has computed its local sum.
        #pragma omp master
        {
            for(int i=1;i<num_of_threads_to_run;i++)
            {
                parallel_communicate[i]+=parallel_communicate[i-1];
            }
        }
        #pragma omp barrier // Threads are running ahead if this barrier is not present.
        int thread_extra=parallel_communicate[thread_id]-thread_local_sum;
        parallel_psum[first]=thread_extra+arr[first];
        for(int i=first+1;i<=last;i++)
        {
            parallel_psum[i]=parallel_psum[i-1]+arr[i];
        }
    }
    auto parallel_end=std::chrono::high_resolution_clock::now();
    auto parallel_time_taken=(std::chrono::duration_cast<std::chrono::microseconds>(parallel_end-parallel_start)).count();
    bool correct=true;
    for(int i=0;i<length_of_array_to_create;i++)
    {
        if(sequential_psum[i]!=parallel_psum[i])
        {
            correct=false;
            break;
        }
    }
    if(correct==false)
    {
        cout<<"The parallel and sequential results are not the same. Make sure the size of the array is greater than equal to the number of processors."<<endl;
        return 0;
    }
    cout<<"Number of threads:"<<num_of_threads_to_run<<endl;
    cout<<"Length of the array:"<<length_of_array_to_create<<endl;
    cout<<"Sequential execution time in microseconds:"<<sequential_time_taken<<endl;
    cout<<"Parallel execution time in microseconds:"<<parallel_time_taken<<endl;
    return 0;
}