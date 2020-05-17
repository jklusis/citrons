/*
 * Jānis Kristaps Lūsis
 * JL17030
 * 17.05.2020.
*/

#include <iostream>
#include <fstream>

using namespace std;

// Constant declarations

const char* INPUT_FILE_NAME = "lidostas.in";
const char* OUTPUT_FILE_NAME = "lidostas.out";

// At least two airports exist since departure will always happen
const unsigned short int MIN_AIRPORT_COUNT = 2; 
const unsigned short int MAX_AIRPORT_COUNT = 20000;

const short int MONEY = 1000;
const short int WAIT_PENALTY = 1;

const unsigned short int MINUTES_IN_HOUR = 60;
const unsigned short int HOURS_IN_DAY = 24;
const unsigned short int MINUTES_IN_DAY = MINUTES_IN_HOUR * HOURS_IN_DAY;

const unsigned short int TRANSFER_TIME = 1;

// Object declarations
struct Flight
{
    // Next flight since they are linked lists
    Flight* next = NULL;

    unsigned short int departure_time;
    unsigned short int arrival_time;

    Flight(unsigned short int departure_time, unsigned short int arrival_time)
    {
        this->departure_time = departure_time;
        this->arrival_time = arrival_time;
    }

    ~Flight()
    {
        if (this->next) {
            delete this->next;
        }
    }
};

struct AirportDestination
{
    // Binary tree mapping for destinations for faster retrieval
    AirportDestination* left = NULL;
    AirportDestination* right = NULL;

    unsigned short int airport_to;
    Flight* flights = NULL;

    AirportDestination(unsigned short int airport_to)
    {
        this->airport_to = airport_to;
    }

    ~AirportDestination()
    {
        if (this->hasLeftNode()) {
            delete this->left;
        }

        if (this->hasRightNode()) {
            delete this->right;
        }

        if (this->hasAtLeastOneFlight()) {
            delete this->flights;
        }
    }

    bool hasAtLeastOneFlight()
    {
        return (bool)this->flights;
    }

    bool hasLeftNode()
    {
        return (bool)this->left;
    }

    bool hasRightNode()
    {
        return (bool)this->right;
    }
};

class AirportRepository
{
    AirportDestination** airport_destinations;

    public:
    AirportRepository(unsigned short int airport_count)
    {
        if (airport_count > MAX_AIRPORT_COUNT || airport_count < 2) {
            throw "Invalid airport count";
        }

        this->airport_destinations = new AirportDestination*[airport_count];
    }

    ~AirportRepository()
    {
        delete[] this->airport_destinations;
    }
};

int main()
{
    AirportRepository(50);

    return 0;
}