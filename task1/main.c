#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <mpi.h>


MPI_Request send_int(int x, int y, MPI_Comm comm, int *data) {
    int target_rank;
    int coords[2] = {x, y};
    MPI_Cart_rank(comm, coords, &target_rank);

    MPI_Request request;
    MPI_Isend(data, 1, MPI_INT, target_rank,
              0, MPI_COMM_WORLD, &request);
    return request;
}

int receive_int(int x, int y, MPI_Comm comm) {
    int target_rank;
    int coords[2] = {x, y};
    MPI_Cart_rank(comm, coords, &target_rank);

    int data;
    MPI_Recv(&data, 1, MPI_INT, target_rank,
             MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return data;
}

int main(int argc, char **argv) {
    int debug = 1;
    int size = 3;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);

    size = (int) sqrt((double) size);

    int rank, r_size;

    int ndims = 2;

    int n_size[2] = {size, size};
    int periodic[2] = {0, 0};

    MPI_Comm cart_comm;

    MPI_Cart_create(MPI_COMM_WORLD, ndims, n_size, periodic, 1, &cart_comm);

    MPI_Comm_rank(cart_comm, &rank);
    MPI_Comm_size(cart_comm, &r_size);

    int mpi_map[size][size];
    for (int i = 0; i < size * size; i++) {
        int coords[2];
        MPI_Cart_coords(cart_comm, i, ndims, coords);
        mpi_map[coords[0]][coords[1]] = i;
    }

    srand(r_size + size);

    int coords[2];
    MPI_Cart_coords(cart_comm, rank, ndims, coords);
//    printf("My id = %2d, my coords is <%2d, %2d>\n", rank, coords[0], coords[1]);


    int number;
    if (coords[0] == 0) {
        if (coords[1] == 0) {
            number = rand();
        } else {
            number = receive_int(coords[0], coords[1] - 1, cart_comm);
            printf("Got number, my coords is <%2d, %2d>\n", coords[0], coords[1]);
        }

        MPI_Request r1, r2;
        if (coords[1] + 1 != size) {
            r1 = send_int(coords[0], coords[1] + 1, cart_comm, &number);
        }
        r2 = send_int(coords[0] + 1, coords[1], cart_comm, &number);
        if (coords[1] + 1 != size) {
            MPI_Wait(&r1, MPI_STATUS_IGNORE);
        }
        MPI_Wait(&r2, MPI_STATUS_IGNORE);
    } else {
        number = receive_int(coords[0] - 1, coords[1], cart_comm);
        printf("Got number, my coords is <%2d, %2d>\n", coords[0], coords[1]);
        MPI_Request r1;
        if (coords[0] + 1 != size) {
            r1 = send_int(coords[0] + 1, coords[1], cart_comm, &number);
            MPI_Wait(&r1, MPI_STATUS_IGNORE);
        }
    }

    MPI_Finalize();
    return 0;
}