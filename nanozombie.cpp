#include <cstdio>
#include <mpi.h>
#include "src/Voyager.h"


int main(int argc, char* argv[]){

    MPI_Init(&argc, &argv);

    int size, id;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    auto *voyager = new Voyager(id, size);

    //TODO: jakieś odczytywanie w pętli przychodzących wiadomości i wywoływanie tego niżej
//    voyager->receive_message(msg);

    delete voyager;
    MPI_Finalize();
    return 0;
}