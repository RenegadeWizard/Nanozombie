//
// Created by er713 on 6/18/20.
//

#include "nano_struct.h"

Message::Message(unsigned int timestamp, int sender, int receiver) {
    this->timestamp = timestamp;
    this->sender_id = sender;
    this->receiver_id = receiver;
}

void Message::send() {

}

void Message::broadcast(int voyagers) {
    for(int i = 0; i < voyagers; ++i){
        if(i != sender_id){
            this->receiver_id = i;
            this-> send();
        }
    }
}

Message::Message() = default;