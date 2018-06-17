#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <stddef.h>
#include <math.h>
#include <float.h>
#include <time.h>

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

int assign_point(Ponto* pts, int numPoints, Cluster* clts, int numClusters) { //return boolean if cluster change happens
	int p;
	int j;
	int stable = 1;
	for (p = 0 ; p < numPoints; p++){
		Ponto* pt = &(pts[p]);
		int myCluster = pt->id_cluster;
		double minDist = myCluster!=-1? euclidian_dist(*pt,clts[myCluster]): DBL_MAX;
		for (j=0; j<numClusters; j++){
			Cluster cluster = clts[j];
			double actualDistance = euclidian_dist(*pt, cluster);
			if(actualDistance<minDist){
				minDist = actualDistance;
				myCluster = j;
				stable = 0;
			}
		}
		pt->id_cluster = myCluster;
	}
	return stable;
}

double generateRandomNumber(int min, int max){
    int ret = min + rand()%(max-min);
    double parsedRet = (double) ret;
    return parsedRet;
}

void generatePoints(Ponto* points, int numberOfPoints){
	srand(time(NULL));   
	int i;
	for (i=0;i<numberOfPoints;i++){
        Ponto temp = {x: generateRandomNumber(-50,50),y:generateRandomNumber(-50,50), id_cluster:-1};
        points[i] = temp;
	}
}

int main (int argc, char *argv[]) {

	int qtyPoints = argc>=2?atoi(argv[1]):100;
	Ponto points[qtyPoints];

	generatePoints(points, qtyPoints);

	Cluster clusters[3] = { {x_centroid:0.0, y_centroid:0.0},
		{x_centroid:7.5, y_centroid:7.6},
		{x_centroid:3.75, y_centroid:3.8}
	};


	int num_tasks, task_id, dest, source, offset, tag_1, tag_2, chunk_size;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

	MPI_Datatype MPI_INFOTYPE;
	initTypes(& MPI_INFOTYPE);

	MPI_Comm_rank(MPI_COMM_WORLD, &task_id);

	int points_t_size = sizeof(points)/sizeof(Ponto);
	int cluster_number = sizeof(clusters)/sizeof(Cluster);

	int resto = points_t_size % num_tasks;
	chunk_size = (points_t_size / num_tasks);

	//myPoints contains all Points that are responsability from current rank
	if(resto){
		if(task_id == num_tasks-1){
			chunk_size += resto;
		}
	}

	Ponto myPoints[chunk_size];

	if(task_id != num_tasks-1){
		int i;
		for(i = 0; i < chunk_size;i++){
			myPoints[i] = points[task_id*chunk_size+i];  
		}
	} else {
		int i;
		for(i = 0 ; i < chunk_size; i++){
			myPoints[i] = points[points_t_size-1 - i];	
		}
	}


	if(task_id == 0){
		struct timespec start, end;
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		int stable = 0;
		while(!stable){
			//Define myPoints Clusters and save x and y from current Cluster
			stable = assign_point(myPoints, chunk_size, clusters, cluster_number);
			int i;
			for (i = 0 ; i<chunk_size; i++){
				Ponto pt = myPoints[i];
			}

			//Current sum_x and sum_y in each Cluster
			double sum_x[cluster_number], sum_y[cluster_number];

			//Current number of points in each Cluster
			int numberOfPoints[cluster_number];

			//Initialize arrays
			for (i=0 ; i<cluster_number;i++){
				sum_x[i] = 0.0;
				sum_y[i] = 0.0;
				numberOfPoints[i] = 0;
			}

			//Sum info from my points
			for (i = 0 ; i<chunk_size; i++){
				Ponto pt = myPoints[i];
				int id = pt.id_cluster;
				numberOfPoints[id]++;
				sum_x[id] += pt.x;
				sum_y[id] += pt.y;
			}

			Info infoFilhos;
			int j;
			//Each process return Info for each Cluster
			for (i=1 ; i<num_tasks; i++){
				//Using id_cluster as tag
				for(j=0; j<cluster_number;j++){
					MPI_Recv(&infoFilhos, 1, MPI_INFOTYPE, i, j, MPI_COMM_WORLD, &status);
					sum_x[j] += infoFilhos.x;
					sum_y[j] += infoFilhos.y;
					numberOfPoints[j] += infoFilhos.numPoints; 
				}
			}
			//Calculate new Cluster Centroids
			for (i = 0; i<cluster_number; i++){
				clusters[i].x_centroid = sum_x[i]/numberOfPoints[i];
				clusters[i].y_centroid = sum_y[i]/numberOfPoints[i];
				//Using numPoints as identifier of Cluster
				Info info = {x: clusters[i].x_centroid, y: clusters[i].y_centroid, numPoints:i};
				MPI_Bcast(&info, 1, MPI_INFOTYPE, 0 ,MPI_COMM_WORLD);
			}
			int aux = 1;
			for(i = 1; i < num_tasks; i++){
				MPI_Recv(&aux,1, MPI_INT, i, 50, MPI_COMM_WORLD, &status);	
				if(!aux){
					stable = 1;
				}
			}
			MPI_Bcast(&stable, 1, MPI_INT, 0, MPI_COMM_WORLD);
		}
		
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		uint delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
		printf("Time for execution: %u miliseconds\n", delta_us);
		int itr_points;
		// for(itr_points = 0; itr_points < chunk_size; itr_points++){
		// 	printf("Ponto ( %f, %f ) no cluster %d\n", myPoints[itr_points].x, myPoints[itr_points].y, myPoints[itr_points].id_cluster);
		// }            
	}
	else{
		int stable = 0;
		while(!stable){    
			stable = assign_point(myPoints, chunk_size, clusters, cluster_number);
			int i;
			int j;
			for (i = 0 ; i<chunk_size; i++){
				Ponto pt = myPoints[i];
			}

			//Current sum_x and sum_y in each Cluster
			double sum_x[cluster_number], sum_y[cluster_number];

			//Current number of points in each Cluster
			int numberOfPoints[cluster_number];

			//Initialize arrays
			for (i=0 ; i<cluster_number;i++){
				sum_x[i] = 0.0;
				sum_y[i] = 0.0;
				numberOfPoints[i] = 0;
			}

			//Sum info from my points
			for (i = 0 ; i<chunk_size; i++){
				Ponto pt = myPoints[i];
				int id = pt.id_cluster;
				numberOfPoints[id]++;
				sum_x[id] += pt.x;
				sum_y[id] += pt.y;
			}

			for (j = 0; j<cluster_number;j++){
				Info info = {x: sum_x[j], y: sum_y[j], numPoints: numberOfPoints[j]};
				MPI_Send(&info, 1, MPI_INFOTYPE, 0, j, MPI_COMM_WORLD);
			}

			//Calculate new Cluster Centroids
			for (i =0; i<cluster_number; i++){
				//Using numPoints as identifier of Cluster
				Info info;
				MPI_Bcast(&info, 1, MPI_INFOTYPE, 0 ,MPI_COMM_WORLD);
				clusters[i].x_centroid = info.x;
				clusters[i].y_centroid = info.y;
			}
			MPI_Send(&stable, 1, MPI_INT, 0, 50, MPI_COMM_WORLD);
			MPI_Bcast(&stable, 1, MPI_INT, 0, MPI_COMM_WORLD);
		}
		int itr_points;
		// for(itr_points = 0; itr_points < chunk_size; itr_points++){
		// 	printf("Ponto ( %f, %f ) no cluster %d\n", myPoints[itr_points].x, myPoints[itr_points].y, myPoints[itr_points].id_cluster);
		// }          
	}

	MPI_Finalize();
	return 0;
}
