/*
 * Jānis Kristaps Lūsis
 * JL17030
 * 02.05.2020
*/

#include <iostream>
#include <fstream>

using namespace std;

// Constant declarations

const char* INPUT_FILE_NAME = "aliens.in";
const char* OUTPUT_FILE_NAME = "aliens.out";

const char COMMAND_ADD_LEFT_CHILD = 'L';
const char COMMAND_ADD_RIGHT_CHILD = 'R';              
const char COMMAND_PRINT_FAVOURITE_ALIENS = '?';
const char COMMAND_END = 'F';

const char* ERROR_NOT_FOUND = "error0";
const char* ERROR_PARENT_CHILD_EQUAL = "error1";
const char* ERROR_PARENT_DOESNT_EXIST = "error2";
const char* ERROR_CHILD_ALREADY_EXISTS = "error3";
const char* ERROR_PARENT_LEFT_CHILD_ALREADY_EXISTS = "error4";
const char* ERROR_PARENT_RIGHT_CHILD_ALREADY_EXISTS = "error5";

const int MIN_ALIEN_NUMBER = 1;
const int MAX_ALIEN_NUMBER = 10000;

const int MAX_ALIENS_IN_FAMILY = 10000;

// Function declarations

int read_ancestor_number(fstream &input_stream);
char read_command(fstream &input_stream);

// Struct  declarations

struct Alien
{
    int number;
    
    Alien* left_child = NULL;
    Alien* right_child = NULL;

    Alien(int number)
    {
        this->number = number;
    }

    ~Alien()
    {
        // Cascade
        if (this->left_child) {
            delete this->left_child;
        }

        if (this->right_child) {
            delete this->right_child;
        }
    }
};

class AlienService
{
    // "Galvenais dzīvais sencis"
    Alien* ancestor;

    // Binary mapping for checking whether alien N has already been added to the ancestor tree
    int added_aliens[MAX_ALIENS_IN_FAMILY] = {};

    public:
    AlienService(int ancestor_number)
    {
        if (ancestor_number > MAX_ALIEN_NUMBER || ancestor_number < MIN_ALIEN_NUMBER) {
            throw "Invalid ancestor number";
        }

        this->ancestor = new Alien(ancestor_number);
    }

    ~AlienService()
    {
        delete this->ancestor;
    }

    // Receives of one defined commands, returns true if it expects more commands and false if doesn't
    bool processCommand(char command, fstream &input_stream)
    {
        if (command == COMMAND_END) {
            return false;
        }

        cout << "Received command: " << command << endl;

        return true;
    }

    private:
    void addLeftChild()
    {

    }

    void addRightChild()
    {

    }
};

int main()
{
    fstream input_stream(INPUT_FILE_NAME, ios::in);
    fstream output_stream(OUTPUT_FILE_NAME, ios::out);
    
    AlienService alien_service(read_ancestor_number(input_stream));

    char command = read_command(input_stream);
    while (alien_service.processCommand(command, input_stream)) {
        command = read_command(input_stream);
    }

    input_stream.close();
    output_stream.close();

    return 0;
}

// Reads the ancestor number from file and returns it
int read_ancestor_number(fstream &input_stream)
{
    int ancestor_number;

    input_stream >> ancestor_number;

    return ancestor_number;
}

char read_command(fstream &input_stream)
{
    char command;

    input_stream >> command;

    return command;
}