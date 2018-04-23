#include<stdio.h>
#include<stdlib.h>
#include "mpi.h"
#include<stddef.h>
#include<math.h>
#include<float.h>

typedef struct Ponto{
    double x;
    double y;
    int id_cluster;   
} Ponto;

typedef struct Cluster{
    double x_centroid;
    double y_centroid;
} Cluster;

typedef struct Info{
    double x;
    double y;
    int numPoints;
}Info;

//Function that initializes DataTypes
void initTypes(MPI_Datatype* infoType){
    int blockLenghts[3] = {1,1,1};
    MPI_Datatype types[3] = {MPI_INT,MPI_DOUBLE, MPI_DOUBLE};
    MPI_Aint disp[3] = {offsetof(Info,numPoints),offsetof(Info,x),offsetof(Info,y)};
    MPI_Type_create_struct(3, blockLenghts, disp, types, infoType);
    MPI_Type_commit(infoType);
}


double euclidian_dist(Ponto p1, Cluster c2) {
    return sqrt(pow((p1.x - c2.x_centroid),2) + pow((p1.y - c2.y_centroid),2));
}

void assign_point(Ponto* pts, int numPoints, Cluster* clts, int numClusters) {
    for (int p=0 ; p<numPoints; p++){
        Ponto* pt = &(pts[p]);
        int myCluster = pt->id_cluster;
        double minDist = myCluster!=-1? euclidian_dist(*pt,clts[myCluster]): DBL_MAX;
        for (int j=0; j<numClusters; j++){
            Cluster cluster = clts[j];
            double actualDistance = euclidian_dist(*pt, cluster);
            if(actualDistance<minDist){
                minDist = actualDistance;
                myCluster = j;
            }
        }
        pt->id_cluster = myCluster;
    }
}

int main (int argc, char *argv[]) {

    Ponto points[8] = { {x:1.0, y:1.0, id_cluster:-1},
                        {x:1.5, y:2.0, id_cluster:-1},
                        {x:3.0, y:4.0, id_cluster:-1},
                        {x:5.0, y:7.0, id_cluster:-1},
                        {x:3.5, y:5.0, id_cluster:-1},
                        {x:4.5, y:5.0, id_cluster:-1},
                        {x:3.5, y:4.5, id_cluster:-1},
                        {x:2.5, y:1.5, id_cluster:-1}
                        };

    Cluster clusters[2] = { {x_centroid:0.0, y_centroid:0.0},
                            {x_centroid:7.5, y_centroid:7.6}
                          };

    
    int num_tasks, task_id, dest, source, offset, tag_1, tag_2, chunk_size;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    
    MPI_Datatype MPI_INFOTYPE;
    initTypes(& MPI_INFOTYPE);
   
    /*
    check if array can be divided
    by the number of tasks proposed
    */

    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    
    int points_t_size = sizeof(points)/sizeof(Ponto);
    int cluster_number = sizeof(clusters)/sizeof(Cluster);
    
    //Considerar resto!!!
    chunk_size = (points_t_size / num_tasks);
    
    //myPoints contains all Points that are responsability from current rank
    Ponto myPoints[chunk_size];
    for(int i=0; i<chunk_size;i++){
        myPoints[i] = points[task_id*chunk_size+i];  
    }
    
    if(task_id == 0){
        //Define myPoints Clusters and save x and y from current Cluster
        assign_point(myPoints, chunk_size, clusters, cluster_number);
        
        for (int i = 0 ; i<chunk_size; i++){
            Ponto pt = myPoints[i];
            printf("Eu sou %d e Ponto (%f,%f) no cluster %d\n",task_id, pt.x,pt.y,pt.id_cluster);
        }

        //Current sum_x and sum_y in each Cluster
        double sum_x[cluster_number], sum_y[cluster_number];
        
        //Current number of points in each Cluster
        int numberOfPoints[cluster_number];

        //Não tenho certeza se o C inicia tudo 0, então quis garantir
        for (int i=0 ; i<cluster_number;i++){
            sum_x[i] = 0.0;
            sum_y[i] = 0.0;
            numberOfPoints[i] = 0;
        }

        //Sum info from my points
        for (int i = 0 ; i<chunk_size; i++){
            Ponto pt = myPoints[i];
            int id = pt.id_cluster;
            numberOfPoints[id]++;
            sum_x[id] += pt.x;
            sum_y[id] += pt.y;
        }

        Info infoFilhos;
        
        //Each process return Info for each Cluster
        for (int i=1 ; i<num_tasks; i++){
            //Using id_cluster as tag
            for(int j=0; j<cluster_number;j++){
                MPI_Recv(&infoFilhos, 1, MPI_INFOTYPE, i, j, MPI_COMM_WORLD, &status);
                sum_x[j] += infoFilhos.x;
                sum_y[j] += infoFilhos.y;
                numberOfPoints[j] += infoFilhos.numPoints; 
            }
        }
        //Calculate new Cluster Centroids
        for (int i =0; i<cluster_number; i++){
            clusters[i].x_centroid = sum_x[i]/numberOfPoints[i];
            clusters[i].y_centroid = sum_y[i]/numberOfPoints[i];
            //Using numPoints as identifier of Cluster (Gambiarraça)
            Info info = {x: clusters[i].x_centroid, y: clusters[i].y_centroid, numPoints:i};
            MPI_Bcast(&info, 1, MPI_INFOTYPE, 0 ,MPI_COMM_WORLD);
            printf("Centroide de %d: (%f,%f)\n", i, clusters[i].x_centroid,clusters[i].y_centroid);
        }

                    
    }
    else{
        assign_point(myPoints, chunk_size, clusters, cluster_number);
        for (int i = 0 ; i<chunk_size; i++){
            Ponto pt = myPoints[i];
            printf("Eu sou %d e Ponto (%f,%f) no cluster %d\n",task_id, pt.x,pt.y,pt.id_cluster);
        }

        //Current sum_x and sum_y in each Cluster
        double sum_x[cluster_number], sum_y[cluster_number];
        
        //Current number of points in each Cluster
        int numberOfPoints[cluster_number];

        //Não tenho certeza se o C inicia tudo 0, então quis garantir
        for (int i=0 ; i<cluster_number;i++){
            sum_x[i] = 0.0;
            sum_y[i] = 0.0;
            numberOfPoints[i] = 0;
        }

        //Sum info from my points
        for (int i = 0 ; i<chunk_size; i++){
            Ponto pt = myPoints[i];
            int id = pt.id_cluster;
            numberOfPoints[id]++;
            sum_x[id] += pt.x;
            sum_y[id] += pt.y;
        }

        for (int j = 0; j<cluster_number;j++){
            Info info = {x: sum_x[j], y: sum_y[j], numPoints: numberOfPoints[j]};
            MPI_Send(&info, 1, MPI_INFOTYPE, 0, j, MPI_COMM_WORLD);
        }

        //Calculate new Cluster Centroids
        for (int i =0; i<cluster_number; i++){
            //Using numPoints as identifier of Cluster (Gambiarraça)
            Info info;
            MPI_Bcast(&info, 1, MPI_INFOTYPE, 0 ,MPI_COMM_WORLD);
            clusters[i].x_centroid = info.x;
            clusters[i].y_centroid = info.y;
            printf("Centroide de %d: (%f,%f)\n", i, clusters[i].x_centroid,clusters[i].y_centroid);
        }

    }    
    
    MPI_Finalize();

    return 0;
}
