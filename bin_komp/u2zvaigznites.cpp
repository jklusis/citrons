#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

// Definējam charu garumus BEZ(!) beigu simbola.
// Šie ir globālie mainīgie, kurus izmantosim jebkur citur.
const int UZVARDA_GARUMS = 20;
const int VARDA_GARUMS = 15;
const int STUD_APL_NR_GARUMS = 7;

char faila_nosaukums[51];

// --------------------------------------------------------------------------------------------------------------------
// VISPĀRĒJAS PALĪGFUNKCIJAS
// --------------------------------------------------------------------------------------------------------------------

bool areCharsEqual(const char *first, const char *second)
{
    if(strlen(first) != strlen(second)) return false;

    for(int i = 0; first[i] != 0; i++)
    {
        if(first[i] != second[i]) return false;
    }

    return true;
}

void enterToContinue()
{
    string temp;
    cout << "-- Ierakstiet jebko un enter, lai turpinatu..";
    cin >> temp;
}
// --------------------------------------------------------------------------------------------------------------------
// KLASE "STUDENTS" UN TĀS FUNKCIJAS
// --------------------------------------------------------------------------------------------------------------------
class Students
{
    private:
        char uzvards[UZVARDA_GARUMS+1]; // +1 beigu simbolam.
        char vards[VARDA_GARUMS+1]; // Tas pats.
        char stud_apl_nr[STUD_APL_NR_GARUMS+1]; // "Bez komentāriem"
        int semestra_nr;
        int atzimes[10];

    public:
        bool fillObject(); // Funkcija, kas paredzēta augšējo lauku aizpildīšanai.
        void writeObject(const char *faila_nosaukums); // Funkcija, kas ieraksta klases saturu faila beigās.

        void printObject(); // Funkcija, kas izdrukā komponenti jeb info par vienu studentu (esošos, ielasītos laukus (uzvārdu, vārdu utt))
        void printFile(const char *faila_nosaukums); // Funkcija, kas izdrukā visu faila saturu

        bool changeUzvards(const char *faila_nosaukums, const char *san, const char *new_uzvards); // Funkcija, kas saņem studentu apl. nr. un jauno uzvārdu, atgriež true, ja izdevās nomainīt, false, ja nē.

        void findStudent(const char *filename, const char *san);

        void printAverageGradesInSemester(const char *filename, int semestris);

        void printInfoAboutStudentsWithGradeGreaterThan(const char *filename, int semestris, int min_atzime);

        void makeBinaryAboutStudentsWith10AndOthers(const char *filename, const char *desmitnieki, const char *parejie, int semestris);
};

// U211
bool Students::fillObject() // Klasē ir funkcija, ar kuru tiek ievadīti dati. Bool, jo ja tiks atgriezts false, tad beigsies ievads.
{
    cout << "Ievadiet uzvardu (20 simboli) (0 un enter, lai beigtu ievadit)" << endl;
    cin >> uzvards;

    if(uzvards[0] == '0')
        return false; // Liecina par ievada beigšanu

    cout << "Ievadiet vardu (15 simboli)" << endl;
    cin >> vards;

    cout << "Ievadiet stud. apl. nr. (7 simboli)" << endl;
    cin >> stud_apl_nr;

    cout << "Ievadiet semestra nr. (vesels skaitlis)" << endl;
    cin >> semestra_nr;

    cout << "Ievadiet 10 atzimes (veseli skaitli)" << endl;
    for(int i = 0; i < 10; i++)
        cin >> atzimes[i];
    
    return true; // Liecina par ievada turpināšanu
}

void Students::writeObject(const char *faila_nosaukums) // Klasē ir funkcija, ar kuru tiek visa komponente izdrukāta faila beigās.
{
    ofstream output(faila_nosaukums, ios::app | ios::binary); // !!! ios::app - append - pievieno faila beigās.
    output.write((char*)(this), sizeof(Students)); // Te ir mazliet interesanti. Klasē students visas vērtības atmiņā uzglabājas viena pēc otras.
    // Un tā kā pati klase ir pointeris, varam nelikt "&" pirms (this).
    // UN "this" norāda uz pašu klasi. Piemēram, ja mēs rakstītu this->uzvards, mēs piekļūtu uzvārdam.
    output.close();
}

void Students::printObject() // Klasē ir funkcija, ar kuru var izdrukāt esošās komponentes saturu.
{
    cout << "Studenta uzvards: " << uzvards << endl;
    cout << "Studenta vards: " << vards << endl;
    cout << "Studenta apl. nr.: " << stud_apl_nr << endl;
    cout << "Semestra nr.: " << semestra_nr << endl;
    cout << "Atzimes: [ ";
    for(int i = 0; i < 10; i++) // Tiek izdrukāts masīvs
        cout << atzimes[i] << " ";
    cout << "]" << endl;
}

void Students::printFile(const char *faila_nosaukums) // Funkcija, kas nolasa failu, nolasa komponenti un to izdrukā.
{
    ifstream input(faila_nosaukums, ios::in | ios::binary);

    if(!input) // Ja failu neizdevās nolasīt.
    {
        cout << "Failu neizdevas nolasit!" << endl;
        return;
    }

    while( input.read((char*)(this), sizeof(Students)) ) // Kamēr izdodas ielasīt veselu komponenti
    {
        printObject(); // Izdrukājam ielasītās komponentes saturu.
        cout << "---------------" << endl;
    }
}

bool Students::changeUzvards(const char *faila_nosaukums, const char *san, const char *new_uzvards)
{
    int pos; // Mainīgais, ar kuru būs iespējams noteikt pozīciju failā.
    bool rez = false;
    fstream file(faila_nosaukums, ios::in | ios::out | ios::binary);
    
    while( file.read((char*)(this), sizeof(Students)) ) // Kamēr izdodas ielasīt veselu komponenti
    {
        if(areCharsEqual(stud_apl_nr, san) == true) // Ja studentu apliecības numuri sakrīt
        {
            pos = file.tellg(); // Iegūstam pozīciju tieši aiz konkrētās studenta komponentes

            file.seekp(pos-sizeof(Students)); // Paejam vienu komponenti uz atpakaļu (nokļūstam atkal pie studenta komponentes, kuru iepriekš nolasījām)

            strncpy(uzvards, new_uzvards, UZVARDA_GARUMS); // Pārkopējam jauno uzvārdu bez beigu simbola

            uzvards[UZVARDA_GARUMS] = 0; // Ievietojam beigu simboolu uzvārda beigās

            file.write((char*)(this), sizeof(Students)); // Ierakstam modificēto komponenti failā
            
            rez = true; // Jo var būt vairākas komponentes ar šādu stud. apl. nr.
        }
    }

    if(rez == true) return true;
    return false; // Ja izgājām cauri visiem un neatradām stud. apl nr., lecam laukā
}

void Students::printAverageGradesInSemester(const char *filename, int semestris)
{
    bool atleast_one = false;
    int count = 0;
    double vid_atzimes[10] = {0};

    fstream file(faila_nosaukums, ios::in | ios::out | ios::binary);
    
    while( file.read((char*)(this), sizeof(Students)) ) // Kamēr izdodas ielasīt veselu komponenti
    {
        if(semestra_nr == semestris)
        { 
            atleast_one = true;
            count++;
            for(int i = 0; i < 10; i++)
                vid_atzimes[i] += atzimes[i];
        }
    }

    if(atleast_one == true)
    {
        cout << "Videjas atzimes " << semestris << " semestrii starp " << count << " studentiem ir -" << endl;
        cout << "[ ";
        for(int i = 0; i < 10; i++)
            cout << vid_atzimes[i]/count << " ";
        cout << "]" << endl;
    }

    else
        cout << semestris << ". semestrii nav neviena registreta studenta." << endl;

}

void Students::findStudent(const char *filename, const char *san)
{
    int pos; // Mainīgais, ar kuru būs iespējams noteikt pozīciju failā.
    bool rez = false;
    fstream file(faila_nosaukums, ios::in | ios::out | ios::binary);
    
    while( file.read((char*)(this), sizeof(Students)) ) // Kamēr izdodas ielasīt veselu komponenti
    {
        if(areCharsEqual(stud_apl_nr, san) == true) // Ja studentu apliecības numuri sakrīt
        {
            rez = true;
            printObject();
        }
    }

    if(rez == false)
    {
        cout << "Informacija par studentu ar stud. apl. nr. " << san << " netika atrasta." << endl;
    }

}

void Students::printInfoAboutStudentsWithGradeGreaterThan(const char *filename, int semestris, int min_atzime)
{
    bool rez = false;
    double videja_atzime = 0;
    fstream file(faila_nosaukums, ios::in | ios::out | ios::binary);
    
    while( file.read((char*)(this), sizeof(Students)) ) // Kamēr izdodas ielasīt veselu komponenti
    {
        if(semestra_nr == semestris) // Ja studentam ir info par padoto semestri
        {
            // Iegūstam studenta vidējo atzīmi
            for(int i = 0; i < 10; i++)
                videja_atzime += atzimes[i];

            videja_atzime /= 10;

            if(videja_atzime >= min_atzime)
            {
                printObject();
                cout << "Studenta videja atzime: " << videja_atzime << endl;
                rez = true;
            }
        }
    }

    if(rez == false)
    {
        cout << "Informacija par studentiem " << semestris << ". semestri ar atzimem augstakam par " << min_atzime << " netika atrasta." << endl;
    }
}

void Students::makeBinaryAboutStudentsWith10AndOthers(const char *filename, const char *desmitnieki, const char *parejie, int semestris)
{
    // filename ir faila nosaukums, no kura ņem datus
    // desmitnieki un pārējie attiecīgi faili, kuri tiks papildināti

    fstream file(filename, ios::in | ios::binary);

    while( file.read((char*)(this), sizeof(Students)) ) // Kamēr ir iespējams nolasit veselu komponenti
    {
        bool ir_desmit = false;
        if(semestra_nr == semestris) // Ja komponentes semestra numurs sakrīt ar pieprasīto semestri
        { 
            for(int i = 0; i < 10; i++) // Skatamies, vai studentam ir desmitnieks
            {
                if(atzimes[i] == 10) // Ja ir
                {
                    ir_desmit = true;
                    break;
                }
            }

            if(ir_desmit == true) // Ja ir bijis kaut viens desmitnieks
            {
                writeObject(desmitnieki); // Izmantojam klases funkciju komponentes izvadīšanai
            }

            else
            {
                writeObject(parejie); // Tas pats.
            }
        }
    }
}
// --------------------------------------------------------------------------------------------------------------------
// MAIN PALĪGFUNKCIJAS
// --------------------------------------------------------------------------------------------------------------------

// U211, U214
void binaryCreate(Students &s) // Palīgfunkcijām jāpadod mainā izveidotā klase ar "&s"
{
    cout << "-- Faila izveide: ievadiet faila nosaukumu (piem. studenti.bin)" << endl;
    cin >> faila_nosaukums;
    while(s.fillObject() == true) // Kamēr lietotājs saka, ka vēlas turpināt ievadu, viņš ievada klasē datus
        s.writeObject(faila_nosaukums); // Un šie dati tiek ierakstīti failā.
}

// U212
void binaryRead(Students &s)
{
    cout << "-- Faila ielasisana: ievadiet faila nosaukumu (piem. studenti.bin)" << endl;
    cin >> faila_nosaukums;
    s.printFile(faila_nosaukums);
}

// U213
void binaryChange(Students &s)
{
    char san[STUD_APL_NR_GARUMS+1]; // Stud apl. nr. pēc kura meklēt
    char new_uzvards[UZVARDA_GARUMS+1]; // Jaunais uzvārds
    cout << "-- Faila apstrade: ievadiet faila nosaukumu (piem. studenti.bin)" << endl;
    cin >> faila_nosaukums;

    cout << "Ievadiet stud. apl. nr. studentam, kura uzvardu velaties nomainit:" << endl;
    cin >> san;
    cout << "Ievadiet jauno uzvardu" << endl;
    cin >> new_uzvards;
    if(s.changeUzvards(faila_nosaukums, san, new_uzvards) == true)
        cout << "Uzvards studentam ar stud. apl. nr." << san << " tika veiksmigi nomainits uz " << new_uzvards << endl;
    else
        cout << "Uzvardu neizdevas nomainit, jo students ar stud. apl. nr. " << san << " failaa netika atrasts." << endl;

}

void binaryReadAverageGradesInSemester(Students &s)
{
    int semestris;
    cout << "-- Faila ievade: ievadiet faila nosaukumu (piem. studenti.bin)" << endl;
    cin >> faila_nosaukums;
    cout << "Ievadiet semestra nr." << endl;
    cin >> semestris;
    s.printAverageGradesInSemester(faila_nosaukums, semestris);
}

void binaryPrintInfoAboutStudent(Students &s)
{
    char san[STUD_APL_NR_GARUMS+1]; // Stud apl. nr. pēc kura meklēt
    cout << "-- Faila apstrade: ievadiet faila nosaukumu (piem. studenti.bin)" << endl;
    cin >> faila_nosaukums;

    cout << "Ievadiet stud. apl. nr. studentam, kura informaciju velaties atrast:" << endl;
    cin >> san;

    s.findStudent(faila_nosaukums, san);
}

void binaryPrintInfoAboutStudentsWithGradeGreaterThan(Students &s)
{
    int semestris;
    int min_atzime;
    cout << "-- Faila ievade: ievadiet faila nosaukumu (piem. studenti.bin)" << endl;
    cin >> faila_nosaukums;
    cout << "Ievadiet semestra nr." << endl;
    cin >> semestris;
    cout << "Ievadiet minimalo atzimi." << endl;
    cin >> min_atzime;

    s.printInfoAboutStudentsWithGradeGreaterThan(faila_nosaukums, semestris, min_atzime);
}

void binaryMakeAboutStudentsWith10AndOthers(Students &s) 
{
    int semestris;
    char desmitnieki[51];
    char parejie[51];

    cout << "-- Faila ievade: ievadiet faila nosaukumu (piem. studenti.bin)" << endl;
    cin >> faila_nosaukums;
    cout << "Ievadiet semestra nr." << endl;
    cin >> semestris;
    cout << "Ievadiet faila nosaukumu desmitniekiem (piem. desmitnieki.bin)" << endl;
    cin >> desmitnieki;
    cout << "Ievadiet faila nosaukumu parejiem (piem. parejie.bin" << endl;
    cin >> parejie;

    s.makeBinaryAboutStudentsWith10AndOthers(faila_nosaukums, desmitnieki, parejie, semestris);
}

int main()
{
    Students s; // Izveidojam klasi.

    int choice;

    while(true)
    {
        cout << "Izvelne:)" << endl;
        cout << "1. Izveidot vai papildinat binaru failu ar studentiem (U211, U214)" << endl;
        cout << "2. Izdrukat esosu failu ar studentiem (U212)" << endl;
        cout << "3. Izmainit studenta uzvardu pec stud. apl. nr. (U213)" << endl;
        cout << "4. Aprekinat video atzimi katra prieksmeta 1. semestri (U215)" << endl;
        cout << "5. Izdrukat informaciju par studentu ar konkretu studentu apliecibas numuru (U216)" << endl;
        cout << "6. Izdrukat zinas par tiem studentiem, kuriem 1. semestri videja atzime ir lielaka par 5 (U217)" << endl;
        cout << "7. (EKS) Izvadit binara faila studentus, kuriem ir kaut viens desmit un cita binara faia parejos konkreta semestri" << endl;
        cout << "999. Beigt darbu" << endl;

        cin >> choice;

        if(choice == 1) binaryCreate(s);
        else if(choice == 2) binaryRead(s);
        else if(choice == 3) binaryChange(s);
        else if(choice == 4) binaryReadAverageGradesInSemester(s);
        else if(choice == 5) binaryPrintInfoAboutStudent(s);
        else if(choice == 6) binaryPrintInfoAboutStudentsWithGradeGreaterThan(s);
        else if(choice == 7) binaryMakeAboutStudentsWith10AndOthers(s);
        else if(choice == 999) break;
        enterToContinue();
    }
}
