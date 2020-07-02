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

class Voyager : private Logger {
public:
    Voyager(int id, int size, MPI_Comm thread_comm);

    virtual ~Voyager();

    void receive_message();

    constexpr static int vessel_capacity[VESSEL_QUANTITY] = {10, 7, 10, 10, 7, 10};
    static void wait_FOR_COSTUME(void *voyager);

private:
    int size; // ilość wszystkich procesów
    int count = 0;
    int count_all = 0;
    unsigned int timestamp = 0;
    int sent_timestamp = -1;
    bool wasDEN = false;
    MPI_Comm thread_comm;
    pthread_t pthread;
    int time_to_sleep;
    std::mt19937 rng;
    std::mutex mutex;


    void handle_START(Message *msg);

    void handle_REQUESTING_COSTUME(Message *msg);

    void handle_HAVE_VESSEL(Message *msg);

    void handle_SIGHTSEEING(Message *msg);

    std::vector<Message> *got_TIC_for;
    bool vesselAway = false;

    void handle_REQUESTING_VESSEL(Message *msg);

    bool got_ACK = false;

    void handle_WANT_DEPARTURE(Message *msg);

    void start_REQUESTING_VESSEL();

    void start_REQUESTING_VESSEL(Resource resource);

    void check_VALID_COSTUME();

    int get_RANDOM_NUMBER(int, int);

    void resources_on_REQ(Message *response, Message *msg);

    void start_SIGHTSEEING(int time);

    static void sightseeing(void *voyager);

    static void start_REQUESTIN_COSTUME(Voyager *th, bool lock);

};


#endif //NANOZOMBIE_VOYAGER_H
