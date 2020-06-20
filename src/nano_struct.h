#ifndef NANO_STRUCT
#define NANO_STRUCT

#include <string>
#include <mpi.h>

#define VESSEL_QUANTITY 10
#define COSTUME_QUANTITY 190
#define MAX_VOYAGER_VOLUME 7
#define TAG 0


enum State : int { // od 0 do VESSEL_Q-1 są żądania poszczególnych łodzi, a potem jak widać
    START = VESSEL_QUANTITY, REQUESTING_COSTUME, HAVE_VESSEL, SIGHTSEEING, WANT_DEPARTURE
};

enum MessageType : int { // opisane w sprawozdaniu/jakimś opisie algorytmu
    REQ, DEN, REP, TIC, ACK, OUT, NOPE
};

enum Resource : int { // od 0 do VESSEL_Q-1 są to łodzie, a potem kostium
    COSTUME = VESSEL_QUANTITY
};

struct Message {
    int sender_id; // chyba się dało otrzymywać dane wysyłajacego bez ich przesyłania, ale to najwyżej później się ogranie
    int receiver_id;
    int info_type;
    unsigned int timestamp;
    unsigned int current_timestamp;
    int data; // zacząłem używać tego jako ilość zasobów
    MessageType msgType;
    Resource resource; // w odpowiedziach zdaje się opcjonalne

    Message();

    Message(Message *message);

    Message(unsigned int timestamp, int sender, int receiver = 0);

    void send();

    void broadcast(int quantity_of_voyagers);
};

class Singleton {
private:
    static Singleton *INSTANCE;
    MPI_Datatype mpi_message_type{};

    void create_custom_message_type();

    Singleton();

public:
    static Singleton getInstance();

    MPI_Datatype getDataType() { return mpi_message_type; }
};

char *state_to_string(State state);

#endif