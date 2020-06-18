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
}

void Voyager::operator()() {
    wait_FOR_COSTUME();
}

void Voyager::wait_FOR_COSTUME() {
    std::this_thread::sleep_for(std::chrono::milliseconds(get_RANDOM_NUMBER(500, 5000)));
    mutex.lock();
    state = REQUESTING_COSTUME;
    auto send = new Message(timestamp, id, 0);
    send->msgType = REQ;
    send->resource = COSTUME;
    send->broadcast(size);
    delete send;
    mutex.unlock();
}

void Voyager::receive_message() {
    MPI_Status status;
    auto msg = new Message();
    MPI_Recv(msg, 1, Singleton::getInstance().getDataType(), MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    mutex.lock();

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
        case WANT_DEPARTURE:
            handle_WANT_DEPARTURE(msg);
            break;
        default:
            handle_REQUESTING_VESSEL(msg);
            break;
    }
    mutex.unlock();

    delete msg;
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
        if (count + 1 < COSTUME_QUANTITY && !wasDEN) {
            state = static_cast<State>(get_RANDOM_NUMBER(0, VESSEL_QUANTITY - 1));  // ubieganie sie o randomowy statek
        } else {
            count = count_all = 0;
            wasDEN = false;
            std::thread thread(std::ref(*&*this));  // To wygląda źle
        }

    }
}

void Voyager::handle_HAVE_VESSEL(Message *msg) {
    auto response = Message(timestamp, id, msg->sender_id);

    switch (msg->msgType) {

        case REQ:

            break;
        case DEN:
            break;
        case REP:
            ++count_all;

            break;
        case TIC:
//            if(msg->resource)
            got_TIC_for = msg->resource;
            break;
        case ACK:
            break;
        case OUT: // wypłynięcie
            state = SIGHTSEEING;
            //TODO: odliczanie czasu msg->data, osobny wątek
            break;
        case RES:
            ++count_all;

            break;
    }

    if (count_all == size - 1) {

    }
}

void Voyager::handle_WANT_DEPARTURE(Message *msg) { //TODO!!!: dodać do sprawozdania, że po TIC odsyłane są DENy kiedy turysta zajmuje więcej miejsa niż jest dostępne, oraz sprawdzanie przed ACK (3.4)

    switch (msg->msgType) {

        case REQ:

            break;
        case DEN:
            ++count_all;
            break;
        case REP:
            break;
        case TIC: // TODO: rozważyć, czy możliwe / czy dwa procesy mogą chcieć wypływać (lepiej żeby było pojedyńczo)
            break;
        case ACK:
            ++count_all;
            got_ACK = true;
            break;
        case OUT: // TODO: rozważyć, czy możliwe / czy dwa procesy mogą chcieć wypływać (lepiej żeby było pojedyńczo)

            break;
        case RES:
            break;
    }

    if (count_all == size - 1) { // kiedy otrzyma wszystkie odpowiedzi

        if (got_ACK) {
            state = HAVE_VESSEL;
        } else {
            auto out = Message(timestamp, id);
            out.msgType = OUT;
            out.data = get_RANDOM_NUMBER(10000, 60000); //TODO: uzupełnić i sprawdzić
            out.resource;
            out.broadcast(size);
        }

    }
}

void Voyager::handle_SIGHTSEEING(Message *msg) {
    auto *send = new Message(timestamp, id, msg->sender_id);
    switch (msg->msgType) {
        case REQ:
            if (msg->resource == (Resource) state) {
                send->msgType = DEN;
            } else if (msg->resource == COSTUME) {
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

