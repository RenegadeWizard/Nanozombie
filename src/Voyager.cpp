//
// Created by er713 on 6/15/20.
//

#include "Voyager.h"
#include <cstdio>
#include <unistd.h>


Voyager::Voyager(int id, int size) : Logger(id, START) {
    this->size = size;
    rng.seed(time(nullptr));
    i("Zaczynamy");
    //TODO: rozpoczęcie czekania losowego czasu, a potem ubiegania się o kostium
    sleep(get_RANDOM_NUMBER(1, 5));
    (new Message())->broadcast(size);    // TODO
}

void Voyager::receive_message(Message *msg) {

    timestamp = std::max(timestamp, msg->timestamp) + 1; // aktualizowanie zegaru Lamporta

    switch (state) {
        case START:
            handle_START(msg);
            break;
        case REQUESTING_COSTUME:
            handle_REQUESTING_COSTUME(msg);
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
    auto response = Message(timestamp, id, msg->sender_id);

    switch (msg->msgType) {

        case DEN:
        case REP: //
        case ACK:
        case RES:
            e("To nie powinno sie wydarzyć", msg);
            break;
        case TIC:
            response.msgType = DEN;
            response.send();
            break;
        case OUT: // nie robi nic
            break;
        case REQ: // TODO: opcjonalnie jakieś sprawdzenie, czy zasoby faktycznie są puste
            response.msgType = RES;
            response.data = 0;
            response.send();
            break;
    }

}

void Voyager::handle_REQUESTING_COSTUME(Message *msg) {
    auto *send = new Message(timestamp, id, msg->sender_id);

    switch (msg->msgType) {
        case REQ:
            if (msg->resource == COSTUME && (msg->timestamp > timestamp || (msg->timestamp == timestamp && msg->sender_id > id))) {
                send->msgType = DEN;
            } else {
                send->msgType = RES;
            }
            break;
        case DEN:
            wasDEN = true;
            break;
        case REP:
            count++;
            count_all++;
            check_VALID_COSTUME();
            break;
        case RES:
            count_all++;
            check_VALID_COSTUME();
            break;
        case TIC:
            send->msgType = DEN;
            break;
        default:
            e("To sie nie powinno było wydarzyć", msg);
            break;
    }
    send->send();
    delete send;
}

void Voyager::check_VALID_COSTUME() {
    if (count_all == size) {
        if (count + 1 < COSTUME_QUANTITY && !wasDEN)
            state = static_cast<State>(get_RANDOM_NUMBER(0, VESSEL_QUANTITY - 1));  // ubieganie sie o randomowy statek
        count = count_all = 0;
        wasDEN = false;
    }
}

void Voyager::handle_HAVE_VESSEL(Message *msg) {

}

void Voyager::handle_SIGHTSEEING(Message *msg) {
    auto *send = new Message(timestamp, id, msg->sender_id);
    switch (msg->msgType) {
        case REQ:
            if(msg->resource == (Resource)state){
                send->msgType = DEN;
            } else if(msg->resource == COSTUME){
                send->msgType = REP;
            } else {
                send->msgType = RES;
            }
            break;
        case TIC:
            send->msgType = DEN;
            break;
        default:
            e("To sie nie powinno było wydarzyć", msg);
            break;
    }
    send->send();
    delete send;
}

void Voyager::handle_REQUESTING_VESSEL(Message *msg) {

}

int Voyager::get_RANDOM_NUMBER(int a, int b) {
    std::uniform_int_distribution<std::mt19937::result_type> dist_ab(a, b);
    return dist_ab(rng);
}

int Voyager::getId() const {
    return id;
}

State Voyager::getState() const {
    return state;
}


