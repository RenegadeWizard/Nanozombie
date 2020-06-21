//
// Created by er713 on 6/18/20.
//

#include "Logger.h"


void Logger::logger(bool error, const std::string &msg) {
    char *text_state = state_to_string(state);


    if (error) {
        char err[200];
        sprintf(err, "[%d]\tKostium: %d | Statek: %d\t| %s |[%d] %s\n", id, (costume == COSTUME) ? 1 : 0, vessel, text_state, id, msg.c_str());
        perror(err);
    } else {
        printf("[%d]\tKostium: %d | Statek: %d\t| %s |[%d] %s\n", id, (costume == COSTUME) ? 1 : 0, vessel, text_state, id, msg.c_str());
    }
    delete[] text_state;
}

void Logger::logger(bool error, const std::string &msg, Message *data) {
    char *text_state = state_to_string(state);

    if (error) {
        char err[250];
        sprintf(err, "[%d]\tKostium: %d | Statek: %d\t| %s | Otrzymana wiadomość: %s od %d | %s\n",
                id, (costume == COSTUME) ? 1 : 0, vessel, text_state, msgType_to_string[data->msgType].c_str(), data->sender_id, msg.c_str());
        perror(err);
    } else {
        if(data->msgType==REQ){
            printf("[%d]\tKostium: %d | Statek: %d\t| %s | Otrzymana wiadomość: %d od %d, %d | %s\n",
                   id, (costume == COSTUME) ? 1 : 0, vessel, text_state, data->msgType, data->sender_id, data->resource, msg.c_str());
        } else if(data->msgType==REP){
            printf("[%d]\tKostium: %d | Statek: %d\t| %s | Otrzymana wiadomość: %d od %d, %d | %s\n",
                   id, (costume == COSTUME) ? 1 : 0, vessel, text_state, data->msgType, data->sender_id, data->data, msg.c_str());
        } else{
            printf("[%d]\tKostium: %d | Statek: %d\t| %s | Otrzymana wiadomość: %d od %d | %s\n",
                   id, (costume == COSTUME) ? 1 : 0, vessel, text_state, data->msgType, data->sender_id, msg.c_str());
        }
    }

    delete[] text_state;
}

void Logger::i(const std::string &msg) {
    logger(false, msg);
}

void Logger::e(const std::string &msg) {
//    logger(true, msg);
}

void Logger::i(const std::string &msg, Message *data) {
//    logger(false, msg, data);
}

void Logger::e(const std::string &msg, Message *data) {
//    logger(true, msg, data);
}

Logger::Logger(int id, State state) {
    this->id = id;
    this->state = state;
}

