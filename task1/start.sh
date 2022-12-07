mpicc main.c -o test -lm
mpiexec -n $1 ./test
