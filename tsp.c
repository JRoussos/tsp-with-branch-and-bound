#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define CITIES 5
#define INF 99

struct thread_args{
    int matrix[CITIES][CITIES];
    int total_cost;
    int where_am_i;
    int least_cost_bound;
    int recurtion_times;
    int thread_id;
};

struct thread_out{
    int new_matrix[CITIES][CITIES];
    int lcb;
};

int travel_path[CITIES] = {0}, pos=0;
int been_there[CITIES] = {0};
int cost_matrix[CITIES][CITIES] = {
    {INF, 20, 30, 10, 11},
    {15, INF, 16, 4, 2},
    {3, 5, INF, 2, 4},
    {19, 6, 18, INF, 3},
    {16, 4, 7, 16, INF}
};

//another matrix for validation of the algorithm. 
//the cost should be 34 and the path 1-> 3 -> 4 -> 2 -> 5 -> 1
// int cost_matrix[CITIES][CITIES] = {
//     {INF, 10, 8, 9, 7},
//     {10, INF, 10, 5, 6},
//     {8, 10, INF, 8, 9},
//     {9, 5, 8, INF, 6},
//     {7, 6, 9, 6, INF}
// };

int branch_and_bound(int matrix[CITIES][CITIES], int total_cost, int where_am_i){
    /** A row <----- INF
     *  B col <----- INF
     *  C(B, A) <--- INF 
     * 
     * LCB(1)+C(1,2)+C = 25+10+0
     */
    int least_cost_bound = INF;
    int travel_cost = 0;
    int new_matrix[CITIES][CITIES];
    int col[CITIES], row[CITIES];
    int keep_node;
    int keep_matrix[CITIES][CITIES];
    been_there[where_am_i] = 1;
    int recurtion_times=0;

    // printf("\n------------------\n");
    // printf("\nEntering func with: \n");
    // printf("total_cost: %d\n", total_cost);
    // printf("where_am_i: %d\n", where_am_i);
    // printf("matrix: \n");
    // for(int k=0; k<CITIES; k++){
    //     for(int f=0; f<CITIES; f++){
    //         if(matrix[k][f] == INF)
    //             printf("  ∞ ");
    //         else
    //             printf("%3d ", matrix[k][f]);
    //     }
    //     printf("\n");
    // }
    // printf("\n------------------\n\n");

    travel_path[pos] = where_am_i+1;
    pos++;

    // printf("been there: ");
    // for(int i=0; i<CITIES; i++){
    //     printf("%d ", been_there[i]);
    // }
    // printf("\n");

    for(int i=0; i<CITIES; i++){
        if(been_there[i] != 1){
            recurtion_times++;
            //reconstruct array
            for(int k=0; k<CITIES; k++){
                for(int l=0; l<CITIES; l++){
                    if(k == where_am_i || l == i || (l == where_am_i && k == 0) ) //if(k == where_am_i || l == i || (l == where_am_i && k == i) )
                        new_matrix[k][l] = INF;
                    else
                        new_matrix[k][l] = matrix[k][l];
                }
            }

            ///////////////
            for(int k=0; k<CITIES; k++){
                row[k] = new_matrix[k][0];
                for(int l=0; l<CITIES; l++){
                    if(new_matrix[k][l] != INF){
                        if(new_matrix[k][l] < row[k])
                            row[k] = new_matrix[k][l];
                    }

                }
                if(row[k] == INF)
                    row[k] = 0;
                travel_cost += row[k];
            }

            if(travel_cost > 0){
                for(int k=0; k<CITIES; k++){
                    for(int l=0; l<CITIES; l++){
                        if(new_matrix[k][l] != INF)
                            new_matrix[k][l] = new_matrix[k][l] - row[k];
                        else
                            new_matrix[k][l] = new_matrix[k][l];
                        // printf("%3d ", new_matrix[k][l]);
                    }
                    // printf("\n");
                }
            }
            // printf("\n");
            //finding the minimum of every column and storing it on the col array
            for(int k=0; k<CITIES; k++){
                col[k] = new_matrix[0][k];
                for(int l=0; l<CITIES; l++){
                    if(new_matrix[l][k] != INF){
                        if(new_matrix[l][k] < col[k])
                            col[k] = new_matrix[l][k];
                    }
                }
                if(col[k] == INF)
                    col[k] = 0;
                travel_cost += col[k];
            }

            //making the reduced array subtracting the lowest value of every column 
            if(travel_cost > 0){ 
                for(int k=0; k<CITIES; k++){
                    for(int l=0; l<CITIES; l++){
                        // printf("(%d)", reduced_matrix[i][k]);
                        if(new_matrix[k][l] != INF){
                            new_matrix[k][l] = new_matrix[k][l] - col[l];
                        }else
                            new_matrix[k][l] = new_matrix[k][l];
                        // printf("%3d ", new_matrix[k][l]);
                    }
                    // printf("\n");
                }
            }
            ///////////////
            
            // printf("travel_cost: %d\tC(%d, %d): %d\n", travel_cost, where_am_i, i, matrix[where_am_i][i]);
            if(least_cost_bound > total_cost + matrix[where_am_i][i] + travel_cost){
                least_cost_bound = total_cost + matrix[where_am_i][i] + travel_cost;
                //keep position in order to know which node has the lowest LCB
                keep_node = i;
                for(int k=0; k<CITIES; k++){
                    for(int l=0; l<CITIES; l++){
                        keep_matrix[k][l] = new_matrix[k][l];
                    }
                }
            }
            // printf("LCB: %d at node: %d\n", least_cost_bound, keep_node);
        }
        travel_cost = 0;
    }
    // printf("\n\n");
    // for(int k=0; k<CITIES; k++){
    //     for(int f=0; f<CITIES; f++){
    //         if(keep_matrix[k][f] == INF)
    //             printf("  ∞ ");
    //         else
    //             printf("%3d ", keep_matrix[k][f]);
    //     }
    //     printf("\n");
    // }
    if(recurtion_times < 2){
        travel_path[pos] = keep_node+1;
        return least_cost_bound;
    }else{
        // printf("total_cost recurive: %d\n", total_cost);
        total_cost = branch_and_bound(keep_matrix, least_cost_bound, keep_node);
    }
}

void *thread_function(void *arguments){
    struct thread_args *args = arguments;
    struct thread_out *out = malloc(sizeof(struct thread_out));

    int new_matrix[CITIES][CITIES];
    int col[CITIES], row[CITIES];
    int travel_cost = 0;
    int keep_node;
    int keep_matrix[CITIES][CITIES];
    int lcb = args->least_cost_bound;

    // printf("least_cost_bound: %d\n", args->least_cost_bound);

    //reconstruct array
    for(int k=0; k<CITIES; k++){
        for(int l=0; l<CITIES; l++){
            if(k == args->where_am_i || l == args->thread_id || (l == args->where_am_i && k == args->thread_id) )
                new_matrix[k][l] = INF;
            else
                new_matrix[k][l] = args->matrix[k][l];
        }
    }

    //print new_matrix array
    // for(int k=0; k<CITIES; k++){
    //     for(int f=0; f<CITIES; f++){
    //         if(new_matrix[k][f] == INF)
    //             printf("  ∞ ");
    //         else
    //             printf("%3d ", new_matrix[k][f]);
    //     }
    //     printf("\n");
    // }

    ///////////////
    for(int k=0; k<CITIES; k++){
        row[k] = new_matrix[k][0];
        for(int l=0; l<CITIES; l++){
            if(new_matrix[k][l] != INF){
                if(new_matrix[k][l] < row[k])
                    row[k] = new_matrix[k][l];
            }

        }
        if(row[k] == INF)
            row[k] = 0;
        travel_cost += row[k];
    }

    // printf("travel_cost: %d\n", travel_cost);
    // printf("row: \n");
    // for(int i=0; i<CITIES; i++){
    //     printf("%d ", row[i]);
    // }
    // printf("\n");

    if(travel_cost > 0){
        for(int k=0; k<CITIES; k++){
            for(int l=0; l<CITIES; l++){
                if(new_matrix[k][l] != INF)
                    new_matrix[k][l] = new_matrix[k][l] - row[k];
                else
                    new_matrix[k][l] = new_matrix[k][l];
                // printf("%3d ", new_matrix[k][l]);
            }
            // printf("\n");
        }
    }
    // printf("\n");
    //finding the minimum of every column and storing it on the col array
    for(int k=0; k<CITIES; k++){
        col[k] = new_matrix[0][k];
        for(int l=0; l<CITIES; l++){
            if(new_matrix[l][k] != INF){
                if(new_matrix[l][k] < col[k])
                    col[k] = new_matrix[l][k];
            }
        }
        if(col[k] == INF)
            col[k] = 0;
        travel_cost += col[k];
    }

    
    // printf("travel_cost: %d\n", travel_cost);
    // printf("col: \n");
    // for(int i=0; i<CITIES; i++){
    //     printf("%d ", col[i]);
    // }
    // printf("\n");

    //making the reduced array subtracting the lowest value of every column 
    if(travel_cost > 0){ 
        for(int k=0; k<CITIES; k++){
            for(int l=0; l<CITIES; l++){
                // printf("(%d)", reduced_matrix[i][k]);
                if(new_matrix[k][l] != INF){
                    new_matrix[k][l] = new_matrix[k][l] - col[l];
                }else
                    new_matrix[k][l] = new_matrix[k][l];
                // printf("%3d ", new_matrix[k][l]);
            }
            // printf("\n");
        }
    }
    ///////////////
    
    // printf("total_cost: %d\tC(%d, %d): %d\ttravel_cost: %d\n", args->total_cost, args->where_am_i, args->thread_id, args->matrix[args->where_am_i][args->thread_id], travel_cost);
    if(args->least_cost_bound > args->total_cost + args->matrix[args->where_am_i][args->thread_id] + travel_cost){
        lcb = args->total_cost + args->matrix[args->where_am_i][args->thread_id] + travel_cost;
        //keep position in order to know which node has the lowest LCB
        // keep_node = args->thread_id;
        // for(int k=0; k<CITIES; k++){
        //     for(int l=0; l<CITIES; l++){
        //         keep_matrix[k][l] = new_matrix[k][l];
        //     }
        // }
    }
    // printf("LCB: %d\n", lcb);
    out->lcb = lcb;
    for(int k=0; k<CITIES; k++){
        for(int l=0; l<CITIES; l++){
            out->new_matrix[k][l] = new_matrix[k][l];
        }
    }

    pthread_exit(out);
}

int do_it_with_threads(int number_of_threads, int matrix[CITIES][CITIES], int total_cost, int where_am_i){
    pthread_t threads[number_of_threads];
    // struct timespec end;
    struct thread_args t_arg;

    t_arg.total_cost = total_cost;
    t_arg.where_am_i = where_am_i;
    t_arg.recurtion_times = 0;
    t_arg.least_cost_bound = INF;

    for(int i=0; i<CITIES; i++){
        for(int k=0; k<CITIES; k++){
            t_arg.matrix[i][k] = matrix[i][k];
        }
    }

    int least_cost_bound = INF;
    int keep_node;
    int keep_matrix[CITIES][CITIES];
    been_there[where_am_i] = 1;
    int recurtion_times=0;

    // void *thread_result;
    struct thread_out *t_out;

    travel_path[pos] = where_am_i+1;
    pos++;

    // printf("been there: ");
    // for(int i=0; i<CITIES; i++){
    //     printf("%d ", been_there[i]);
    // }
    // printf("\n");

    for(int i=0; i<CITIES; i++){
        if(been_there[i] != 1){
            recurtion_times++;
            t_arg.thread_id = i;
            if (pthread_create(&threads[i], NULL, thread_function, &t_arg) != 0 ){
                perror("ERROR: Thread Creation\n");
                exit(EXIT_FAILURE);
            }
            if (pthread_join(threads[i], (void*)&t_out) != 0 ){
                perror("ERROR: Thread Join\n");
                exit(EXIT_FAILURE);
            }
            // printf("thread %d said: %d\n\n",i+1, t_out->lcb);
            if(t_out->lcb < least_cost_bound){
                least_cost_bound = t_out->lcb;
                keep_node = i;
                for(int k=0; k<CITIES; k++){
                    for(int l=0; l<CITIES; l++){
                        keep_matrix[k][l] = t_out->new_matrix[k][l];
                        // printf("%d ", keep_matrix[k][l]);
                    }
                    // printf("\n");
                }
            }
            free(t_out);
        }
    }

    // printf("\n---------------------\n");

    //make here some kind of logic for exiting
    if(recurtion_times < 2){
        travel_path[pos] = keep_node+1;
        printf("\ntraveled path: ");
        for(int i=0; i<CITIES; i++){
            printf("%d -> ", travel_path[i]);
        }
        //print the starting node, we are starting from node 1
        printf("1");
        return least_cost_bound;
    }else{
        // printf("keep node %d\n", keep_node);
        do_it_with_threads(number_of_threads-1, keep_matrix, least_cost_bound, keep_node);
    }

}

int main(int argc, char const *argv[]){ 
    int reduced_matrix[CITIES][CITIES];
    int col[CITIES], row[CITIES];
    int total_cost = 0;

    struct timespec start, end;

    //print the initial matrix
    printf("\nThe cost matrix is the following:\n\n");
    for(int i=0; i<CITIES; i++){
        for(int k=0; k<CITIES; k++){
            if(cost_matrix[i][k] != INF)
                printf("%3d ", cost_matrix[i][k]);
            else
                printf("  ∞ ");
        }
        printf("\n");
    }

    clock_gettime(CLOCK_MONOTONIC, &start); //start the clock 

    //finding the minimum of every row and storing it on the row array
    for(int i=0; i<CITIES; i++){
        row[i] = cost_matrix[i][0];
        // printf("row: %d ", row[i]);
        for(int k=0; k<CITIES; k++){
            if(cost_matrix[i][k] != INF){
                if(cost_matrix[i][k] < row[i]){
                    row[i] = cost_matrix[i][k];
                }
            }
        }
        // printf("\n");
        total_cost += row[i];
    }

    //making the reduced array subtracting the lowest value of every row  
    for(int i=0; i<CITIES; i++){
        for(int k=0; k<CITIES; k++){
            if(cost_matrix[i][k] != INF){
                reduced_matrix[i][k] = cost_matrix[i][k] - row[i];
                // printf("%3d ", reduced_matrix[i][k]);
            }
            else{
                reduced_matrix[i][k] = cost_matrix[i][k];
                // printf("  ∞ ");
            }
        }
        // printf("\n");
    }

    // printf("\n");
    //finding the minimum of every column and storing it on the col array
    for(int i=0; i<CITIES; i++){
        col[i] = reduced_matrix[0][i];
        // printf("col: %d ", col[i]);
        for(int k=0; k<CITIES; k++){
            if(reduced_matrix[k][i] != INF){
                if(reduced_matrix[k][i] < col[i]){
                    col[i] = reduced_matrix[k][i];
                }
            }
        }   
        // printf("\n");
        total_cost += col[i];
    }
    
    //making the reduced array subtracting the lowest value of every column  
    for(int i=0; i<CITIES; i++){
        for(int k=0; k<CITIES; k++){
            // printf("(%d)", reduced_matrix[i][k]);
            if(reduced_matrix[i][k] != INF){
                reduced_matrix[i][k] = reduced_matrix[i][k] - col[k];
                // printf("%3d ", reduced_matrix[i][k]);
            }else{
                reduced_matrix[i][k] = reduced_matrix[i][k];
                // printf("  ∞ ");
            }
        }
        // printf("\n");
    }

    //logging results
    // printf("\nrow: ");
    // for (int i = 0; i < CITIES; i++){
    //     printf("%d ", row[i]);
    // }
    // printf("\ncol: ");
    // for (int i = 0; i < CITIES; i++){
    //     printf("%d ", col[i]);
    // }
    
    // printf("\n%d\n", total_cost);

    // been_there[0] = 1;
    int keep_total_cost_for_threads = total_cost;
    total_cost = branch_and_bound(reduced_matrix, total_cost, 0);
    printf("\ntraveled path: ");
    for(int i=0; i<CITIES; i++){
        printf("%d -> ", travel_path[i]);
    }
    //print the starting node, we are starting from node 1
    printf("1");
    printf("\ntotal travel cost: %d\n", total_cost);
    
    clock_gettime(CLOCK_MONOTONIC, &end); //stop the clock 
    //calculating the time it took to find the path and min cost recursively
    double time_lapsed = (end.tv_sec - start.tv_sec) * 1000000000;
    time_lapsed = (time_lapsed + (end.tv_nsec - start.tv_nsec)) / 1000000000;
    printf("time taken to calculate sortest path recurively: %f sec\n\n", time_lapsed);

    pos = 0; //zeroing the position to start counting again with the threads 
    memset(been_there, 0, sizeof(been_there)); //zeroing been_there array
    clock_gettime(CLOCK_MONOTONIC, &start); //start the clock again for threads
    total_cost = do_it_with_threads(CITIES-1, reduced_matrix, keep_total_cost_for_threads, 0);

    printf("\ntotal travel cost with threads: %d\n", total_cost);

    clock_gettime(CLOCK_MONOTONIC, &end); //stop the clock 
    // calculating the time it took to find the path and min cost recursively
    time_lapsed = (end.tv_sec - start.tv_sec) * 1000000000;
    time_lapsed = (time_lapsed + (end.tv_nsec - start.tv_nsec)) / 1000000000;
    printf("time taken to calculate sortest path with %d threads: %f sec\n", CITIES-1, time_lapsed);
    
    return 0;
}
