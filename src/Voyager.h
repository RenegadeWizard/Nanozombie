//
// Created by er713 on 6/15/20.
//

#ifndef NANOZOMBIE_VOYAGER_H
#define NANOZOMBIE_VOYAGER_H

#include "nano_struct.h"
#include <random>
#include <ctime>

class Voyager {
public:
    Voyager(int id, int size);

    void receive_message(Message *msg);
    void send_message(Message *msg);

private:
    int id; // id procesu w którym wykonywany jest kod
    int size; // ilość wszystkich procesów
    int count = 0;
    int count_all = 0;
    unsigned int timestamp = 0;
    State state = START;
    std::mt19937 rng;

    // trochę pospamowałem tymi funkcjiami (do obsługi poszczególnych typów wiadomości), ale zdaje mi się, że tak będzie bardziej przejrzyście
    // bo jak się nagle pojawi switch w switch to cała przejrzystość pójdzie w las, możliwe, że niektóre przypadki będą na tyle łatwe, że się usunie taką metodę
    void handle_START(Message *msg);

    void handle_REQUESTING_COSTUME(Message *msg);

    void handle_HAVE_VESSEL(Message *msg);

    void handle_SIGHTSEEING(Message *msg);

    void handle_REQUESTING_VESSEL(Message *msg);

    void send_REQUEST_COSTUME(Message *msg);

    void check_VALID_COSTUME();

    int get_RANDOM_NUMBER(int, int);

    void logger(char *msg);

};


#endif //NANOZOMBIE_VOYAGER_H
