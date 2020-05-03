/*
 * Jānis Kristaps Lūsis
 * JL17030
 * 01.05.2020.
*/

#include <fstream>

using namespace std;

// Constant declarations

const char* INPUT_FILE_NAME = "hair.in";
const char* OUTPUT_FILE_NAME = "hair.out";

const int MAX_TIMESTAMP = 2000000000;

const int MIN_BARBER_COUNT = 1;
const int MAX_BARBER_COUNT = 9;

const int MAX_CUSTOMER_COUNT = 200000;

const int END_SYMBOL = 0;

// Object declarations

// Struct that contains current row from file
struct RowData 
{
    int timestamp = 0;
    int customer_number = 0;
    int customer_job_duration = 0;
};

class Barber
{
    int number = 0;

    int last_job_at = 0;

    int customer_number = 0;
    int customer_started_at = 0;
    int customer_job_duration = 0;

    public:
    void setNumber(int number) 
    {
        if (number > MAX_BARBER_COUNT || number < MIN_BARBER_COUNT) {
            throw "Invalid barber number";
        }

        this->number = number;
    }

    int getNumber() 
    {
        return this->number;
    }

    int getTimeSinceLastJob() 
    {
        return this->last_job_at;
    }

    int getCustomerNumber() 
    {
        return this->customer_number;
    }

    int getCustomerJobFinishedAt() 
    {
        return this->customer_started_at + this->customer_job_duration - 1;
    }

    void setCustomer(int customer_number, int current_timestamp, int customer_job_duration) 
    {
        this->customer_number = customer_number;
        this->customer_started_at = current_timestamp;
        this->customer_job_duration = customer_job_duration;
    }

    void clearCustomer() 
    {
        this->last_job_at = this->getCustomerJobFinishedAt();

        this->customer_number = 0;
        this->customer_started_at = 0;
        this->customer_job_duration = 0;
    }

    bool isBusy() 
    {
        return (bool)this->customer_started_at;
    }

    bool isOnVacation(int timestamp) 
    {
        return timestamp % 1000 / 100 == this->number;
    }

    bool willBeOnVacation(int timestamp, int customer_job_duration) 
    {
        for (int i = timestamp; i < timestamp + customer_job_duration; i++) {
            if (this->isOnVacation(i)) {
                return true;
            }
        }

        return false;
    }
};


// Services

class BarberService
{
    int barber_count = 0;
    Barber barbers[MAX_BARBER_COUNT]; // Could be done also with pointers but avoiding mess with garbage collection

    public:
    BarberService(int barber_count) 
    {
        if (barber_count > MAX_BARBER_COUNT || barber_count < MIN_BARBER_COUNT) {
            throw "Invalid barber count";
        }

        this->barber_count = barber_count;

        for (int barber_number = 1; barber_number <= this->barber_count; barber_number++) {
            this->barbers[barber_number - 1].setNumber(barber_number);
        }
    }

    int getBarberCount() 
    {
        return this->barber_count;
    }

    Barber* getBarberByNumber(int number) 
    {
        if (number > this->barber_count || number < MIN_BARBER_COUNT) {
            throw "Invalid barbercount";
        }

        return &this->barbers[number - 1];
    }

    // Finds an available barber that is not on a vacation. If no barbers exist, null pointer is returned.
    Barber* getBarberForJob(int current_timestamp, int customer_job_duration) 
    {
        // Mainīgais kas satur pointeri uz objektu; objektu var mainīt, piešķirot citu pointera vērtību.
        Barber* selected_barber = NULL;

        for (int barber_number = 1; barber_number <= this->barber_count; barber_number++) {
            Barber* potential_barber = &this->barbers[barber_number - 1];

            if (potential_barber->isBusy()) {
                continue;
            }

            if (potential_barber->isOnVacation(current_timestamp)) {
                continue;
            }

            if (potential_barber->willBeOnVacation(current_timestamp, customer_job_duration)) {
                continue;
            }

            if (!selected_barber) {
                selected_barber = potential_barber;

                continue;
            }

            if (selected_barber->getTimeSinceLastJob() > potential_barber->getTimeSinceLastJob()) {
                selected_barber = potential_barber;
            }

            // No need to compare barber numbers since they are processed in asceding order
        }

        return selected_barber;
    }
};

class CustomerService
{
    int customers[MAX_CUSTOMER_COUNT] = {}; // customer_number - 1 => customer_job_duration; initialise it empty
    
    int total_customers = 0; // How many customers have signed up for a hairdresser
    int now_serving = 0; // Index in queue which customer should now be served; also counts as how many have been assigned to jobs.
    int total_served_customers = 0; // How many customers have received the hairdressing service

    public:
    void addToQueue(int customer_number, int job_duration)
    {
        this->customers[customer_number - 1] = job_duration;
        this->total_customers++;
    }

    bool isNextInQueueSet()
    {
        if (this->total_customers == 0 || this->now_serving >= this->total_customers) {
            return false;
        }

        return this->customers[this->now_serving] != 0; // If index is not zero, then duration is set and therefore customer too
    }

    // Returns customer_number which is next in queue or 0 if queue is empty
    int getNextInQueue()
    {
        if (!this->isNextInQueueSet()) {
            return 0;
        }

        return this->now_serving + 1;
    }

    int getJobDurationForNextInQueue()
    {
        if (!this->isNextInQueueSet()) {
            return 0;
        }

        return this->customers[this->now_serving];
    }

    void advanceQueue()
    {
        this->now_serving++;
    }

    void advanceServedCount()
    {
        this->total_served_customers++;
    }

    bool areAllCustomersProcessed()
    {
        return this->total_served_customers >= this->total_customers;
    }
};

class BarberJobService
{
    BarberService* barber_service;
    CustomerService* customer_service;

    const int I_JOB_FINISHED_AT = 0;
    const int I_CUSTOMER_NUMBER = 1; 
    int finished_jobs[MAX_BARBER_COUNT][2] = {}; // barber_number - 1 => [job_finished_at, customer_number]; initially fill with zeroes

    public:
    BarberJobService(BarberService *barber_service, CustomerService *customer_service) 
    {
        this->barber_service = barber_service;
        this->customer_service = customer_service;
    }

    bool assignJob(int current_timestamp, int customer_number, int customer_job_duration)
    {
        Barber* barber = this->barber_service->getBarberForJob(current_timestamp, customer_job_duration);

        if (!barber) {
            return false;
        }
        
        barber->setCustomer(customer_number, current_timestamp, customer_job_duration);

        return true;
    }

    void processStartedJobs(int current_timestamp, fstream &output_stream)
    {
        if (current_timestamp < 1) {
            return;
        }

        int finished_job_count = 0;

        for (int barber_number = 1; barber_number <= this->barber_service->getBarberCount(); barber_number++) {
            Barber* barber = this->barber_service->getBarberByNumber(barber_number);

            if (barber->getCustomerNumber() && barber->getCustomerJobFinishedAt() < current_timestamp) {
                this->addFinishedJob(
                    barber->getCustomerJobFinishedAt(), 
                    barber->getNumber(), 
                    barber->getCustomerNumber()
                );

                barber->clearCustomer();
                finished_job_count++;
            }
        }

        if (finished_job_count) {
            this->printFinishedJobs(output_stream, finished_job_count);
        }
    }

    private:
    void addFinishedJob(int job_finished_at, int barber_number, int customer_number)
    {
        this->finished_jobs[barber_number - 1][I_JOB_FINISHED_AT] = job_finished_at;
        this->finished_jobs[barber_number - 1][I_CUSTOMER_NUMBER] = customer_number;
        
        this->customer_service->advanceServedCount();
    }

    // Loops through all jobs by finding the oldest ones, printing them out and clearing them afterwards
    void printFinishedJobs(fstream &output_stream, int finished_job_count)
    {
        for (int i = 0; i < finished_job_count; i++) {
            int lowest_job_finished_at = 0;
            int selected_barber_number = 0;

            for (int barber_number = 1; barber_number <= this->barber_service->getBarberCount(); barber_number++) {
                int job_finished_at = this->finished_jobs[barber_number - 1][I_JOB_FINISHED_AT];

                if (job_finished_at && (!lowest_job_finished_at || lowest_job_finished_at < job_finished_at)) {
                    lowest_job_finished_at = lowest_job_finished_at;
                    selected_barber_number = barber_number;
                }
            }

            output_stream << this->finished_jobs[selected_barber_number - 1][I_JOB_FINISHED_AT] << " " << selected_barber_number << " " << this->finished_jobs[selected_barber_number - 1][I_CUSTOMER_NUMBER] << endl;

            // Clear
            this->finished_jobs[selected_barber_number - 1][I_JOB_FINISHED_AT] = 0;
            this->finished_jobs[selected_barber_number - 1][I_CUSTOMER_NUMBER] = 0;
        }
    }
};

// Function declarations
int readBarberCount(fstream &input_stream);
bool readNextRow(fstream &input_stream, RowData *row);

int main()
{
    fstream input_stream(INPUT_FILE_NAME, ios::in);
    fstream output_stream(OUTPUT_FILE_NAME, ios::out);

    RowData row;
    BarberService barber_service(readBarberCount(input_stream));
    CustomerService customer_service;
    BarberJobService barber_job_service(&barber_service, &customer_service);

    int current_timestamp = 0;

    while (readNextRow(input_stream, &row)) {
        current_timestamp = row.timestamp;

        barber_job_service.processStartedJobs(current_timestamp, output_stream);

        customer_service.addToQueue(row.customer_number, row.customer_job_duration);

        while (customer_service.isNextInQueueSet()) {
            int customer_number = customer_service.getNextInQueue();
            int customer_job_duration = customer_service.getJobDurationForNextInQueue();

            if (!barber_job_service.assignJob(current_timestamp, customer_number, customer_job_duration)) {
                break;
            }

            customer_service.advanceQueue();
        }
    }

    // When there are no more new customers, process the queue or already assigned jobs by advancing the time one by one
    while ((customer_service.isNextInQueueSet() || !customer_service.areAllCustomersProcessed()) && MAX_TIMESTAMP >= current_timestamp) {
        barber_job_service.processStartedJobs(current_timestamp, output_stream);

        int customer_number = customer_service.getNextInQueue();
        int customer_job_duration = customer_service.getJobDurationForNextInQueue();

        if (customer_number && barber_job_service.assignJob(current_timestamp, customer_number, customer_job_duration)) {
            customer_service.advanceQueue();
        }

        current_timestamp++;
    }

    return 0;
}

int readBarberCount(fstream &input_stream)
{
    int barber_count = 0;
    input_stream >> barber_count;

    if (barber_count < MIN_BARBER_COUNT || barber_count > MAX_BARBER_COUNT) {
        throw "Invalid barber count provided";
    }

    return barber_count;
}

// Reads next row from input stream and returns true if row contains data and false if it contained the end symbol.
bool readNextRow(fstream &input_stream, RowData* row)
{
    input_stream >> row->timestamp;

    if (row->timestamp == END_SYMBOL) {
        return false;
    }

    input_stream >> row->customer_number;
    input_stream >> row->customer_job_duration;

    return true;
}