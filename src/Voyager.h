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
    Voyager(int id, int size);

    virtual ~Voyager();

    void receive_message();

    void operator()();

    constexpr static int vessel_capacity[VESSEL_QUANTITY] = {10, 10, 10};  // na razie tak to zainplementowałem, najwyżej później się zminu
private:
//    int id; // id procesu w którym wykonywany jest kod
    int size; // ilość wszystkich procesów
    int volume; // ile zajmuje dany turysta
    int count = 0;
    int count_all = 0;
    unsigned int timestamp = 0;
    int sent_timestamp = -1;
    bool wasDEN = false;
//    State state = START;
    std::mt19937 rng;
    std::mutex mutex;


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

    struct comparator {
        int x;

        explicit comparator(int x);

        bool operator()(Message i, Message j) const;
    };

};


#endif //NANOZOMBIE_VOYAGER_H
