#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <float.h>
#include <time.h>

typedef struct Ponto{
	double x;
	double y;
	int id_cluster;   
} Ponto;


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

int main(int argc, char *argv[]){
    int numberOfPoints = atoi(argv[1]);
    Ponto pontos[numberOfPoints];
    generatePoints(pontos, numberOfPoints);
    int i;
    for  (i = 0; i<numberOfPoints; i++){
        printf("x:%.0f\ty:%.0f\tid:%d\n", pontos[i].x,pontos[i].y,pontos[i].id_cluster);
    }
}