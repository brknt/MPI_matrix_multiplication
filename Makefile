build:
	mpicc -o mpi mpi.c

run2:
	mpiexec -n 2 ./mpi
	
run3:
	mpiexec -n 3 ./mpi

run4:
	mpiexec -n 4 ./mpi
