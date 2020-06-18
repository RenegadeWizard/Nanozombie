//
// Created by er713 on 6/18/20.
//

#include "nano_struct.h"
#include <cstdio>

Message::Message(unsigned int timestamp, int sender, int receiver) {
    this->timestamp = timestamp;
    this->sender_id = sender;
    this->receiver_id = receiver;
}

void Message::send() {

}

void Message::broadcast(int voyagers) {
    for (int i = 0; i < voyagers; ++i) {
        if (i != sender_id) {
            this->receiver_id = i;
            this->send();
        }
    }
}

Message::Message() = default;

char *state_to_string(State state) {
    char *text_state = new char[50];
    switch (state) {

        case START:
            sprintf(text_state, "Odpoczywam...                  ");
            break;
        case REQUESTING_COSTUME:
            sprintf(text_state, "Domagam się kostiumu...        ");
            break;
        case HAVE_VESSEL:
            sprintf(text_state, "Czekam na wypłynięcie statku...");
            break;
        case SIGHTSEEING:
            sprintf(text_state, "Zwiedzam...                    ");
            break;
        default:
            sprintf(text_state, "Domagam się statku numer %d... ", state);
            break;
    }

    return text_state;
}