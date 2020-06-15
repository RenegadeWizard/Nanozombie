//
// Created by er713 on 6/15/20.
//

#include "Voyager.h"
#include <cstdio>

Voyager::Voyager(int id, int size) {
    this->id = id;
    this->size = size;
    //TODO: rozpoczęcie czekania losowego czasu, a potem ubiegania się o kostium
}

void Voyager::receive_message(Message *msg) {

    switch (state) {
        case START:
            handle_START(msg);
            break;
        case REQUESTING_COSTUME:
            handle_REQUESTING_COSTUME(msg);
            break;
        case HAVE_COSTUME:
            handle_HAVE_COSTUME(msg);
            break;
        case HAVE_VESSEL:
            handle_HAVE_VESSEL(msg);
            break;
        case SIGHTSEEING:
            handle_SIGHTSEEING(msg);
            break;
        default:
            handle_REQUESTING_VESSEL(msg);
            break;
    }

}

void Voyager::handle_START(Message *msg) {

}

void Voyager::handle_REQUESTING_COSTUME(Message *msg) {

}

void Voyager::handle_HAVE_COSTUME(Message *msg) {

}

void Voyager::handle_HAVE_VESSEL(Message *msg) {

}

void Voyager::handle_SIGHTSEEING(Message *msg) {

}

void Voyager::handle_REQUESTING_VESSEL(Message *msg) {

}

