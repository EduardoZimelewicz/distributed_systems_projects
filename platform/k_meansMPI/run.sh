mpicc k_means.c -o k_means -lm
mpirun -np 2 ./k_means 100
