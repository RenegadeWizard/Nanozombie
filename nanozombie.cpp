#include <cstdio>
#include <mpi.h>
#include "src/Voyager.h"
#include <thread>


int main(int argc, char* argv[]){

    MPI_Init(&argc, &argv);

    int size, id;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    auto *voyager = new Voyager(id, size);
//    Voyager voy(id,size);
    std::thread thread(std::ref(*voyager));

    while (true){
        voyager->receive_message();
//        voy.receive_message();
    }

    delete voyager;
    MPI_Finalize();
    return 0;
}