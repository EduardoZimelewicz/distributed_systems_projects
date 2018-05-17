#include "mpi.h"
#include <stdio.h>

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


int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Datatype MPI_PONTO, MPI_CLUSTER;
    initTypes(&MPI_PONTO, &MPI_CLUSTER);
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Status status;
    if (my_rank==0){
        Ponto pt = {x:1.0, y:2.0, lastDistance:0, id_cluster:0.0};
        Cluster clt = {x_centroid:1, y_centroid:0, id_cluster:1};
        MPI_Send(&pt, 1, MPI_PONTO, 1, 200, MPI_COMM_WORLD);
        MPI_Send(&clt, 1, MPI_CLUSTER, 1, 200, MPI_COMM_WORLD);
    }
    else{
        Ponto pt;
        Cluster clt;
        MPI_Recv(&pt, 1, MPI_PONTO, MPI_ANY_SOURCE, MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        
        printf("x: %f\ny: %f\nlastDistance: %f\nid_cluster: %d\n", pt.x,pt.y,pt.lastDistance
                                                                            ,pt.id_cluster);
        
        MPI_Recv(&clt, 1, MPI_CLUSTER, MPI_ANY_SOURCE, MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        
        printf("x_c: %f\ny_C: %f\nid_cluster: %d\n", clt.x_centroid,clt.y_centroid,clt.id_cluster);
    }
    MPI_Finalize();
    return 0;
}
