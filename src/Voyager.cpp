//
// Created by er713 on 6/15/20.
//

#include "Voyager.h"
#include <cstdio>


Voyager::Voyager(int id, int size) {
    this->id = id;
    this->size = size;
    rng.seed(time(nullptr));
    logger(false, "Zaczynamy");
    //TODO: rozpoczęcie czekania losowego czasu, a potem ubiegania się o kostium
    send_REQUEST_COSTUME(new Message());    // TODO
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

void Voyager::send_message(Message *msg) {

}

void Voyager::handle_START(Message *msg) {

    switch (msg->msgType) {

        case REQ:
            break;
        case DEN:
        case REP: //
            break;
        case TIC:
            break;
        case ACK:
            break;
        case OUT:
            break;
        case RES:
            break;
        default:
            break;
    }

}

void Voyager::handle_REQUESTING_COSTUME(Message *msg) {
    auto *send = new Message(timestamp, id, msg->sender_id); // przepraszam, nie chciałem się wpieprzać od nie mojego kodu, ale mam nadzieję, że nic nie zepsułem (nie mogłem się powstrzymać)
//    send->timestamp = timestamp;
//    send->receiver_id = msg->sender_id;
//    send->sender_id = id;

    switch (msg->msgType) {
        case REQ:
            if (msg->resource == COSTUME && (msg->timestamp > timestamp || (msg->timestamp == timestamp && msg->sender_id > id))) {
                send->msgType = DEN;
            } else {
                send->msgType = RES;
            }
            break;
        case DEN:
            count = 0;
            count_all = 0;
            send_REQUEST_COSTUME(new Message());    // TODO
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
//            printf("To sie nie powinno było wydarzyć (handle_REQUESTING_COSTUME -> default w switch dostał wiadomość: %d)", msg->msgType);
            logger(true, "To sie nie powinno było wydarzyć", msg->msgType);
            break;
    }
//    send_message(send);
    send->send();
    delete send;
}

void Voyager::check_VALID_COSTUME() {
    if (count_all == size) {
        if (count + 1 < COSTUME_QUANTITY)
            state = static_cast<State>(get_RANDOM_NUMBER(0, VESSEL_QUANTITY - 1));
        count = count_all = 0;
    }
}

void Voyager::handle_HAVE_VESSEL(Message *msg) {

}

void Voyager::handle_SIGHTSEEING(Message *msg) {

}

void Voyager::handle_REQUESTING_VESSEL(Message *msg) {

}

void Voyager::send_REQUEST_COSTUME(Message *msg) {
//    dla każdego odbiorcy (broadcast):
    send_message(msg);
}

int Voyager::get_RANDOM_NUMBER(int a, int b) {
    std::uniform_int_distribution<std::mt19937::result_type> dist6(a, b);
    return dist6(rng);
}

void Voyager::logger(bool error, const std::string &msg) {
    char *text_state = state_to_string(state);


    if (error) {
        char err[200];
        sprintf(err, "[%d]\tKostium: %d | Statek: %d | %s\n\t\t%s", id, 0, 0, text_state, msg.c_str());
        perror(err);
    } else {
        printf("[%d]\tKostium: %d | Statek: %d | %s\n\t\t%s\n", id, 0, 0, text_state, msg.c_str()); //TODO: zamienić 0'a na informacje o posiadaniu kostiumu i numerze statku
    }
    delete[] text_state;
}

std::string msgType_to_string[7] = {"REQ", "DEN", "REP", "TIC", "ACK", "OUT", "RES"}; // pewnie nie najpiękniejsze rozwiązanie

void Voyager::logger(bool error, const std::string &msg, MessageType msgType) {
    char *text_state = state_to_string(state);

    if (error) {
        char err[250];
        sprintf(err, "[%d]\tKostium: %d | Statek: %d | %s | Otrzymana wiadomość: %s\n\t\t%s", id, 0, 0, text_state, msgType_to_string[msgType].c_str(), msg.c_str());
        perror(err);
    } else {
        printf("[%d]\tKostium: %d | Statek: %d | %s | Otrzymana wiadomość: %s\n\t\t%s\n", id, 0, 0, text_state, msgType_to_string[msgType].c_str(), msg.c_str()); //TODO: zamienić 0'a na informacje o posiadaniu kostiumu i numerze statku
    }

    delete[] text_state;
}


