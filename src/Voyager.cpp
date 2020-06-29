//
// Created by er713 on 6/15/20.
//

#include "Voyager.h"
#include <unistd.h>
//#include <fcntl.h>

const int Voyager::vessel_capacity[];

Voyager::Voyager(int id, int size, MPI_Comm thread_comm) : Logger(id, START) {
//    messageToServe = new std::list<std::function<void(void)>>();
//    sem_init(semaphore, 0, 0);
    this->size = size;
    this->thread_comm = thread_comm;
    unsigned int seed = time(nullptr);
//    unsigned int seed = 1592725966;
    char err[250];
    sprintf(err, "seed: %d", seed);
    i(err);
    srand(seed ^ (id << 16));
    rng.seed(seed ^ (id << 16));
    got_TIC_for = new std::vector<Message>();
    volume = get_RANDOM_NUMBER(1, MAX_VOYAGER_VOLUME); // losowanie ile miejsca zajmuje dany turysta
    i("Zaczynamy");
}

void Voyager::operator()() {
    wait_FOR_COSTUME();
}

void Voyager::wait_FOR_COSTUME() {  // TODO: wysyłaj z poprzednim timestamp'em
    i("śpię");
    std::this_thread::sleep_for(std::chrono::milliseconds(get_RANDOM_NUMBER(500, 5000)));

    i("nie śpię");
    mutex.lock();
    state = REQUESTING_COSTUME;
    i("Zaczynam domagać się kostiumu!");
    auto send = new Message(timestamp, id, 0);
    send->timestamp = (sent_timestamp != -1) ? (unsigned int) sent_timestamp : timestamp;
    if (sent_timestamp == -1) {
        sent_timestamp = (int) timestamp;
    }
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

    timestamp = std::max(timestamp, msg->current_timestamp) + 1; // aktualizowanie zegaru Lamporta

    switch (state) {
        case START:
            costume = static_cast<Resource>(-1);
            vessel = static_cast<Resource>(-1);
            handle_START(msg);
            handle_START(msg);
            break;
        case REQUESTING_COSTUME:
            costume = static_cast<Resource>(-1);
            vessel = static_cast<Resource>(-1);
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
            break;
        case REP: //
            break;
        case ACK:
//            e("To nie powinno sie wydarzyć", msg);
            break;
        case TIC:
            response.msgType = NOPE;
            response.send();
            break;
        case OUT: // nie robi nic
            break;
        case NOPE:
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
            if (msg->resource == COSTUME && (msg->timestamp > (unsigned int) (sent_timestamp != -1) ? sent_timestamp : timestamp
                                                                                                                       || (msg->timestamp == (unsigned int) (sent_timestamp != -1) ? sent_timestamp : timestamp &&
                                                                                                                                                                                                      msg->sender_id >
                                                                                                                                                                                                      id))) {
                send->msgType = DEN;
            } else {
                send->msgType = REP;
                send->data = 0;
            }
            send->send();
            break;
        case DEN:
            count_all++;
            wasDEN = true;
            check_VALID_COSTUME();
            break;
        case REP:
            count += msg->data;
            count_all++;
            check_VALID_COSTUME();
            break;
        case TIC:
            send->msgType = NOPE;
            send->send();
            break;
        case NOPE: // ignorujemy
        case OUT:
            break;
        default:
//            e("To sie nie powinno było wydarzyć", msg);
            break;
    }
    delete send;
}

void Voyager::check_VALID_COSTUME() {
    if (count_all == size - 1) {
        if (wasDEN) {
//            if (thread != nullptr) {
//                delete thread;
//                thread = nullptr;
//            }
//            thread = new std::thread(std::ref(*this));
//            thread.detach();
            start_REQUESTIN_COSTUME(this, false);
        } else if (count + 1 > COSTUME_QUANTITY) {
            auto *attr = new pthread_attr_t;
            pthread_attr_init(attr);
            pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
            pthread_create(&pthread, attr, reinterpret_cast<void *(*)(void *)>(Voyager::wait_FOR_COSTUME2), this);
        } else {
            costume = COSTUME; // dopisałem dwie zmienne od kostiumu i statku (w logger),
            sent_timestamp = -1;
            i("Dostałem kostium!");
            start_REQUESTING_VESSEL();
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
            break;
        case REP:
            break;
        case ACK:
            break;
        case NOPE:
//            e("Nie powinno się zdarzyć", msg);
            break;
        case TIC:
            response.msgType = NOPE;
            response.send();
            break;
        case OUT: // wypłynięcie
            if (msg->resource == vessel) {
                start_SIGHTSEEING(msg->data);
            }
            break;

    }

}

// TODO: rozważyć usunięcie założenia o maksymalnym czasie transmisji (być może poprzez rozwinięcie structury o State state_in_request)
void Voyager::handle_WANT_DEPARTURE(Message *msg) {
    auto response = Message(timestamp, id, msg->sender_id);

    switch (msg->msgType) {

        case REQ:
            resources_on_REQ(&response, msg);
            response.send();
            break;
        case NOPE:
            ++count_all;
            break;
        case REP:
            break;
        case DEN:
//            e("Nie powinno się zdarzyć", msg);
            break;
        case TIC:
            response.msgType = NOPE;
            response.receiver_id = msg->sender_id;
            response.send();
            break;
        case ACK:
            ++count_all;
            got_ACK = true;
            break;
        case OUT:
            if (msg->resource == vessel) {
                start_SIGHTSEEING(msg->data);
            }
            break;
    }

    if (count_all == size - 1) { // kiedy otrzyma wszystkie odpowiedzi
        count_all = count = 0;
        if (got_ACK) {
            state = HAVE_VESSEL;
        } else {
            auto out = Message(timestamp, id);
            out.msgType = OUT;
            out.data = get_RANDOM_NUMBER(10, 20);
            out.resource = vessel;
            out.broadcast(size);
            start_SIGHTSEEING(out.data);
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
            send->msgType = NOPE;
            break;
        case OUT: // uznałem, że może się zdarzyć, że dwa lub więcej może się ubiegać o wpłynięcie, a skoro już wypłyneli to ignoruje to
            break;
        case NOPE:
            break;
        case ACK:
            delete send;
            return;
        default:
//            e("To sie nie powinno było wydarzyć", msg);
            delete send;
            return;
    }
    send->send();
    delete send;
}

void Voyager::handle_REQUESTING_VESSEL(Message *msg) {
    Message response(timestamp, id, msg->sender_id);

    switch (msg->msgType) {

        case REQ:
            if (msg->resource != static_cast<Resource>(state)) {
                resources_on_REQ(&response, msg);
            } else {
                if (msg->timestamp > (unsigned int) sent_timestamp || (msg->timestamp == (unsigned int) sent_timestamp && msg->sender_id > id)) {
                    response.msgType = DEN;
                } else {
                    response.msgType = REP;
                    response.data = 0;
                }
                response.send();
            }
            break;
        case DEN:
            ++count_all;
            wasDEN = true;
            break;
        case REP:
            ++count_all;
            count += msg->data;
            break;
        case TIC:
            if (volume > vessel_capacity[msg->resource] - msg->data) {
                response.msgType = NOPE;
                response.send();
            } else {
                if (msg->resource == static_cast<Resource>(state)) {
                    response.msgType = ACK;
                    response.send();
                } else {
                    got_TIC_for->push_back(Message(msg));
                }
            }
            break;
        case ACK:
            break;
        case NOPE:
//            e("nie powinno wystąpić", msg);
            break;
        case OUT: // ignoruje
            break;
    }

    if (count_all == size - 1) {
        if (!wasDEN && count + volume <= Voyager::vessel_capacity[state]) { // uzyskanie statku
            vessel = static_cast<Resource>(state);
            state = HAVE_VESSEL;
            sent_timestamp = -1;
            i("Dostałem statek!");
            if (!got_TIC_for->empty()) { // odpowiedzi na TIC, odmowa
                Message den(timestamp, id);
                den.msgType = NOPE;
                for (auto m: *got_TIC_for) {
                    den.receiver_id = m.sender_id;
                    den.send();
                }
            }
            if (Voyager::vessel_capacity[vessel] == count + volume) { // sprawdzanie czy pełen
                response.msgType = OUT;
                response.resource = vessel;
                response.data = get_RANDOM_NUMBER(10, 20);
                response.broadcast(size);
                start_SIGHTSEEING(response.data);
            } else if (vessel_capacity[vessel] - (count + volume) <= MAX_VOYAGER_VOLUME) { // sprawdzanie czy WANT_DEPARTURE
                state = WANT_DEPARTURE;
                i("Próbuję wypłynąć! " + std::to_string(count + volume));
                response.msgType = TIC;
                response.data = count + volume;
                response.resource = vessel;
                response.broadcast(size);
            }
        } else { // nie uzyskanie miejsca w statku
            if (!got_TIC_for->empty()) {
                int mi = 0;
                if (got_TIC_for->size() > 1) { // odmowy dla innych niż pierwszy
                    // małe dopasowanie aby następnie ubiegać się o statek do którego najlepiej się dopsuje/pozostawi najmniej miejsca
                    int min = vessel_capacity[got_TIC_for->at(0).resource] - (got_TIC_for->at(0).data + volume);
                    for (size_t i = 1; i < got_TIC_for->size(); ++i) {
                        if (min > vessel_capacity[got_TIC_for->at(i).resource] - (got_TIC_for->at(i).data + volume)) {
                            mi = (int) i;
                            min = vessel_capacity[got_TIC_for->at(i).resource] - (got_TIC_for->at(i).data + volume);
                        }
                    }

                    Message den(timestamp, id);
                    den.msgType = NOPE;
                    for (size_t i = 1; i < got_TIC_for->size(); ++i) {
                        if (i != (size_t) mi) {
                            den.receiver_id = got_TIC_for->at(i).sender_id;
                            den.send();
                        }
                    }
                }
                response.msgType = ACK;
                response.receiver_id = got_TIC_for->at(mi).sender_id;
                response.send();
                start_REQUESTING_VESSEL(got_TIC_for->at(mi).resource);
                got_TIC_for->resize(0);
            } else { // jeśli nie otrzymał, żadnego TICa
                start_REQUESTING_VESSEL();
            }
        }
        wasDEN = false;
        count = count_all = 0;
    }

}

void Voyager::start_REQUESTING_VESSEL() {
    int rand = get_RANDOM_NUMBER(0, VESSEL_QUANTITY - 1);
    state = static_cast<State>(rand);  // ubieganie sie o randomowy statek
    i("Zaczynam domagać się statku!");

    auto msg = Message(timestamp, id); // żadanie statku po uzyskaniu kostiumu
    msg.timestamp = (sent_timestamp == -1) ? timestamp : (unsigned int) sent_timestamp;
    if (sent_timestamp == -1) {
        sent_timestamp = (int) timestamp;
    }
    msg.msgType = REQ;
    msg.resource = static_cast<Resource>(state);
    msg.broadcast(size);
}

void Voyager::start_REQUESTING_VESSEL(Resource resource) {
    state = static_cast<State>(resource);
    i("Zaczynam domagać się statku!");

    auto msg = Message(timestamp, id); // żadanie statku po uzyskaniu kostiumu
    msg.timestamp = (sent_timestamp == -1) ? timestamp : (unsigned int) sent_timestamp;
    if (sent_timestamp == -1) {
        sent_timestamp = (int) timestamp;
    }
    msg.msgType = REQ;
    msg.resource = static_cast<Resource>(state);
    msg.broadcast(size);
}

int Voyager::get_RANDOM_NUMBER(int a, int b) {
    std::uniform_int_distribution<std::mt19937::result_type> dist_ab(a, b);
    return dist_ab(rng);
}

void Voyager::resources_on_REQ(Message *response, Message *msg) {
    response->msgType = REP;
    response->receiver_id = msg->sender_id;
    if (msg->resource == COSTUME) {
        response->data = (COSTUME == costume) ? 1 : 0;
    } else {
        response->data = (msg->resource == vessel) ? volume : 0;
    }
}

void Voyager::sightseeing(int time) {
    i("śpię");
    std::this_thread::sleep_for(std::chrono::milliseconds(time));
    i("nie śpię");
    mutex.lock();
    state = START;
    i("Zaczynam odpoczywać!");
    costume = static_cast<Resource>(-1);
    vessel = static_cast<Resource>(-1);
    count = 0;
    count_all = 0;
    mutex.unlock();
    wait_FOR_COSTUME();
}

void Voyager::start_SIGHTSEEING(int time) {
    state = SIGHTSEEING;
    i("Zaczynam zwiedzać!");
//    std::thread thread(&Voyager::sightseeing, this, time);
//    thread.detach();
    time_to_sleep = time;
    auto *attr = new pthread_attr_t;
    pthread_attr_init(attr);
    pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&pthread, attr, reinterpret_cast<void *(*)(void *)>(Voyager::sightseeing2), this);
//    pthread_detach(pthread);
}

Voyager::~Voyager() {
    i("!!!!!!!!Zabiłem się!!!!!!!");
//    running_computing_thread = false;

//    sem_destroy(semaphore);
//    delete semaphore;
    delete got_TIC_for;
//    delete messageToServe;
}

void Voyager::wait_FOR_COSTUME2(void *voyager) {
    auto *th = static_cast<Voyager *>(voyager);
    th->i("spanko");
    sleep(rand() % 15 + 5);
    start_REQUESTIN_COSTUME(th, true);
    return;
}

void Voyager::sightseeing2(void *voyager) {
    auto *th = static_cast<Voyager *>(voyager);
    th->i("zwiedzanie");
    sleep((unsigned int) th->time_to_sleep);
//    sleep(1);
    th->mutex.lock();
    th->i("po zwiedzaniu");
    th->state = START;
    th->i("Zaczynam odpoczywać!");
    th->costume = static_cast<Resource>(-1);
    th->vessel = static_cast<Resource>(-1);
    th->count = 0;
    th->count_all = 0;
    th->mutex.unlock();
    wait_FOR_COSTUME2(voyager);
    th->i("Po odpoczywaniu");
    pthread_exit(nullptr);
}

void Voyager::start_REQUESTIN_COSTUME(Voyager *th, bool const lock) {
    if (lock) {
        th->mutex.lock();
    }
    th->state = REQUESTING_COSTUME;
    th->count_all = th->count = 0;
//    th->i("Zaczynam domagać się kostiumu!");
    auto send = new Message(th->timestamp, th->id, 0);
    send->timestamp = (th->sent_timestamp != -1) ? (unsigned int) th->sent_timestamp : th->timestamp;
    if (th->sent_timestamp == -1) {
        th->sent_timestamp = (int) th->timestamp;
    }
    send->msgType = REQ;
    send->resource = COSTUME;
    send->broadcast(th->size);
    delete send;
    if (lock) {
        th->mutex.unlock();
    }
}

//void Voyager::computing_thread(void *voyager) {
//    auto *th = static_cast<Voyager *>(voyager);
//
//    while (th->running_computing_thread) {
//        sem_close(th->semaphore);
//        if (!th->messageToServe->empty()) {
//            th->messageToServe->front().operator()();
//            th->messageToServe->pop_front();
//        } else {
//            th->i("!!!!brak zadan!!!!");
//        }
//    }
//
//}
