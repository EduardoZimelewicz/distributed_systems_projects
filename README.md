# distributed_systems_projects
Repository for distributed systems class

For running the first k means draft use:

```
mpicc k_means.c -o k_means -lm
```

and then

```
mpirun -np 2 k_means
```

Note: The matrix for points and clusters is fixed, you should test with 2, 4 or 8 tasks only for now...
