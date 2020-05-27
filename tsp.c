#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#define CITIES 5
#define INF 99

int travel_path[CITIES] = {0}, pos=0;
int been_there[CITIES] = {0};
int cost_matrix[CITIES][CITIES] = {
    {INF, 20, 30, 10, 11},
    {15, INF, 16, 4, 2},
    {3, 5, INF, 2, 4},
    {19, 6, 18, INF, 3},
    {16, 4, 7, 16, INF}
};

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

    printf("\n------------------\n");
    printf("\nEntering func with: \n");
    printf("total_cost: %d\n", total_cost);
    printf("where_am_i: %d\n", where_am_i);
    printf("matrix: \n");
    for(int k=0; k<CITIES; k++){
        for(int f=0; f<CITIES; f++){
            if(matrix[k][f] == INF)
                printf("  ∞ ");
            else
                printf("%3d ", matrix[k][f]);
        }
        printf("\n");
    }
    printf("\n------------------\n\n");

    travel_path[pos] = where_am_i+1;
    pos++;

    for(int i=0; i<CITIES; i++){
        if(been_there[i] != 1){
            recurtion_times++;
            //reconstruct array
            for(int k=0; k<CITIES; k++){
                for(int l=0; l<CITIES; l++){
                    if(k == where_am_i || l == i || (l == where_am_i && k == i) )
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
            
            printf("travel_cost: %d\tC(%d, %d): %d\n", travel_cost, where_am_i, i, matrix[where_am_i][i]);
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
            printf("LCB: %d at node: %d\n", least_cost_bound, keep_node);
        }
        travel_cost = 0;
    }
    printf("\n\n");
    for(int k=0; k<CITIES; k++){
        for(int f=0; f<CITIES; f++){
            if(keep_matrix[k][f] == INF)
                printf("  ∞ ");
            else
                printf("%3d ", keep_matrix[k][f]);
        }
        printf("\n");
    }
    if(recurtion_times < 2){
        travel_path[pos] = keep_node+1;
        return least_cost_bound;
    }else{
        // printf("total_cost recurive: %d\n", total_cost);
        total_cost = branch_and_bound(keep_matrix, least_cost_bound, keep_node);
    }
}

int main(int argc, char const *argv[]){ 
    int reduced_matrix[CITIES][CITIES];
    int col[CITIES], row[CITIES];
    int total_cost = 0;

    struct timespec start, end;
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
            if(cost_matrix[i][k] != INF)
                reduced_matrix[i][k] = cost_matrix[i][k] - row[i];
            else
                reduced_matrix[i][k] = cost_matrix[i][k];
            printf("%3d ", reduced_matrix[i][k]);
        }
        printf("\n");
    }

    printf("\n");
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
            }else
                reduced_matrix[i][k] = reduced_matrix[i][k];
            printf("%3d ", reduced_matrix[i][k]);
        }
        printf("\n");
    }

    //logging results
    printf("\nrow: ");
    for (int i = 0; i < CITIES; i++){
        printf("%d ", row[i]);
    }
    printf("\ncol: ");
    for (int i = 0; i < CITIES; i++){
        printf("%d ", col[i]);
    }
    
    printf("\n%d\n", total_cost);

    // been_there[0] = 1;
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
    printf("time taken to calculate sortest path recurively: %f sec\n", time_lapsed);
    
    return 0;
}
