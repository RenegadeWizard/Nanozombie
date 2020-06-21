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
    pthread_create(&thread, nullptr, reinterpret_cast<void *(*)(void *)>(Voyager::wait_FOR_COSTUME2), voyager);
//    std::thread thread(std::ref(*voyager));
//    thread.detach();

    while (true) {
        voyager->receive_message();
//        voy.receive_message();
    }

    delete
            voyager;

    MPI_Finalize();

    return 0;
}