function mpirun {
    docker run --user $(id -u):$(id -g) --cap-drop=all --security-opt label:disabled -v $PWD:/sandbox $ulfm_image mpirun --map-by :oversubscribe --mca btl tcp,self $@
}
function mpicc {
    docker run --user $(id -u):$(id -g) --cap-drop=all --security-opt label:disabled -v $PWD:/sandbox $ulfm_image mpicc $@
}

mpicc mpi_sor_3d_upgraded.c -o test
mpirun --with-ft ulfm -n $1 ./test
