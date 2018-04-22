#include<stdio.h>
#include<stdlib.h>
#include "mpi.h"
#include<stddef.h>
#include<math.h>

typedef struct Ponto{
    double x;
    double y;
    double lastDistance;
    int id_cluster;
} Ponto;

typedef struct Cluster{
    double x_centroid;
    double y_centroid;
    int id_cluster;
} Cluster;

//Função que inicializa os data types ponto e cluster
void initTypes(MPI_Datatype* pontoType, MPI_Datatype* clusterType){
    int blockLenghtsPonto[3] = {1,1,1};
    int blockLenghtsCluster[3] = {1,1,1};
    MPI_Datatype typesPonto[4] = {MPI_INT,MPI_DOUBLE,MPI_DOUBLE,MPI_DOUBLE};
    MPI_Datatype typesCluster[3] = {MPI_INT,MPI_DOUBLE,MPI_DOUBLE};
    MPI_Aint dispPonto[4] = {offsetof(Ponto,id_cluster),offsetof(Ponto,x),offsetof(Ponto,y),offsetof(Ponto,lastDistance)};
    MPI_Aint dispCluster[3] = {offsetof(Cluster,id_cluster), offsetof(Cluster,x_centroid),offsetof(Cluster,y_centroid)};
    MPI_Type_create_struct( 4, blockLenghtsPonto, dispPonto, typesPonto, pontoType );
    MPI_Type_create_struct( 3, blockLenghtsCluster, dispCluster, typesCluster, clusterType );
    MPI_Type_commit(pontoType);
    MPI_Type_commit(clusterType);
}


double euclidian_dist(double point_x, double point_y, double centr_x, double centr_y) {
    return sqrt(pow((point_x - centr_x),2) + pow((point_y - centr_y),2));
}

void assign_point(double point[8][4], int point_id, double cluster[2][2], int cluster_id) {
    int itr_point = 0;
    double old_ecl_dist = 0.0;
    double new_ecl_dist = 0.0;
    old_ecl_dist = point[point_id][2];
    new_ecl_dist = euclidian_dist(point[point_id][0], point[point_id][1], cluster[cluster_id][0], cluster[cluster_id][1]);
    
    if(new_ecl_dist < old_ecl_dist){
        point[point_id][2] = new_ecl_dist;
        
        if(point[point_id][3] != 99.0){
            printf("Removed from cluster %f\n", (int)point[point_id][3]);
            point[point_id][3] = 99.0;
        }
        
        point[point_id][3] = (double)cluster_id;
        printf("Added in cluster %d\n", cluster_id);
    }
}

void update_centroids(int offset, int chunk_size, double points[8][4], double cluster[2][2], int cluster_id) {
    int itr_points = 0;
    double sum_x, sum_y = 0.0;
    double n_cluster_pts = 0.0;
    double new_ctr_x, new_ctr_y = 0.0;

    sum_x = 0.0;
    sum_y = 0.0;
    new_ctr_x = 0.0;
    new_ctr_y = 0.0;
    n_cluster_pts = 0.0;
    
        for(itr_points = offset; itr_points < chunk_size + offset; itr_points++){
            
            if(points[itr_points][3] == cluster_id){
                n_cluster_pts++;
                sum_x += points[itr_points][0];
                sum_y += points[itr_points][1];
            }
        }
    new_ctr_x = sum_x / n_cluster_pts;
    new_ctr_y = sum_y / n_cluster_pts;

    if(new_ctr_x != cluster[cluster_id][0] || new_ctr_y != cluster[cluster_id][1]){
        cluster[cluster_id][0] = new_ctr_x;
        cluster[cluster_id][1] = new_ctr_y;
        //printf("Centroid from cluster %d changed!\n", cluster_id);
    }

    else {
        //printf("Centroid from cluster %d stayed the same...\n", cluster_id);
    }
}

void print_clusters(double point[8][4], double cluster[2][2]){
    int itr_points = 0;
    int itr_clusters = 0;
    
    for (itr_clusters = 0; itr_clusters < 2; itr_clusters++){
        printf("Elements from cluster %d:\n", itr_clusters);
        for(itr_points = 0; itr_points < 8; itr_points++){
            if(point[itr_points][3] == itr_clusters){
                printf("( %f, %f ) ", point[itr_points][0], point[itr_points][1]);
            }
        }
        itr_points = 0;
        printf("\n");
    }
}

int main (int argc, char *argv[]) {
    int num_tasks, task_id, dest, source, offset, tag_1, tag_2, chunk_size;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    
    //A partir de agora pode utilizar os tipos MPI_PONTOTYPE e MPI_CLUSTERTYPE para trocar informações desse tipo
    MPI_Datatype MPI_PONTOTYPE, MPI_CLUSTERTYPE;
    initTypes(&MPI_PONTOTYPE, &MPI_CLUSTERTYPE);
    /*
    check if array can be divided
    by the number of tasks proposed
    */

    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    //Message tags, tag 1 for offset and tag 2 for data_chunk
    tag_1 = 1;
    tag_2 = 2;

    int cluster_t_size = 2;
    int points_t_size = 8;

    chunk_size = (points_t_size / num_tasks);
 
     double points[8][4] = {{1.0, 1.0, 99.0, 99.0},
                         {1.5, 2.0, 99.0, 99.0},
                         {3.0, 4.0, 99.0, 99.0},
                         {5.0, 7.0, 99.0, 99.0},
                         {3.5, 5.0, 99.0, 99.0},
                         {4.5, 5.0, 99.0, 99.0},
                         {3.5, 4.5, 99.0, 99.0},
                         {2.5, 1.5, 99.0, 99.0}};

    double clusters_centroids[2][2] = {{0.0, 0.0},
                                    {7.5, 7.5}};

    // ============== MASTER =========================
    if(task_id == 0){

        offset = chunk_size;

        //send each process its chunk
        for (dest=1; dest < num_tasks; dest++){
            MPI_Send(&offset, 1, MPI_INT, dest, tag_1, MPI_COMM_WORLD);
            MPI_Send(&points[offset], chunk_size, MPI_DOUBLE, dest, tag_2, MPI_COMM_WORLD);
            offset = offset + chunk_size;
        }

        offset = 0;

        //printf("process id %d offset %d\n", task_id, offset);

        int itr_p;
        int itr_c;
        for(itr_c = 0; itr_c < cluster_t_size; itr_c++) {
            for (itr_p = offset; itr_p < chunk_size + offset; itr_p++){
                assign_point(points, itr_p, clusters_centroids, itr_c);
            }
            itr_p = offset;
        }
        
        for(itr_c = 0; itr_c < cluster_t_size; itr_c++) {
            update_centroids(offset, chunk_size, points, clusters_centroids, itr_c);
        }

        int tasks;
        for(tasks = 1; tasks < num_tasks; tasks++){
            MPI_Recv(&offset, 1, MPI_INT, tasks, tag_1, MPI_COMM_WORLD, &status);
            MPI_Recv(&points[offset], chunk_size, MPI_DOUBLE, tasks, tag_2, MPI_COMM_WORLD, &status);
        }

        print_clusters(points, clusters_centroids);

    }

    // ================== WORKER ======================
    if(task_id > 0){
        source = 0;
        MPI_Recv(&offset, 1, MPI_INT, source, tag_1, MPI_COMM_WORLD, &status);
        MPI_Recv(&points[offset], chunk_size, MPI_DOUBLE, source, tag_2, MPI_COMM_WORLD, &status);

        //printf("process id %d offset %d\n", task_id, offset);
        
        int itr_p = 0;
        int itr_c = 0;
        for(itr_c = 0; itr_c < cluster_t_size; itr_c++) { 
            for (itr_p = offset; itr_p < chunk_size + offset; itr_p++){
                assign_point(points, itr_p, clusters_centroids, itr_c);
            }
            itr_p = offset;
        }

        for(itr_c = 0; itr_c < cluster_t_size; itr_c++) {
            update_centroids(offset, chunk_size, points, clusters_centroids, itr_c);
        }

        dest = 0;
        MPI_Send(&offset, 1, MPI_INT, dest, tag_1, MPI_COMM_WORLD);
        MPI_Send(&points[offset], chunk_size, MPI_DOUBLE, dest, tag_2, MPI_COMM_WORLD);

        print_clusters(points, clusters_centroids);
    }

    MPI_Finalize();

    return 0;
}
