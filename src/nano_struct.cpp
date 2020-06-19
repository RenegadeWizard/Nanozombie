//
// Created by er713 on 6/18/20.
//

#include "nano_struct.h"
#include <cstdio>

Singleton* Singleton::INSTANCE = nullptr;

void Singleton::create_custom_message_type() {
    int blocklengths[7] = {1, 1, 1, 1, 1, 1, 1};
    MPI_Datatype types[7] = {MPI_INT, MPI_INT, MPI_INT, MPI_UNSIGNED, MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint offsets[7];
    offsets[0] = offsetof(struct Message, sender_id);
    offsets[1] = offsetof(struct Message, receiver_id);
    offsets[2] = offsetof(struct Message, info_type);
    offsets[3] = offsetof(struct Message, timestamp);
    offsets[4] = offsetof(struct Message, data);
    offsets[5] = offsetof(struct Message, msgType);
    offsets[6] = offsetof(struct Message, resource);

    if(MPI_Type_create_struct(7, blocklengths, offsets, types, &mpi_message_type)!=MPI_SUCCESS){
        perror("nie dobrze");
    }
    MPI_Type_commit(&mpi_message_type);
}

Singleton::Singleton() {
    create_custom_message_type();
}

Singleton Singleton::getInstance() {
    if(INSTANCE == nullptr){
        INSTANCE = new Singleton();
    }
    return *INSTANCE;
}

Message::Message(unsigned int timestamp, int sender, int receiver) {
    this->timestamp = timestamp;
    this->sender_id = sender;
    this->receiver_id = receiver;
}

void Message::send() {
    MPI_Send( this, 1, Singleton::getInstance().getDataType(), receiver_id, TAG, MPI_COMM_WORLD );
}

void Message::broadcast(int voyagers) {
    for (int i = 0; i < voyagers; ++i) {
        if (i != sender_id) {
            this->receiver_id = i;
            this->send();
        }
    }
}

Message::Message(Message *message) {
    sender_id = message->sender_id;
    receiver_id = message->receiver_id;
    timestamp = message->timestamp;
    data = message->data;
    msgType = message->msgType;
    resource = message->resource;
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
