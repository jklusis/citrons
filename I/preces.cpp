#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

void createFile(int &kods, int &p_n_garums, char *preces_nosaukums, double &preces_cena, char *preces_tips);
void readFile(int &kods, int &p_n_garums, char *preces_nosaukums, double &preces_cena, char *preces_tips);

int main()
{
    // Mainīgo deklarēšana, tie tiks izmantoti visos gadījumos.
    int kods;
    int p_n_garums; // Preces nosaukuma garums
    char preces_nosaukums[51]; // Līdz 50 simboliem.
    double preces_cena;
    char preces_tips[2]; // 2 baiti (1 baits + beigu simbols), glabās 0 vai 1.

    int izvelne;

    while(true)
    {
        cout << "-- Precu saraksta izveide un nolasisana." << endl;
        cout << "1. Izveidot failu." << endl;
        cout << "2. Nolasit failu." << endl;
        cout << "3. Beigt darbu." << endl;
        cout << "-- Ievadiet izveli:" << endl;
        cin >> izvelne;

        if(izvelne == 1) createFile(kods, p_n_garums, preces_nosaukums, preces_cena, preces_tips);
        if(izvelne == 2) readFile(kods, p_n_garums, preces_nosaukums, preces_cena, preces_tips);
        if(izvelne == 3) break;
    }

    cout << "Jauku dienu!" << endl;
}

void createFile(int &kods, int &p_n_garums, char *preces_nosaukums, double &preces_cena, char *preces_tips)
{
    string file_name;
    cout << "-- Ludzu ievadiet pilno faila nosaukumu precu failam, kuru velaties izveidot!" << endl;
    cout << "-- (Piem. preces.bin)" << endl;
    cin >> file_name;
    fstream output(file_name, ios::out | ios::binary);

    cout << "-- Ludzu ievadiet kodu, preces nosaukumu, preces cenu un preces tipu. (-1 koda vietaa lai beigtu.)" << endl;
    while(true)
    {
        cin >> kods;
        if(kods < 0) break;
        cin >> preces_nosaukums >> preces_cena >> preces_tips;

        p_n_garums = strlen(preces_nosaukums);
        
        output.write((char*)&kods, sizeof(int));
        output.write((char*)&p_n_garums, 1);
        output.write(preces_nosaukums, p_n_garums);
        output.write((char*)&preces_cena, sizeof(double));
        output.write(preces_tips, 1);
    }

    cout << "-- Fails " << file_name << " veiksmigi izveidots!" << endl;

    output.close();
}

void readFile(int &kods, int &p_n_garums, char *preces_nosaukums, double &preces_cena, char *preces_tips)
{
    string file_name;
    cout << "-- Ludzu ievadiet pilno faila nosaukumu precu failam, kuru velaties nolasit!" << endl;
    cout << "-- (Piem. preces.bin)" << endl;
    cin >> file_name;
    fstream input(file_name, ios::in | ios::binary);
    
    if(!input) // Ja failu neizdevaas nolasiit.
    {
        cout << "-- Failu " << file_name << " neizdevas nolasit!" << endl;
        return;
    }

    cout << "-- Faila " << file_name << " sakums" << endl;
    for(int i = 1; input.read((char*)&kods, sizeof(int)); i++)
    {
        input.read((char*)&p_n_garums, 1); // Nolasam preces garumu viena baita garumā
        input.read(preces_nosaukums, p_n_garums); // Preces nosaukums precīzā garumā.
        input.read((char*)&preces_cena, sizeof(double));
        input.read(preces_tips, 1);

        preces_nosaukums[(int)p_n_garums] = 0; // Ievietojam beigu simbolu atpakaļ.

        cout << i << ". prece, kods: " << kods << ", nosaukums " << preces_nosaukums << ", cena: " << preces_cena << ", tips: " << preces_tips[0] << endl;

    }
    cout << "-- Faila " << file_name << " beigas." << endl;

    input.close();

}
