#include <fstream>

using namespace std;

const int rt_size = 2;
const int id_size = 12;
const int cd_name_size = 51;

// Apstrādes palīgfunkcijas
int charSize(const char *text);
void charCopy(char *dest, char *src);
bool charsEqual(char *first, char *second);

struct request
{
    int date; // Skaitlis no 1 .. 1'000'000'000
    char request_type[rt_size]; // A vai B
    char p_id[id_size]; // Personas kods
    char cd_name[cd_name_size]; // Diska nosaukums + beigu simbols
    request *next; // Norāde uz nākamo elementu.

    request(int i_d, char *i_rt, char *i_pid, char *i_cd) // Konstruktors
    {
        date = i_d;
        request_type[0] = i_rt[0];
        request_type[1] = 0; // Beigu simbols
        charCopy(p_id, i_pid);
        charCopy(cd_name, i_cd);
        next = NULL;
    }

};


// Trīs galvenās funkcijas
void processList(request *&first); // Funkcija, kas nolasa, apstrādā, pārbauda datus un izveido otru sarakstu.
void deleteList(request *&first);

// Galveno funkciju palīgfunkcijas
void addRequest(request *&first, request *&last, int date, char request_type[rt_size], char p_id[id_size], char cd_name[cd_name_size]);
bool lookForPair(request *&first, int date, char request_type[rt_size], char p_id[id_size], char cd_name[cd_name_size], char *return_id);

int main()
{
    request *first = NULL; // Pieprasījumu norāde uz pirmo elementu.

    processList(first);

    deleteList(first); // Izdzēš visu pieprasījumu sarakstu no atmiņas (visus neizpildītos pieprasījumus)

    return 0;
}

// Palīgfunkcijas teksta apstrādei.

int charSize(const char *text) // Atgriež char garumu, neieskaitot beigu simbolu.
{
    int size = 0;
    while(text[size] != '\0')
        size++;
    return size;
}

void charCopy(char *dest, char *src)
{
    int pos;
    for(pos = 0; src[pos] != '\0'; pos++)
        dest[pos] = src[pos];

    dest[pos] = '\0';
}

bool charsEqual(char *first, char *second)
{
    if(charSize(first) != charSize(second)) return false;

    for(int pos = 0; first[pos] != '\0'; pos++)
        if(first[pos] != second[pos]) return false;

    return true;
}

void processList(request *&first)
{
    // Mainīgie ievadam
    int date;
    char request_type[rt_size];
    char p_id[id_size];
    char cd_name[cd_name_size];

    request *last_r;

    // Mainīgie apstrādei
    char A_id[id_size];
    char B_id[id_size];
    char return_id[id_size];

    bool is_output = false;

    // Faila atvēršana
    ifstream input;
    input.open("cd.in", ios::in);
    if(!input) return;

    ofstream output;
    output.open("cd.out", ios::out);

    // Funkcijas darbības sākums
    while(input >> date) // Kamēr ir ko ielasīt.
    {
        input >> request_type >> p_id >> cd_name; // .. tiek ielasīti visi mainīgie no rindas.

        if(lookForPair(first, date, request_type, p_id, cd_name, return_id) == true)
        {
            if(request_type[0] == 'A')
            {
                charCopy(A_id, p_id);
                charCopy(B_id, return_id);
            }

            else
            {
                charCopy(A_id, return_id);
                charCopy(B_id, p_id);
            }
            output << date << " " << A_id << " " << B_id << endl;
            is_output = true;
        }

        else addRequest(first, last_r, date, request_type, p_id, cd_name); // Citādāk tas tiek pievienots pieprasījumu sarakstam.

    }

    if(is_output == false) output << 0;

    input.close();
    output.close();
}


// ---------- Pieprasījumu saraksta izdzēšana ----------
void deleteList(request *&first)
{
    while (first != NULL)
    {
        request *next = first->next;
        delete first;
        first = next;
    }
}

bool lookForPair(request *&first, int date, char request_type[rt_size], char p_id[id_size], char cd_name[cd_name_size], char *return_id)
{
    bool pair_found = false;

    request *other_previous = NULL;
    request *other;

    // Pārbaudam, vai sarakstā eksistē otrs elements padotajam pārim (piem A ABBA un B ABBA)
    for(request *current = first; current != NULL; current = current->next)
    {
        // Ja A != B, A_id != p_id, ABBA == ABBA, kaut kāds pāris, kam ir kopīgas intereses, ir atrasts.
        if(current->request_type[0] != request_type[0] && charsEqual(current->p_id, p_id) == false && charsEqual(current->cd_name, cd_name) == true)
        {
            other = current; // Saglabājam pāra otras puses adresi.
            pair_found = true;
            break;
        }

        other_previous = current; // Ja esam apskatījuši pirmo elementu, tad elements pirms other būs viens pirms tā.
    }

    // Ja jauns pāris netika atrasts, sakam, ka tas, nu, netika atrasts un lecam ārā
    if(pair_found == false) return false;

    // Ja esam atraduši jaunu pāri,

    charCopy(return_id, other->p_id); // atgriežam otras personas kodu.

    // un izdzēšam otru elementu.

    if(other == first) // Ja elements bija pirmais
    {
        first = first->next;
        delete other;
    }

    else // ja kaut kur citur
    {
        other_previous->next = other->next; // Lecam pāri other
        delete other; // Izdzēšam to.
    }

    return true; // Sakam, ka jauns pāris atrasts.

}

// --------- Pievienošanas funkcijas ---------

// Pievieno pieprasījumu sarakstam
void addRequest(request *&first, request *&last, int date, char request_type[rt_size], char p_id[id_size], char cd_name[cd_name_size])
{
    if(first == NULL) first = last = new request(date, request_type, p_id, cd_name);
    else
    {
        last->next = new request(date, request_type, p_id, cd_name);
        last = last->next;
    }
}