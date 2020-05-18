/*
 * Jānis Kristaps Lūsis
 * JL17030
 * 17.05.2020.
*/

#include <fstream>

using namespace std;

// Constant declarations

const char* INPUT_FILE_NAME = "lidostas.in";
const char* OUTPUT_FILE_NAME = "lidostas.out";

const unsigned short int MIN_AIRPORT_NUMBER = 1;
const unsigned short int MIN_AIRPORT_COUNT = 2; // At least two airports exist since departure will always happen
const unsigned short int MAX_AIRPORT_COUNT = 20000;

const short int MONEY = 1000;
const short int WAIT_PENALTY = 1;

const unsigned short int MINUTES_IN_HOUR = 60;
const unsigned short int HOURS_IN_DAY = 24;
const unsigned short int MINUTES_IN_DAY = MINUTES_IN_HOUR * HOURS_IN_DAY;

const unsigned short int TRANSFER_TIME = 1;

const int MIN_FLIGHT_TIME = 30;
const int MAX_FLIGHT_TIME = MINUTES_IN_DAY - 1;

const char* OUTPUT_FLIGHT_PLAN_IMPOSSIBLE = "Impossible";

// Object declarations

struct Flight;
struct AirportDestination;
class AirportRepository;

// Function declarations

unsigned short int read_airport_count(fstream &input_stream);
unsigned short int read_airport(fstream &input_stream);
unsigned long long int read_time(fstream &input_stream);
void read_flights(fstream &input_stream, AirportRepository* repository);

unsigned long long int convert_time_to_minutes(const char* time);
void convert_minutes_to_time(unsigned short int minutes, char* to);
void copy_string(const char* from, char* to, int to_length);

struct Flight
{
    // Next flight since they are linked lists
    Flight* next = NULL;

    AirportDestination* parent;
    unsigned short int departure_time;
    unsigned short int arrival_time;

    bool flight_used = false; // TODO: maybe instantly delete?

    Flight(AirportDestination* parent, unsigned short int departure_time, unsigned short int arrival_time)
    {
        this->parent = parent;
        this->departure_time = departure_time;
        this->arrival_time = arrival_time;
    }

    ~Flight()
    {
        if (this->next) {
            delete this->next;
        }
    }

    unsigned short int getFlightDuration()
    {
        if (arrival_time < departure_time) {
            return arrival_time + MINUTES_IN_DAY - departure_time;
        }

        return arrival_time - departure_time;
    }
};

struct AirportDestination
{
    // Binary tree mapping for destinations for faster retrieval
    AirportDestination* left = NULL;
    AirportDestination* right = NULL;

    unsigned short int airport_from;
    unsigned short int airport_to;

    Flight* flights = NULL;

    AirportDestination(unsigned short int airport_from, unsigned short int airport_to)
    {
        this->airport_from = airport_from;
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

    bool hasLeftNode()
    {
        return (bool)this->left;
    }

    bool hasRightNode()
    {
        return (bool)this->right;
    }

    bool hasAtLeastOneFlight()
    {
        return (bool)this->flights;
    }
};

class AirportRepository
{
    unsigned short int airport_count;
    AirportDestination** airport_destinations;

    public:
    AirportRepository(unsigned short int airport_count)
    {
        if (airport_count > MAX_AIRPORT_COUNT || airport_count < MIN_AIRPORT_COUNT) {
            throw "Invalid airport count";
        }

        this->airport_count = airport_count;
        this->airport_destinations = new AirportDestination*[this->airport_count];

        for (int i = 0; i < airport_count; i++) {
            this->airport_destinations[i] = NULL;
        }
    }

    ~AirportRepository()
    {
        delete[] this->airport_destinations;
    }

    AirportDestination* getAirport(unsigned short int airport)
    {
        if (airport > MAX_AIRPORT_COUNT || airport < MIN_AIRPORT_NUMBER) {
            throw "Invalid airport passed";
        }

        return this->airport_destinations[airport - 1];
    }

    AirportDestination* getOrCreateDestination(unsigned short int airport_from, unsigned short int airport_to)
    {
        if (airport_from == airport_to) {
            throw "Can't have destination to same place";
        }

        if (airport_from > MAX_AIRPORT_COUNT || airport_from < MIN_AIRPORT_NUMBER) {
            throw "Invalid 'Airport from' passed";
        }

        if (airport_to > MAX_AIRPORT_COUNT || airport_to < MIN_AIRPORT_NUMBER) {
            throw "Invalid 'Airport to' passed";
        }

        // If there are no other destinations yet, create and return instantly since it's the first one
        if (!this->airport_destinations[airport_from - 1]) {
            this->airport_destinations[airport_from - 1] = new AirportDestination(airport_from, airport_to);

            return this->airport_destinations[airport_from - 1];
        }

        // Recursively loop through the binary tree until tree is found. If no child nodes contain the destination, create a new one
        return this->resolveDestination(this->airport_destinations[airport_from - 1], airport_from, airport_to);
    }

    void addFlight(AirportDestination* destination, unsigned short int departure_time, unsigned short int arrival_time)
    {
        if (!destination->hasAtLeastOneFlight()) {
            destination->flights = new Flight(destination, departure_time, arrival_time);

            return;
        }

        Flight* flight = destination->flights;
        while (flight->next != NULL) {
            if (flight->departure_time == departure_time && flight->arrival_time == arrival_time) {
                throw "Unexpected duplicate flight";
            }

            flight = flight->next;
        }

        flight->next = new Flight(destination, departure_time, arrival_time);
    }

    private:
    AirportDestination* resolveDestination(AirportDestination* destination, unsigned short int &airport_from, unsigned short int &airport_to)
    {
        if (destination->airport_to == airport_to) {
            return destination;
        }

        if (destination->airport_to > airport_to) {
            if (!destination->hasLeftNode()) {
                destination->left = new AirportDestination(airport_from, airport_to);
            }

            return resolveDestination(destination->left, airport_from, airport_to);
        }

        if (destination->airport_to < airport_to) {
            if (!destination->hasRightNode()) {
                destination->right = new AirportDestination(airport_from, airport_to);
            }

            return resolveDestination(destination->right, airport_from, airport_to);
        }

        throw "This should not happen";
    }
};

class AirportService
{
    AirportRepository* airport_repository;

    unsigned short int start_airport;
    unsigned short int destination_airport;
    unsigned long long int current_time;

    unsigned short int current_airport;
    unsigned short int money_spent = 0;

    public:
    AirportService(AirportRepository* airport_repository, unsigned short int start_airport, unsigned short int destination_airport, unsigned long long int time)
    {
        this->airport_repository = airport_repository;
        this->start_airport = start_airport;
        this->destination_airport = destination_airport;
        this->current_time = time;
    }

    // True if flight successful, false if not
    bool fly(fstream &output_stream)
    {
        this->arriveAtAirport(output_stream);

        while (this->moneyLeft() >= 0) {
            Flight* flight = this->getNextFlight();

            if (!flight) {
                return false; // If no flights are found, nothing we can do
            }

            if (this->current_time != flight->departure_time) {
                this->advanceTime(this->getTimeUntil(flight->departure_time), true);

                if (!this->moneyLeft()) {
                    return false; // If there is no more money, then nothing we can do
                }
            }

            this->current_airport = flight->parent->airport_to;
            flight->flight_used = true;

            char departure_time[6] = {};
            convert_minutes_to_time(flight->departure_time, departure_time);

            char arrival_time[6] = {};
            convert_minutes_to_time(flight->arrival_time, arrival_time);

            output_stream << flight->parent->airport_from << "->" << flight->parent->airport_to << " " << departure_time << "-" << arrival_time << endl;

            this->advanceTime(flight->getFlightDuration(), false);
            this->advanceTime(TRANSFER_TIME, true);

            if (this->current_airport == this->destination_airport) {
                return true;
            }
        }

        return false;
    }

    private:
    void arriveAtAirport(fstream &output_stream)
    {
        this->current_airport = this->start_airport;

        char start_time[6] = {};
        convert_minutes_to_time(this->current_time, start_time);

        output_stream << this->start_airport << " " << start_time << endl;

        this->advanceTime(TRANSFER_TIME, true); // When arriving, at least one minute is necessary for transfer
    }

    unsigned short int moneyLeft()
    {
        return MONEY - this->money_spent;
    }

    Flight* getNextFlight()
    {
        Flight* flight = NULL;

        findMinTimeFlight(this->airport_repository->getAirport(this->current_airport), flight);

        return flight;
    }

    // Walks all destinations in order and searches for the lowest time flight
    void findMinTimeFlight(AirportDestination *destination, Flight *&flight)
    {
        if (destination == NULL) {
            return;
        }

        findMinTimeFlight(destination->left, flight);

        for (Flight* potential_flight = destination->flights; potential_flight != NULL; potential_flight = potential_flight->next) {
            if (potential_flight->flight_used) {
                continue;
            }

            if (!flight) {
                flight = potential_flight;

                continue;
            }

            if (this->getTimeUntil(potential_flight->departure_time) < this->getTimeUntil(flight->departure_time)) {
                flight = potential_flight;
            }
        }

        findMinTimeFlight(destination->right, flight);
    }

    void advanceTime(unsigned short int minutes, bool is_waiting)
    {
        this->current_time += minutes;

        if (this->current_time >= MINUTES_IN_DAY) {
            this->current_time -= MINUTES_IN_DAY;
        }

        if (is_waiting) {
            this->money_spent += minutes;
        }
    }

    unsigned short int getTimeUntil(unsigned short int time)
    {
        short int time_until = time - this->current_time;

        if (time_until < 0) {
            time_until += MINUTES_IN_DAY;
        }

        return time_until;
    }
};

void lidostas()
{
    // Init

    fstream input_stream(INPUT_FILE_NAME, ios::in);

    AirportRepository repository(read_airport_count(input_stream));

    // read_airport part didn't work on APTS for some reason but did work locally; possibly something related to how constructor processes parameters
    AirportService service(&repository, read_airport(input_stream), read_airport(input_stream), read_time(input_stream));
    read_flights(input_stream, &repository);

    input_stream.close();

    // Process
    fstream output_stream(OUTPUT_FILE_NAME, ios::out);

    bool flight_plan_successful = service.fly(output_stream);
    output_stream.close();

    // If successful, then we are happy with the results and we can quit
    if (flight_plan_successful) {
        return;
    }

    // Else clean the previous file and output 'Impossible'
    fstream override_output_stream(OUTPUT_FILE_NAME, ios::out | ios::trunc);

    override_output_stream << OUTPUT_FLIGHT_PLAN_IMPOSSIBLE << endl;

    override_output_stream.close();
}

// Main
int main()
{
    try {
        lidostas();
    } catch (const char* exception) {
        return 13;
    }

    return 0;
}

// Functions

unsigned short int read_airport_count(fstream &input_stream)
{
    unsigned short int airport_count;
    
    input_stream >> airport_count;

    return airport_count;
}

unsigned short int read_airport(fstream &input_stream)
{
    unsigned short int airport;

    input_stream >> airport;

    if (airport < MIN_AIRPORT_NUMBER || airport > MAX_AIRPORT_COUNT) {
        throw "Invalid airport number";
    }

    return airport;
}

unsigned long long int read_time(fstream &input_stream)
{
    char time[6] = {};

    input_stream >> time;

    unsigned long long int time_in_minutes = convert_time_to_minutes(time);

    return time_in_minutes;
}

void read_flights(fstream &input_stream, AirportRepository* repository)
{
    while (true) {
        // Read airport from / airport to and create the destination
        unsigned short int airport_from;
        unsigned short int airport_to;

        input_stream >> airport_from;

        // 0 - EOF
        if (airport_from == 0) {
            break;
        }

        input_stream >> airport_to;
    
        AirportDestination* destination = repository->getOrCreateDestination(airport_from, airport_to);

        // Read flight count and create flights

        unsigned short int flight_count;
        input_stream >> flight_count;

        char flight_details[12] = {};
        for (int i = 0; i < flight_count; i++) {
            input_stream >> flight_details;

            char time[6] = {};
            
            // Get the departure time
            for (int i = 0; i < 5; i++) {
                time[i] = flight_details[i]; 
            }

            unsigned short int departure_time = convert_time_to_minutes(time);

            // Get the arrival time
            for (int i = 0; i < 5; i++) {
                time[i] = flight_details[i + 6]; // Needs +6 offset since HH:MM-{H}H:MM
            }
            
            unsigned short int arrival_time = convert_time_to_minutes(time);

            repository->addFlight(destination, departure_time, arrival_time);
        }
    }
}

// Helper-helpers

unsigned long long int convert_time_to_minutes(const char* time)
{  
    // Extract hours
    unsigned short int hours = 0;

    int current_number = (int)time[0] - 48;
    if (current_number < 0 || current_number > 2) {
        throw "{H}H:MM invalid";
    }

    hours = current_number * 10;

    current_number = (int)time[1] - 48;
    if (current_number < 0 || (hours > 19 && current_number > 3)) {
        throw "H{H}:MM invalid";
    }

    hours += current_number;

    // Extract minutes
    unsigned short int minutes = 0;

    current_number = (int)time[3] - 48;
    if (current_number < 0 || current_number > 5) {
        throw "HH:{M}M invalid";
    }

    minutes += current_number * 10;

    current_number = (int)time[4] - 48;
    if (current_number < 0 || current_number > 9) {
        throw "HH:M{M} invalid";
    }

    minutes += current_number;

    return (hours * MINUTES_IN_HOUR) + minutes;
}

void convert_minutes_to_time(unsigned short int minutes, char* to)
{
    char time[6] = {};

    time[0] = (minutes / 60 / 10) + 48; // Get {H}H:MM
    time[1] = (minutes / 60 % 10) + 48; // Get H{H}:MM
    time[2] = ':';
    time[3] = (minutes % 60 / 10) + 48; // Get HH:{M}M
    time[4] = (minutes % 60 % 10) + 48; // Get HH:M{M}

    copy_string(time, to, 5);
}

void copy_string(const char* from, char* to, int to_length)
{
    for (int i = 0; from[i] != 0; i++) {
        if (i > to_length) {
            return;
        }

        to[i] = from[i];
    }
}