//
// Created by er713 on 6/15/20.
//

#include "Voyager.h"
#include <cstdio>
#include <unistd.h>


Voyager::Voyager(int id, int size) : Logger(id, START) {
    this->size = size;
    rng.seed(time(nullptr) ^ (id<<16));
    i("Zaczynamy");
}

void Voyager::operator()() {
    wait_FOR_COSTUME();
}

void Voyager::wait_FOR_COSTUME() {  // TODO: wysyłaj z poprzednim timestamp'em
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
    i("przed", msg);
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

//    i("po");
    delete msg;
}

void Voyager::handle_START(Message *msg) {
    auto response = Message(timestamp, id, msg->sender_id);

    switch (msg->msgType) {

        case DEN:
        case REP: //
        case ACK:
            e("To nie powinno sie wydarzyć", msg);
            break;
        case TIC:
            response.msgType = DEN;
            response.send();
            break;
        case OUT: // nie robi nic
            break;
        case REQ:
            resources_on_REQ(&response, msg);
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
                send->msgType = REP;
                send->data = 0;
            }
            send->send();
            break;
        case DEN:
            wasDEN = true;
            break;
        case REP:
            count += msg->data;
            count_all++;
            check_VALID_COSTUME();
            break;
        case TIC:
            send->msgType = DEN;
            send->send();
            break;
        default:
            e("To sie nie powinno było wydarzyć", msg);
            break;
    }
    delete send;
}

void Voyager::check_VALID_COSTUME() {
    if (count_all == size - 1) {
        if (count + 1 <= COSTUME_QUANTITY && !wasDEN) {
            int rand = get_RANDOM_NUMBER(0, VESSEL_QUANTITY - 1);
            state = static_cast<State>(rand);  // ubieganie sie o randomowy statek
            costume = COSTUME; // dopisałem dwie zmienne od kostiumu i statku (w logger),

            auto msg = Message(timestamp, id); // żadanie statku po uzyskaniu kostiumu
            msg.msgType = REQ;
            msg.resource = static_cast<Resource>(state);
            msg.broadcast(size);
        } else {
            std::thread thread(std::ref(*this));
        }
        count = count_all = 0;
        wasDEN = false;
    }
}

void Voyager::handle_HAVE_VESSEL(Message *msg) {
    auto response = Message(timestamp, id, msg->sender_id);

    switch (msg->msgType) {

        case REQ:
            resources_on_REQ(&response, msg);
            response.send();
            break;
        case DEN:
        case REP:
        case ACK:
            e("Nie powinno się zdarzyć", msg);
            break;
        case TIC:
            response.msgType = DEN;
            response.send();
            break;
        case OUT: // wypłynięcie
            if (msg->resource == vessel) {
                state = SIGHTSEEING;
                std::thread thread(&Voyager::sightseeing, this, msg->data);
            }
            break;

    }

}

void Voyager::handle_WANT_DEPARTURE(Message *msg) { // TODO: rozważyć usunięcie założenia o maksymalnym czasie transmisji (być może poprzez rozwinięcie structury o State state_in_request)
    auto response = Message();

    switch (msg->msgType) {

        case REQ:
            resources_on_REQ(&response, msg);
            response.send();
            break;
        case DEN:
            ++count_all;
            break;
        case REP:
            e("Nie powinno się zdarzyć", msg);
            break;
        case TIC:
            response.msgType = DEN;
            response.send();
            break;
        case ACK:
            ++count_all;
            got_ACK = true;
            break;
        case OUT:
            if (msg->resource == vessel) {
                state = SIGHTSEEING;
                std::thread thread(&Voyager::sightseeing, this, msg->data);
            }
            break;
    }

    if (count_all == size - 1) { // kiedy otrzyma wszystkie odpowiedzi

        if (got_ACK) {
            state = HAVE_VESSEL;
            count_all = 0;
            count = 0;
        } else {
            auto out = Message(timestamp, id);
            out.msgType = OUT;
            out.data = get_RANDOM_NUMBER(10000, 60000);
            out.resource = vessel;
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
                send->data = 1;
            } else {
                send->msgType = REP;
            }
            break;
        case TIC:
            send->msgType = DEN;
            break;
        case OUT: // uznałem, że może się zdarzyć, że dwa lub więcej może się ubiegać o wpłynięcie, a skoro już wypłyneli to ignoruje to
        case DEN:
        case ACK:
            delete send;
            return;
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

void Voyager::resources_on_REQ(Message *response, Message *msg) {
    response->msgType = REP;
    if (msg->resource == COSTUME) {
        response->data = (COSTUME == costume) ? 1 : 0;
    } else {
        response->data = (msg->resource == vessel) ? volume : 0;
    }
}

void Voyager::sightseeing(int time) {
    std::this_thread::sleep_for(std::chrono::milliseconds(time));
    mutex.lock();
    state = START;
    costume = static_cast<Resource>(-1);
    vessel = static_cast<Resource>(-1);
    count = 0;
    count_all = 0;
    mutex.unlock();
    wait_FOR_COSTUME();
}

