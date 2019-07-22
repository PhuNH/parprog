#include "nbody.h"

void simulate_par(int steps, int num_leaves, Node* root) {
    // TODO

    // You do not need to call MPI_Init and MPI_Finalize, this is already done.

    // root is the root node of the whole domain. The data will be initialised in each process, so
    // you do not need to copy it at the start. However, the main process (rank 0) has to have the
    // full data at the end of the simulation, so you will need to collect it.
}
