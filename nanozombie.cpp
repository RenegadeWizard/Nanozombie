#include <cstdio>
#include <mpi.h>
#include "src/Voyager.h"
#include <thread>


int main(int argc, char *argv[]) {

    int thr;
    MPI_Comm thread_comm;
    pthread_t thread;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &thr);

    switch (thr) {
        case MPI_THREAD_SERIALIZED:
            printf("serlialized\n");
            break;
        case MPI_THREAD_FUNNELED:
            printf("funneled\n");
            break;
        case MPI_THREAD_MULTIPLE:
            printf("multiple\n");
            break;
        case MPI_THREAD_SINGLE:
            printf("single\n");
            break;
    }

    MPI_Comm_dup(MPI_COMM_WORLD, &thread_comm);

    int size, id;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    auto *voyager = new Voyager(id, size, thread_comm);
    auto *attr = new pthread_attr_t;
    pthread_attr_init(attr);
    pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread, attr, reinterpret_cast<void *(*)(void *)>(Voyager::wait_FOR_COSTUME), voyager);

    while (true) {
        voyager->receive_message();
    }

    delete voyager;
    MPI_Finalize();
    return 0;
}