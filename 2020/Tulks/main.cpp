/*
 * Jānis Kristaps Lūsis
 * JL17030
 * 02.05.2020
*/

#include <iostream>
#include <fstream>

using namespace std;

// Constant declarations

const int MAX_WORD_LENGTH = 20;

const int MIN_NUMBER = 48;
const int MAX_NUMBER = 57;
const int MIN_LARGE_CHAR = 65;
const int MAX_LARGE_CHAR = 90;
const int MIN_SMALL_CHAR = 97;
const int MAX_SMALL_CHAR = 122;

const int NUMBER_COUNT = MAX_NUMBER - MIN_NUMBER + 1;
const int LARGE_CHAR_COUNT = MAX_LARGE_CHAR - MIN_LARGE_CHAR + 1;
const int SMALL_CHAR_COUNT = MAX_SMALL_CHAR - MIN_SMALL_CHAR + 1;

const int CHAR_COUNT = NUMBER_COUNT + LARGE_CHAR_COUNT + SMALL_CHAR_COUNT;

const char UNKNOWN_WORD_PREFIX = '?';

// Function declarations
void copy_string(const char* from, char* to, int to_length);
bool are_strings_equal(char* one, const char* two);

// Structures

struct Word
{
    // On way linked list
    Word* next = NULL;

    char word_a[MAX_WORD_LENGTH] = {};
    char word_b[MAX_WORD_LENGTH] = {};

    Word(char* word_a, char* word_b)
    {
        copy_string(word_a, this->word_a, MAX_WORD_LENGTH);
        copy_string(word_b, this->word_b, MAX_WORD_LENGTH);
    }

    ~Word()
    {
        cout << "Deleting self: " << this->word_a << endl;

        // Cascade all next items in list
        while (this->next != NULL) {
            Word* next_word = this->next;
            this->next = this->next->next;

            cout << "Deleting child: " << next_word->word_a << " " << next_word->word_b << endl;

            delete next_word;
        }
    }
};

// Classes

class DictionaryRepository
{
    // Dictionary contains pointers that point to first structure that begins with char N (e.g. char A)
    Word* dictionary[CHAR_COUNT] = {NULL};

    public:
    ~DictionaryRepository() 
    {
        // Delete all the created words (Kas ar NEW izveidots, tam ar DELETE būs mirt)
        for (int i = 0; i < CHAR_COUNT; i++) {
            if (dictionary[i] != NULL) {
                delete dictionary[i];
            }
        }
    }

    void addWord(char first_char, Word* word)
    {
        int dictionary_index = this->resolveDictionaryIndex(first_char);
        Word* last_word = this->resolveLastWord(dictionary_index);

        cout << "Trying to add word " << word->word_a << endl;

        if (last_word) {
            last_word->next = word;

            cout << "Word added to previous word in list - " << last_word->word_a << endl;

            return;
        }

        this->dictionary[dictionary_index] = word;

        cout << "Word added as the first word in dictionary index " << dictionary_index << endl;
    }

    // Searches the dictionary for the word; if it is found, word structure is returned, otherwise NULL is returned.
    Word* getWord(const char* searchable_word)
    {
        int dictionary_index = this->resolveDictionaryIndex(searchable_word[0]);

        cout << "Searching for " << searchable_word << endl;

        for (Word* word = this->dictionary[dictionary_index]; word != NULL; word = word->next) {
            cout << "Comparing " << word->word_a << " to " << searchable_word << endl;
            if (are_strings_equal(word->word_a, searchable_word)) {
                cout << "Success - found" << endl;

                return word;
            }
        }

        cout << "Fail - Not found" << endl;

        return NULL;
    }

    private:
    // Receives the first char of a word
    int resolveDictionaryIndex(char first_char)
    {
        if (first_char >= MIN_NUMBER && first_char <= MAX_NUMBER) {
            return (int)first_char - MIN_NUMBER;
        }

        if (first_char >= MIN_LARGE_CHAR && first_char <= MAX_LARGE_CHAR) {
            return (int)first_char - MIN_LARGE_CHAR + NUMBER_COUNT;
        }

        if (first_char >= MIN_SMALL_CHAR && first_char <= MAX_SMALL_CHAR) {
            return (int)first_char - MIN_SMALL_CHAR + NUMBER_COUNT + LARGE_CHAR_COUNT;
        }

        cout << "Invalid character: " << first_char << endl;

        throw "Invalid character";
    }

    Word* resolveLastWord(int dictionary_index)
    {
        if (this->dictionary[dictionary_index] == NULL) {
            return NULL;
        }

        Word* word = this->dictionary[dictionary_index];

        while (word->next != NULL) {
            word = word->next;
        }

        return word;
    }
};

class DictionaryService
{

};

int main()
{
    // Ielasam visus vārdus kaut kādā bibliotēkā; jābūt bi-directional, lai var atrast gan no A uz B gan no B uz A (divas dažādas biblitotēkas, kas satur pointerus un rāda viens uz otru?)
    // Ķip var būt a-z A-Z 0-9; domāju, ka jātaisa saistītais saraksts vai kkāds hash mappings, lai var pēc pirmā, otrā, trešā simbola vtml fiksi atrast

    // Noskaidrojam direciton, vienreizējs pasākums laikam

    // 0. Lasam rindu pa vārdam
    // 1. ja tulkojums ir, tad uzreiz izvadam vārdu
    // 2. ja tulkojums nav, izvadam ?oriģinālais
    // 3. ja rindas beigas, sākam jaunu rindu (atkārtojam 0.)

    char word_a[MAX_WORD_LENGTH] = "Abols";
    char word_b[MAX_WORD_LENGTH] = "Apple";

    DictionaryRepository dictionary_repository;

    Word* word = new Word(word_a, word_b);
    dictionary_repository.addWord(word->word_a[0], word);

    strcpy(word_a, "Abols2");
    strcpy(word_b, "Apple2");

    word = new Word(word_a, word_b);
    dictionary_repository.addWord(word->word_a[0], word);

    strcpy(word_a, "Banans");
    strcpy(word_b, "Banana");

    word = new Word(word_a, word_b);
    dictionary_repository.addWord(word->word_a[0], word);

    strcpy(word_a, "Zars");
    strcpy(word_b, "Zbranch");

    word = new Word(word_a, word_b);
    dictionary_repository.addWord(word->word_a[0], word);

    strcpy(word_a, "9kaut kas");
    strcpy(word_b, "9something");

    word = new Word(word_a, word_b);
    dictionary_repository.addWord(word->word_a[0], word);

    strcpy(word_a, "abols");
    strcpy(word_b, "apple");

    word = new Word(word_a, word_b);
    dictionary_repository.addWord(word->word_a[0], word);


    word = dictionary_repository.getWord("Abols");
    word = dictionary_repository.getWord("Abols2");
    word = dictionary_repository.getWord("Banans");
    word = dictionary_repository.getWord("Zars");
    word = dictionary_repository.getWord("9kaut kas");
    word = dictionary_repository.getWord("abols");
    word = dictionary_repository.getWord("neeksiste");


    return 0;
}

// Functions

void copy_string(const char* from, char* to, int to_length)
{
    for (int i = 0; from[i] != 0; i++) {
        if (i > to_length) {
            return;
        }

        to[i] = from[i];
    }
}

bool are_strings_equal(char* one, const char* two)
{
    for (int i = 0; one[i] != 0; i++) {
        if (one[i] != two[i]) {
            return false;
        }
    }

    return true;
}