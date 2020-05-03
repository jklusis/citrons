/*
 * Jānis Kristaps Lūsis
 * JL17030
 * 02.05.2020.
*/

#include <fstream>

using namespace std;

// Constant declarations

const char* INPUT_FILE_NAME = "tulks.in";
const char* OUTPUT_FILE_NAME = "tulks.out";

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

const int MAX_WORD_LENGTH = 20 + 1;
const char UNKNOWN_WORD_PREFIX = '?';

const char MODE_A_TO_B[4] = "-->";
const char MODE_B_TO_A[4] = "<--";

// Function declarations

void copy_string(const char* from, char* to, int to_length);
bool are_strings_equal(char* one, const char* two);

// Structures

struct Word
{
    // One way linked list
    Word* next = NULL;

    char original[MAX_WORD_LENGTH] = {};
    Word* translation = NULL;

    Word(const char* original)
    {
        copy_string(original, this->original, MAX_WORD_LENGTH);
    }

    ~Word()
    {
        // Cascade
        if (this->next != NULL) {
            delete this->next;
        }
    }
};

// Classes

class DictionaryRepository
{
    // Dictionary contains pointers that point to first structure that contains word that begins with char N (e.g. char A)
    // Ideally a binary tree could be implemented instead of a linked list to improve performance
    Word* dictionary[CHAR_COUNT] = {NULL};

    public:
    ~DictionaryRepository() 
    {
        // Delete all the created words (Kas ar NEW izveidots, tam ar DELETE būs mirt)
        for (int i = 0; i < CHAR_COUNT; i++) {
            if (dictionary[i] != NULL) {
                delete dictionary[i]; // They cascade themselves
            }
        }
    }

    void addWord(Word* word)
    {
        int dictionary_index = this->resolveDictionaryIndex(word->original[0]);
        Word* last_word = this->resolveLastWord(dictionary_index);

        if (last_word) {
            last_word->next = word;

            return;
        }

        this->dictionary[dictionary_index] = word;
    }

    // Searches the dictionary for the word; if it is found, word structure is returned, otherwise NULL is returned.
    Word* getWord(const char* searchable_word)
    {
        int dictionary_index = this->resolveDictionaryIndex(searchable_word[0]);

        for (Word* word = this->dictionary[dictionary_index]; word != NULL; word = word->next) {
            if (are_strings_equal(word->original, searchable_word)) {
                return word;
            }
        }

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

class TranslationService
{
    DictionaryRepository repository_a;
    DictionaryRepository repository_b;

    public:
    void addWord(const char* word_in_a, const char* word_in_b)
    {
        Word* word_a = new Word(word_in_a);
        Word* word_b = new Word(word_in_b);

        word_a->translation = word_b;
        word_b->translation = word_a;

        this->repository_a.addWord(word_a);
        this->repository_b.addWord(word_b);
    }

    void translateFromAToB(const char* word_in_a, char* word_out)
    {
        this->resolveTranslation(word_in_a, this->getWordInA(word_in_a), word_out);
    }

    void translateFromBToA(const char* word_in_b, char* word_out)
    {
        this->resolveTranslation(word_in_b, this->getWordInB(word_in_b), word_out);
    }

    private:
    Word* getWordInA(const char* word)
    {
        return this->repository_a.getWord(word);
    }

    Word* getWordInB(const char* word)
    {
        return this->repository_b.getWord(word);
    }

    void resolveTranslation(const char* word_to_translate, Word* word, char* word_out)
    {
        if (word) {
            copy_string(word->translation->original, word_out, MAX_WORD_LENGTH + 1);

            return;
        }

        word_out[0] = UNKNOWN_WORD_PREFIX;
        for (int i = 0; word_to_translate[i] != 0; i++) {
            if (i > MAX_WORD_LENGTH) {
                break;
            }

            word_out[i + 1] = word_to_translate[i];
        }
    }
};

int main()
{
    fstream input_stream(INPUT_FILE_NAME, ios::in);
    fstream output_stream(OUTPUT_FILE_NAME, ios::out);

    TranslationService translation_service;

    bool is_mode_a_to_b = false;

    // 1. Read the dictionary into the memory and resolve type
    while (true) {
        char word_in_a[MAX_WORD_LENGTH] = {};
        char word_in_b[MAX_WORD_LENGTH] = {};

        input_stream >> word_in_a;

        // Resolve translation type if '-' is encountered
        if (are_strings_equal(word_in_a, MODE_A_TO_B)) {
            is_mode_a_to_b = true;

            break;
        }

        if (are_strings_equal(word_in_a, MODE_B_TO_A)) {
            is_mode_a_to_b = false;

            break;
        }

        input_stream >> word_in_b;

        translation_service.addWord(word_in_a, word_in_b);
    }

    // 2. While able to read words, try to translate them and output into file
    while (true) {
        char word_to_translate[MAX_WORD_LENGTH] = {};
        
        input_stream >> word_to_translate;

        if (!word_to_translate[0]) {
            break;
        }

        char translated_string[MAX_WORD_LENGTH + 1] = {};
        
        if (is_mode_a_to_b) {
            translation_service.translateFromAToB(word_to_translate, translated_string);
        } else {
            translation_service.translateFromBToA(word_to_translate, translated_string);
        }

        output_stream << translated_string << " ";
    }

    input_stream.close();
    output_stream.close();

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

        if (one[i + 1] == 0 && two[i + 1] != 0) {
            return false;
        }
    }

    return true;
}