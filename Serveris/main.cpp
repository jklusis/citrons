#include <fstream>

using namespace std;

int rekursija(int virsotne, int step_count, int dati[][2], bool visited[], const int &rindu_skaits)
{
    bool plus_step = false;
    visited[virsotne-1] = true;
    for(int i = 0; i < rindu_skaits; i++)
    {
        if(dati[i][0] == virsotne && visited[dati[i][1]-1] == false)
        {
            if(plus_step == false) // Pārbaude, vai no virsotnes jau ir iziets.
            {
                step_count++;
                plus_step = true;
            }
            step_count = rekursija(dati[i][1], step_count, dati, visited, rindu_skaits);
        }

        else if(dati[i][1] == virsotne && visited[dati[i][0]-1] == false)
        {
            if(plus_step == false) // Pārbaude, vai no virsotnes jau ir iziets.
            {
                step_count++;
                plus_step = true;
            }
            step_count = rekursija(dati[i][0], step_count, dati, visited, rindu_skaits);
        }
    }
    return step_count;
}

void emptyArray(int visited[], const int &datoru_skaits)
{
    for(int i = 0; i < datoru_skaits; i++)
        visited[i] = 0;
}

void emptyArray(bool visited[], const int &datoru_skaits)
{
    for(int i = 0; i < datoru_skaits; i++)
        visited[i] = 0;
}



int main()
{  
    // Mainīgo deklarēšana
    int datoru_skaits;
    int dators;
    int rindu_skaits = 0;

    int temp;
    int min = 2147483647; // min:)


    // Datu ielasīšana
    fstream input("serveris.in", ios::in);

    // Tiek noteikts rindu skaits
    input.seekg (0, input.end);
    rindu_skaits = input.tellg();
    rindu_skaits = (rindu_skaits-5)/4; // Tiek ignorēta pirmā un pēdējā rinda (1 + 4 = 5) un dalīts ar 4, lai iegūtu katru rindu

    input.seekg (0, input.beg); // Lai varētu veiksmīgi nolasīt visas rindas, atgriežamies faila sākumā.

    input >> datoru_skaits; // Iegūstam datoru skaitu

    int dati[rindu_skaits][2]; // Tiek izveidots statisks masīvs pēc noteiktā rindu skaita
    int min_steps[datoru_skaits]; // Tiek izveidots masīvs soļu uzglabāšanai.
    bool visited[datoru_skaits]; // Tiek izveidots boolean masīvs, lai noteiktu, kuri ir jau apmeklēti.
    
    emptyArray(visited, datoru_skaits); // Aizpilda bool masīvu ar nullēm.
    emptyArray(min_steps, datoru_skaits);


    for(int i = 0; i < rindu_skaits; i++)
    {
        input >> dators;
        dati[i][0] = dators;
        input >> dators;
        dati[i][1] = dators;
    }

    input.close(); // Aizveram ciet ievadfailu.
    
    // REKURSIJA - no aizietās virsotnes ej tālāk uz nākamo virsotni, vislaik +1, beigās return soļu skaitu
    for(int virsotne = 1; virsotne <= datoru_skaits; virsotne++) // Tiek apstaigātas visas virsotnes
    {
        for(int i = 0; i < rindu_skaits; i++)
        {
            if(dati[i][0] == virsotne)
            {
                emptyArray(visited, datoru_skaits); // Pēc katras iterācijas ir jānotīra apmeklēto virsotņu masīvs.
                visited[virsotne-1] = true; // Pasakam, ka sākuma virsotne ir apmeklēta.
                temp = rekursija(dati[i][1], 1, dati, visited, rindu_skaits);
                if(temp > min_steps[virsotne-1]) min_steps[virsotne-1] = temp;
            }

            else if(dati[i][1] == virsotne)
            {
                emptyArray(visited, datoru_skaits); // Pēc katras iterācijas ir jānotīra apmeklēto virsotņu masīvs.
                visited[virsotne-1] = true; // Pasakam, ka sākuma virsotne ir apmeklēta.
                temp = rekursija(dati[i][0], 1, dati, visited, rindu_skaits);
                if(temp > min_steps[virsotne-1]) min_steps[virsotne-1] = temp;
            }

        }

        if(min > min_steps[virsotne-1]) min = min_steps[virsotne-1];

    }

    // Rezultātu izvads
    fstream output("serveris.out", ios::out);

    output << min << endl;

    for(int i = 0; i < datoru_skaits; i++)
    {
        if(min_steps[i] == min) output << i+1 << " ";
    }

    output.close();
}


