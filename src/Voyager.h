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

class Voyager : private Logger {
public:
    Voyager(int id, int size);

    void receive_message();

    void operator()();

private:
    constexpr static const int vessel_capacity[VESSEL_QUANTITY] = {10, 10, 10};  // na razie tak to zainplementowałem, najwyżej później się zminu
//    int id; // id procesu w którym wykonywany jest kod
    int size; // ilość wszystkich procesów
    int volume; // ile zajmuje dany turysta
    int count = 0;
    int count_all = 0;
    unsigned int timestamp = 0;
    bool wasDEN = false;
//    State state = START;
    std::mt19937 rng;
    std::mutex mutex;


    void handle_START(Message *msg);

    void wait_FOR_COSTUME();

    void handle_REQUESTING_COSTUME(Message *msg);

    void handle_HAVE_VESSEL(Message *msg);

    void handle_SIGHTSEEING(Message *msg);

    Resource got_TIC_for = static_cast<Resource>(-1);

    void handle_REQUESTING_VESSEL(Message *msg);

    bool got_ACK = false;

    void handle_WANT_DEPARTURE(Message *msg);

    void check_VALID_COSTUME();

    int get_RANDOM_NUMBER(int, int);

    void resources_on_REQ(Message *response, Message *msg);

    void sightseeing(int time);

};


#endif //NANOZOMBIE_VOYAGER_H
