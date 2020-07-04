//
// Created by er713 on 6/18/20.
//

#ifndef NANOZOMBIE_LOGGER_H
#define NANOZOMBIE_LOGGER_H


#include <string>
#include "nano_struct.h"

class Logger {

public:
    explicit Logger(int id, State state);

    void i(const std::string &msg);

    void e(const std::string &msg);

    void i(const std::string &msg, Message *data);

    void e(const std::string &msg, Message *data);


    std::string msgType_to_string[8] = {"REQ", "DEN", "REP", "TIC", "ACK", "OUT", "NOPE", "AWAY"}; // pewnie nie najpiękniejsze rozwiązanie

protected:
    int id;
    int volume; // ile zajmuje dany turysta
    int size; // ilość wszystkich procesów
    State state;
    int count = 0;
    int count_all = 0;
    Resource costume = static_cast<Resource>(-1);
    Resource vessel = static_cast<Resource>(-1);

private:

    void logger(bool error, const std::string &msg);

    void logger(bool error, const std::string &msg, Message *data);

};


#endif //NANOZOMBIE_LOGGER_H
