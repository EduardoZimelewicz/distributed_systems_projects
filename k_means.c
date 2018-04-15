#include<stdio.h>
#include<stdlib.h>
#include<math.h>

typedef struct point {
    int id;
    double point_x;
    double point_y;
    double ctr_dist;
    int cluster_id;
} T_point;

typedef struct cluster {
    double centr_x;
    double centr_y;
    int cluster_id;
} T_cluster;


double euclidian_dist(double point_x, double point_y, double centr_x, double centr_y) {
    return sqrt(pow((point_x - centr_x),2) + pow((point_y - centr_y),2));
}

void assign_points(T_point * points, int points_len, T_cluster * clusters, int clusters_len) {
    int itr_point = 0;
    int itr_cluster = 0;
    double old_ecl_dist = 0;
    double new_ecl_dist = 0;

    for(itr_cluster = 0; itr_cluster < clusters_len; itr_cluster++){
        for(itr_point = 0; itr_point < points_len; itr_point++){
            
            old_ecl_dist = points[itr_point].ctr_dist;
            
            new_ecl_dist = euclidian_dist(points[itr_point].point_x, points[itr_point].point_y, 
            clusters[itr_cluster].centr_x, clusters[itr_cluster].centr_x);

            if(new_ecl_dist > old_ecl_dist){
                points[itr_point].ctr_dist = new_ecl_dist;
                
                if(points[itr_point].cluster_id != -1){
                    printf("Removed from cluster %d\n", points[itr_point].cluster_id);
                    points[itr_point].cluster_id = -1;
                }
                
                points[itr_point].cluster_id = clusters[itr_cluster].cluster_id;
                printf("Added in cluster %d\n", points[itr_point].cluster_id);
            }
        }
        itr_point = 0;
    }
}

void update_centroids(T_point * points, int points_len, T_cluster * clusters, int cltrs_len) {
    int itr_points, itr_cluster = 0;
    double sum_x, sum_y = 0.0;
    double n_cluster_pts = 0.0;
    double new_ctr_x, new_ctr_y = 0.0;

    for(itr_cluster = 0; itr_cluster < cltrs_len; itr_cluster++){
        sum_x = 0.0;
        sum_y = 0.0;
        new_ctr_x = 0.0;
        new_ctr_y = 0.0;
        n_cluster_pts = 0.0;
        
        for(itr_points = 0; itr_points < points_len; itr_points++){
            
            if(points[itr_points].cluster_id == clusters[itr_cluster].cluster_id){
                n_cluster_pts++;
                sum_x += points[itr_points].point_x;
                sum_y += points[itr_points].point_y;
            }
        }
        new_ctr_x = sum_x / n_cluster_pts;
        new_ctr_y = sum_y / n_cluster_pts;

        if(new_ctr_x != clusters[itr_cluster].centr_x && new_ctr_y != clusters[itr_cluster].centr_y){
            clusters[itr_cluster].centr_x = new_ctr_x;
            clusters[itr_cluster].centr_y = new_ctr_y;
            printf("Centroid from cluster %d changed!\n", itr_cluster);
        }

        else {
            printf("Centroid from cluster %d stayed the same...\n", itr_cluster);
        }
        
        itr_points = 0;
    }
}

void print_clusters(T_point * points, int points_len, T_cluster * clusters, int clusters_len){
    int itr_cluster = 0;
    int itr_points = 0;

    for(itr_cluster = 0; itr_cluster < clusters_len; itr_cluster++){
        printf("Elements from cluster %d:\n", itr_cluster);
        for(itr_points = 0; itr_points < points_len; itr_points++){
            
            if(points[itr_points].cluster_id == clusters[itr_cluster].cluster_id){
                printf("( %f, %f ) ", points[itr_points].point_x, points[itr_points].point_y);
            }
        }
        printf("\n");
    }
}

int main () {
    
    double points [7][2] = {{1.0, 1.0},
                         {1.5, 2.0},
                         {3.0, 4.0},
                         {5.0, 7.0},
                         {3.5, 5.0},
                         {4.5, 5.0},
                         {3.5, 4.5}};
    T_point points_t [7];
    
    int itr = 0;
    
    for (itr = 0; itr < 7; itr++){
        points_t[itr].point_x = points[itr][0];
        points_t[itr].point_y = points[itr][1];
        points_t[itr].ctr_dist = 0.0;
        points_t[itr].cluster_id = -1;
        points_t[itr].id = itr;
    }
    
    double clusters_centroids [2][2] = {{0.0,0.0},{7.5,7.5}};

    T_cluster clusters_t [2];
    
    clusters_t[0].centr_x = clusters_centroids[0][0];
    clusters_t[0].centr_y = clusters_centroids[0][1];
    clusters_t[0].cluster_id = 0;

    clusters_t[1].centr_x = clusters_centroids[1][0];
    clusters_t[1].centr_y = clusters_centroids[1][1];
    clusters_t[1].cluster_id = 1;

    int n_iter = 1;
    
    while(n_iter <= 5){
        assign_points(points_t, 7, clusters_t, 2);
        update_centroids(points_t, 7, clusters_t, 2);
        n_iter++;
    }

    print_clusters(points_t, 7, clusters_t, 2);

    return 0;
}