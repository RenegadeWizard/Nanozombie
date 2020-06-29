//
// Created by er713 on 6/15/20.
//

#ifndef NANOZOMBIE_VOYAGER_H
#define NANOZOMBIE_VOYAGER_H

#include "nano_struct.h"
#include "Logger.h"
#include <random>
#include <ctime>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
//#include <list>
//#include <functional>
//#include <semaphore.h>

class Voyager : private Logger {
public:
    Voyager(int id, int size, MPI_Comm thread_comm);

    virtual ~Voyager();

    void receive_message();

    void operator()();

//    constexpr static int vessel_capacity[VESSEL_QUANTITY] = {15, 20, 24, 19, 20, 21, 16, 20, 18, 17};  // na razie tak to zainplementowałem, najwyżej później się zminu
    constexpr static int vessel_capacity[VESSEL_QUANTITY] = {10, 7};  // na razie tak to zainplementowałem, najwyżej później się zminu
    static void wait_FOR_COSTUME2(void *voyager);

private:
//    int id; // id procesu w którym wykonywany jest kod
    int size; // ilość wszystkich procesów
    int volume; // ile zajmuje dany turysta
    int count = 0;
    int count_all = 0;
    unsigned int timestamp = 0;
    int sent_timestamp = -1;
    bool wasDEN = false;
    MPI_Comm thread_comm;
    std::thread *thread;
    pthread_t pthread;
    int time_to_sleep;
//    State state = START;
    std::mt19937 rng;
    std::mutex mutex;
    /*sem_t *semaphore;
    std::list<std::function<void(void)>> *messageToServe;
    bool running_computing_thread = true;
    pthread_t pthread_computing;*/


    void handle_START(Message *msg);

    void wait_FOR_COSTUME();

    void handle_REQUESTING_COSTUME(Message *msg);

    void handle_HAVE_VESSEL(Message *msg);

    void handle_SIGHTSEEING(Message *msg);

    std::vector<Message> *got_TIC_for;

    void handle_REQUESTING_VESSEL(Message *msg);

    bool got_ACK = false;

    void handle_WANT_DEPARTURE(Message *msg);

    void start_REQUESTING_VESSEL();

    void start_REQUESTING_VESSEL(Resource resource);

    void check_VALID_COSTUME();

    int get_RANDOM_NUMBER(int, int);

    void resources_on_REQ(Message *response, Message *msg);

    void start_SIGHTSEEING(int time);

    void sightseeing(int time);

    static void sightseeing2(void *voyager);

    static void start_REQUESTIN_COSTUME(Voyager *th, bool lock);

//    static void computing_thread(void *voyager);

};


#endif //NANOZOMBIE_VOYAGER_H
