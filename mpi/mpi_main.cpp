#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <string.h>
void fill_global_array(int *global_array_ptr, int size_of_array)
{

    for(int i=0; i<size_of_array; i++)
    {
        global_array_ptr[i]=i;
    }
    for(int i=size_of_array-1;i>=0;i--)
    {
        int switch_index=rand()%size_of_array;
        int temp=global_array_ptr[i];
        global_array_ptr[i]=global_array_ptr[switch_index];
        global_array_ptr[switch_index]=temp;
    }
}
int main(int argc, char** argv) {
    srand(time(NULL));
    int size_of_array=atoi(argv[1]);
    int *local_array_ptr = NULL;
    int *global_array_ptr = NULL;
    int query_index;// Used for checking the correctness of the search
    int query_element;// To be shared for parallel search
    int rank, total_number_of_processes;
    int local_start_index, local_end_index;
    MPI_Comm global_channel=MPI_COMM_WORLD;
    MPI_Request request, request2;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(global_channel, &rank);
    MPI_Comm_size(global_channel, &total_number_of_processes);
    // Setting up the problem
    local_start_index=(size_of_array/total_number_of_processes)*rank;
    local_end_index=(size_of_array/total_number_of_processes)*(rank+1)-1;
    double start_time, end_time;
    if(rank==total_number_of_processes-1)
    {
        local_end_index=size_of_array-1;
    }
    if(rank==0)
    {
        global_array_ptr=(int*)malloc(size_of_array*sizeof(int));
        fill_global_array(global_array_ptr, size_of_array);
        // for(int i=0;i<size_of_array;i++)
        // {
        //     printf("%d |", global_array_ptr[i]);
        // }
        query_index=rand()%size_of_array;
        query_element=global_array_ptr[query_index];
        // printf("\nQuery element: %d\n", query_element);
        // printf("Query index: %d\n", query_index);
        printf("Number of processes:%d\n", total_number_of_processes);
        printf("Length of array:%d\n", size_of_array);
        start_time=MPI_Wtime();
    }


    MPI_Bcast(&query_element, 1, MPI_INT, 0, global_channel);
    local_array_ptr=(int*)malloc((local_end_index-local_start_index+1)*sizeof(int));
    int *send_count_arr = NULL;
    int *start_index_arr = NULL;

    if (rank == 0) {
        send_count_arr = (int*)malloc(total_number_of_processes * sizeof(int));
        start_index_arr = (int*)malloc(total_number_of_processes * sizeof(int));
        for (int i = 0; i < total_number_of_processes; i++) {
            send_count_arr[i] = (size_of_array / total_number_of_processes);
            if (i == total_number_of_processes - 1) {
                send_count_arr[i] += size_of_array % total_number_of_processes;
            }
            start_index_arr[i] = (size_of_array / total_number_of_processes) * i;
        }
    }

    MPI_Scatterv(global_array_ptr, send_count_arr, start_index_arr, MPI_INT, local_array_ptr, local_end_index - local_start_index + 1, MPI_INT, 0, global_channel);

    if (rank == 0) {
        free(send_count_arr);
        free(start_index_arr);
    }
    // Search phase
    int send_index=-1;
    int recv_index=-1;
    int found_status=0;
    MPI_Irecv(&recv_index, 1, MPI_INT, MPI_ANY_SOURCE, 0, global_channel, &request);
    for(int local_index=0;local_index<local_end_index-local_start_index+1;local_index++)
    {
        if(local_array_ptr[local_index]==query_element)
        {
            send_index=local_index+local_start_index;
            break;
        }
        MPI_Test(&request, &found_status, MPI_STATUS_IGNORE);
        if(found_status)
        {
            break;
        }
    }
    if(rank==0)
    {
        if(send_index!=-1) // Root found the element
        {
            MPI_Cancel(&request);
            for(int send_rank=1;send_rank<total_number_of_processes;send_rank++)
            {
                MPI_Send(&send_index, 1, MPI_INT, send_rank, 0, global_channel);//Tell the other processes to stop searching
            }
            recv_index=send_index;
        }
        else
        {
            MPI_Wait(&request, MPI_STATUS_IGNORE);// Wait for the message to be received
        }
        if(recv_index!=query_index)
        {
            printf("Error: Query index does not match the received index %d\n", recv_index);// To simulataneously check correctness
        }
        else
        {
            printf("Query index matches the received index %d\n", recv_index); 
            end_time=MPI_Wtime();
            printf("Time taken:%f\n", end_time-start_time);
        }
    }
    else if(send_index!=-1)
    {
        MPI_Cancel(&request);
        MPI_Isend(&send_index, 1, MPI_INT, 0, 0, global_channel, &request2);
        for(int send_rank=1;send_rank<total_number_of_processes;send_rank++)
        {
            if(send_rank!=rank)
            {
                MPI_Send(&send_index, 1, MPI_INT, send_rank, 0, global_channel);//Tell the other process to stop searching
            }
        }
        MPI_Wait(&request2, MPI_STATUS_IGNORE);
    }
    MPI_Finalize();
}
