/*
 * Jānis Kristaps Lūsis
 * JL17030
 * 02.05.2020.
*/

#include <fstream>

using namespace std;

// Constant declarations

const char* INPUT_FILE_NAME = "aliens.in";
const char* OUTPUT_FILE_NAME = "aliens.out";

const char COMMAND_ADD_LEFT_CHILD = 'L';
const char COMMAND_ADD_RIGHT_CHILD = 'R';              
const char COMMAND_PRINT_FAVOURITE_ALIENS = '?';
const char COMMAND_END = 'F';

const int MIN_ALIEN_NUMBER = 1;
const int MAX_ALIEN_NUMBER = 10000;

const int MAX_ALIENS_IN_FAMILY = 10000;

const char* ERROR_NOT_FOUND_IN_FAMILY_TREE = "error0";
const char* ERROR_PARENT_CHILD_EQUAL = "error1";
const char* ERROR_PARENT_DOESNT_EXIST = "error2";
const char* ERROR_CHILD_ALREADY_EXISTS = "error3";
const char* ERROR_PARENT_LEFT_CHILD_ALREADY_EXISTS = "error4";
const char* ERROR_PARENT_RIGHT_CHILD_ALREADY_EXISTS = "error5";

// Function declarations

int read_ancestor_number(fstream &input_stream);
char read_command(fstream &input_stream);

// Struct  declarations

struct Alien
{
    int number;
    
    Alien* parent;
    Alien* left_child = NULL;
    Alien* right_child = NULL;

    Alien(int number, Alien* parent = NULL)
    {
        this->number = number;
        this->parent = parent;
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

    bool hasParent()
    {
        return (bool)this->parent;
    }

    bool isAncestor()
    {
        return !this->hasParent();
    }

    bool isLeftChild()
    {
        return this->hasParent() && this == this->parent->left_child;
    }

    bool isRightChild()
    {
        return this->hasParent() && this == this->parent->right_child;
    }

    bool hasNoChildren()
    {
        return !this->hasLeftChild() && !this->hasRightChild();
    }

    bool hasLeftChild()
    {
        return (bool)this->left_child;
    }

    bool hasRightChild()
    {
        return (bool)this->right_child;
    }

    bool hasBothChildren()
    {
        return this->hasLeftChild() && this->hasRightChild();
    }
};

class AlienValidationService
{
    Alien* ancestor;
    bool* added_aliens;

    public:
    AlienValidationService(Alien* ancestor, bool* added_aliens)
    {
        this->ancestor = ancestor;
        this->added_aliens = added_aliens;
    }

    // Should only be used in '?' command
    void failIfAlienIsNotInFamily(int alien_number)
    {
        if (!this->checkIsAlienInFamily(alien_number)) {
            throw ERROR_NOT_FOUND_IN_FAMILY_TREE;
        }
    }

    void failIfParentChildEqual(int parent_number, int child_number)
    {
        if (parent_number == child_number) {
            throw ERROR_PARENT_CHILD_EQUAL;
        }
    }

    void failIfParentDoesntExist(int parent_number)
    {
        if (!this->checkIsAlienInFamily(parent_number)) {
            throw ERROR_PARENT_DOESNT_EXIST;
        }
    }

    void failIfChildAlreadyExists(int child_number)
    {
        if (this->checkIsAlienInFamily(child_number)) {
            throw ERROR_CHILD_ALREADY_EXISTS;
        }
    }

    void failIfParentLeftChildAlreadyExists(Alien* parent) 
    {
        if (!parent) {
            throw "This is not supposed to happen (Parent is nullptr)";
        }

        if (parent->left_child) {
            throw ERROR_PARENT_LEFT_CHILD_ALREADY_EXISTS;
        }
    }

    void failIfParentRightChildAlreadyExists(Alien* parent) 
    {
        if (!parent) {
            throw "This is not supposed to happen (Parent is nullptr)";
        }

        if (parent->right_child) {
            throw ERROR_PARENT_RIGHT_CHILD_ALREADY_EXISTS;
        }
    }

    private:
    bool checkIsAlienInFamily(int alien_number)
    {
        return this->added_aliens[alien_number - 1];
    }
};

class AlienService
{
    // "Galvenais dzīvais sencis"
    Alien* ancestor;

    // Binary mapping for checking whether alien N has already been added to the ancestor tree
    bool added_aliens[MAX_ALIENS_IN_FAMILY] = {};

    // Validation service that will validate actions
    AlienValidationService* validation_service;

    public:
    AlienService(int ancestor_number)
    {
        if (ancestor_number > MAX_ALIEN_NUMBER || ancestor_number < MIN_ALIEN_NUMBER) {
            throw "Invalid ancestor number";
        }

        this->ancestor = new Alien(ancestor_number);
        this->added_aliens[ancestor_number - 1] = true;
        this->validation_service = new AlienValidationService(this->ancestor, this->added_aliens);
    }

    ~AlienService()
    {
        delete this->ancestor; // This will cascade all children too
        delete this->validation_service;
    }

    // Receives of one defined commands, returns true if it expects more commands and false if doesn't
    bool processCommand(char command, fstream &input_stream, fstream &output_stream)
    {
        if (command == COMMAND_END) {
            return false;
        }

        try {
            if (command == COMMAND_ADD_LEFT_CHILD) {
                return this->addLeftChild(input_stream, output_stream);
            }

            if (command == COMMAND_ADD_RIGHT_CHILD) {
                return this->addRightChild(input_stream, output_stream);
            }

            if (command == COMMAND_PRINT_FAVOURITE_ALIENS) {
                return this->printFavouriteAliens(input_stream, output_stream);
            }
        } catch (const char* validation_exception) { // Catch and process validation error
            output_stream << validation_exception << endl;

            return true;
        }

        throw "Unknown command";
    }

    private:
    bool addLeftChild(fstream &input_stream, fstream &output_stream)
    {
        int parent_number;
        int child_number;

        this->readParentChildNumbersAndValidateOrFail(input_stream, parent_number, child_number);

        Alien* parent = this->resolveAlien(parent_number, this->ancestor);
        this->validation_service->failIfParentLeftChildAlreadyExists(parent);

        parent->left_child = new Alien(child_number, parent);
        this->added_aliens[child_number - 1] = true;

        return true;
    }

    bool addRightChild(fstream &input_stream, fstream &output_stream)
    {
        int parent_number;
        int child_number;
        
        this->readParentChildNumbersAndValidateOrFail(input_stream, parent_number, child_number);

        Alien* parent = this->resolveAlien(parent_number, this->ancestor);
        this->validation_service->failIfParentRightChildAlreadyExists(parent);
        
        parent->right_child = new Alien(child_number, parent);
        this->added_aliens[child_number - 1] = true;

        return true;
    }

    void readParentChildNumbersAndValidateOrFail(fstream &input_stream, int &parent_number, int &child_number)
    {
        input_stream >> parent_number;
        input_stream >> child_number;

        this->validation_service->failIfParentChildEqual(parent_number, child_number);
        this->validation_service->failIfParentDoesntExist(parent_number);
        this->validation_service->failIfChildAlreadyExists(child_number);
    }

    Alien* resolveAlien(int alien_number, Alien* current_alien)
    {
        if (!current_alien) {
            return NULL;
        }

        if (current_alien->number == alien_number) {
            return current_alien;
        }
        
        Alien* resolved_alien = resolveAlien(alien_number, current_alien->left_child);
        if (resolved_alien) {
            return resolved_alien;
        }

        resolved_alien = resolveAlien(alien_number, current_alien->right_child);
        if (resolved_alien) {
            return resolved_alien;
        }

        return NULL;
    }

    bool printFavouriteAliens(fstream &input_stream, fstream &output_stream)
    {
        int alien_number;

        input_stream >> alien_number;

        this->validation_service->failIfAlienIsNotInFamily(alien_number);   

        int previous_number = 0;
        bool previous_found = false;
        bool next_found = false;

        walkAliensInOrder(this->ancestor, alien_number, output_stream, previous_number, previous_found, next_found);

        // In case next wasn't found, 0 wasn't outputted so resolve that here
        if (!next_found) {
            output_stream << 0 << endl;
        }

        return true;
    }

    // Walks aliens with inorder traversal and outputs previous, next aliens for the searched alien
    void walkAliensInOrder(Alien* alien, int &searched_alien_number, fstream &output_stream, int &previous_number, bool &previous_found, bool &next_found)
    {
        // Nothing to do here anymore
        if (previous_found && next_found) {
            return;
        }

        if (alien == NULL) {
            return;
        }

        walkAliensInOrder(alien->left_child, searched_alien_number, output_stream, previous_number, previous_found, next_found);

        if (alien->number != searched_alien_number) {
            previous_number = alien->number;

            if (previous_found && !next_found) { // If previous was found and current node is not the searched alien, output current node as the next node
                output_stream << alien->number << endl;
                next_found = true;

                return; // Both previous and next found, can exit
            }
        } else { // If visited node is the searched alien, output previous number and mark it as found
            output_stream << previous_number << " ";
            previous_found = true;
        }

        walkAliensInOrder(alien->right_child, searched_alien_number, output_stream, previous_number, previous_found, next_found);
    }
};

int main()
{
    fstream input_stream(INPUT_FILE_NAME, ios::in);
    fstream output_stream(OUTPUT_FILE_NAME, ios::out);
    
    AlienService alien_service(read_ancestor_number(input_stream));

    char command = read_command(input_stream);
    while (alien_service.processCommand(command, input_stream, output_stream)) {
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