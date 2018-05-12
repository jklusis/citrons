#include <iostream>
#include <fstream>
#include <string.h>
#include <iomanip>
#include <map>

using namespace std;

struct prece
{
    
    char nosaukums[51];
    double cena;
    char tips[2];

    prece(const char *nos, double c, const char *t)
    {
        strcpy(nosaukums, nos);
        cena = c;
        strcpy(tips, t);
    }
};

class datubaze
{
    int kods;
    int n_garums;
    char nosaukums[51];
    double cena;
    char tips[2];

    prece *p; // Norāde uz struct
    map<int, prece*> db; // Pati datu bāze

    public:
    datubaze(fstream &fails) // Konstruktorā tiek veikta binārā faila ielasīšana.
    {
        n_garums = 0; // Lai novērstu segmentation fault, jo reizēm tika iedalīta atmiņa, kurā pārējie 3 baiti bija ar kaut ko.
        while(fails.read((char*)&kods, sizeof(int)))
        {
            fails.read((char*)&n_garums, 1);
            fails.read(nosaukums, n_garums);
            fails.read((char*)&cena, sizeof(double));
            fails.read(tips, 1);

            nosaukums[n_garums] = 0; // Ievietojam atpakaļ beigu simbolus
            tips[1] = 0; // ...
            p = new prece(nosaukums, cena, tips);
            db.insert (pair<int,prece*>(kods, p));
        }
    };

    ~datubaze() // Destruktorā tiek iznīcināti visi izveidotie structi.
    {
        for (auto it = db.begin(); it != db.end(); it++)
        {
            delete it->second;
            db.erase(it);
        }
    }

    void printAll() // Izdrukā visus datubāzes elementus
    {
        int i = 1;
        for (auto it = db.begin(); it != db.end(); it++, i++)
            cout << i << ". prece, kods: " << it->first << ", nosaukums: " << it->second->nosaukums << ", cena: " << it->second->cena << ", tips: " << it->second->tips << endl;
    }
    
    bool get(int &k, char *n, double &c, char *t) // Atgriež preci pēc svītru koda, ievietojot visu mainīgajos.
    {
        auto it = db.find(k); // Tiek sameklēts pēc atslēgas.

        if(it == db.end()) // Ja ir sasniegtas beigas (elements nav atrasts)
        {
            cout << "Prece ar kodu " << k << " datubaze netika atrasta!" << endl;
            return false;
        }
        strcpy(n, it->second->nosaukums);
        c = it->second->cena;
        strcpy(t, it->second->tips);
        //cout << "Prece: " << it->first << " " << it->second->nosaukums << " " << it->second->cena << " " << " " << it->second->tips << endl; 
        return true;
    }

    string getName(int k)
    {
        auto it = db.find(k); // Tiek sameklēts pēc atslēgas.
        return it->second->nosaukums;
    }

    double getPrice(int k)
    {
        auto it = db.find(k); // Tiek sameklēts pēc atslēgas.
        return it->second->cena;
    }

};

class kase : public datubaze
{
    int kods;
    char nosaukums[51];
    double cena;
    char tips[2];

    double amount; // Preces daudzums - var būt gan veseli skaitļi daudzumā, gan decimāli svarā.

    int nr; // Katru reizi startējot programmu, čeka nr. būs 0. Ar katru pirkumu ++.

    map<int, double> preces; // Preču saraksts, uzglabā kodu un daudzumu. 

    int choice;

    public:
    kase(fstream &fails) : datubaze(fails)
    {
        cout << "-- Laipni lugti pasapkalposanas sisteemaa!" << endl;
        nr = 0; // nr tiek iestatīts par 0.
    };

    void izvelne()
    {
        while(true)
        {
            cout << "-- Iespejamas darbibas:" << endl;
            cout << "1. Noskenet preces." << endl;
            cout << "2. Nonemt preci." << endl;
            cout << "3. Apskatit sobridejo pirkumu sarakstu." << endl;
            cout << "4. Veikt pirkumu." << endl;
            cout << "5. Atcelt pirkumu." << endl;
            // 13, lai beigtu.
            cout << "-- Ievadiet darbibu, kuru velaties veikt:" << endl;
            cin >> choice;

            if(choice == 1) scan();
            else if(choice == 2) remove();
            else if(choice == 3) print();
            else if(choice == 4) checkOut();
            else if(choice == 5) cancel();
            else if(choice == 13) return;
        }
    }

    void scan() // Skenēt preces
    {
        while(true)
        {
            cout << "-- Ludzu ievadiet preces kodu! (-1 koda vietaa, lai beigtu ievadi)" << endl;
            cin >> kods;
            if(kods == -1) break;
            
            if(get(kods, nosaukums, cena, tips) == false) continue;

            if(tips[0] == '0')
            {
                cout << "-- Noskeneta prece: " << nosaukums << ", cena par vienu vienibu: " << cena << " eur" << endl;
                cout << "-- Ludzu ievadiet, cik \"" << nosaukums << "\" velaties iegadaties!" << endl;
                while(true)
                {
                    cin >> amount;
                    if(amount != static_cast<int>(amount)) cout << "-- Ludzu ievadiet veselu skaitli!" << endl;
                    else if(amount < 1) cout << "-- Ludzu ievadiet vismaz vienu vienibu!" << endl; 
                    else break;
                }
            }
            else
            {
                cout << "-- Noskeneta prece: " << nosaukums << ", cena par vienu kilogramu: " << cena << " eur/kg" <<  endl;
                cout << "-- Ludzu ievadiet, cik kilogramus \"" << nosaukums << "\" velaties iegadaties!" << endl;
                while(true)
                {
                    cin >> amount;
                    if(amount < 0) cout << "-- Svars nevar but negativs!" << endl;
                    else break;
                }
            }


            // Ja prece ar kodu nav iepriekš noskenēta, izveidojam jaunu
            if(checkIfKeyExists() == false)
            {
                preces.insert (pair<int, double>(kods, amount));
                cout << "-- Prece " << nosaukums << " veiksmigi pievienota, cena par " << amount << " vienibam: " << cena*amount << " eur" << endl;

            }

            // Ja ir, pieskaitam skaitu iepriekšējai.
            else addToKey();
        }
    }

    void remove() // Noņemt kādu konkrētu preci pēc svītru koda
    {
        cout << "-- Ludzu ievadiet svitru kodu precei, kuru velaties nonemt." << endl;
        cin >> kods;

        if(checkIfKeyExists() == false)
        {
            cout << "-- Prece ar kodu " << kods << " nav Jusu pirkumu sarakstaa." << endl;
            return;
        }

        auto it = preces.find(kods);
        cout << "-- Vai tiesam velaties nonemt preci " << getName(kods) << "? Daudzums " << it->second << " vienibas, cena " << it->second*getPrice(kods) << endl;
        cout << "-- 1: jaa, jebkas cits: nee" << endl;
        cin >> choice;

        if(choice == 1)
        {
            preces.erase(kods);
            cout << "-- Prece tika veiksmigi nonemta." << endl;
        }

        else
        {
            cout << "-- Prece netika nonemta." << endl;
        }
        enterToContinue();
    }

    void print() // Izdrukāt visas šobrīd esošās preces
    {
        int i = 1;
        double sum = 0;
        cout << "-- Jusu pirkumu saraksts:" << endl;

        for(auto it = preces.begin(); it != preces.end(); it++, i++)
        {
            cout << i << ". Kods: " << it->first << ", nosaukums: " << getName(it->first) << ", cena par vienibu: " << getPrice(it->first) << ", daudzums: " << it->second << ", cena: " << getPrice(it->first)*it->second << " eur" << endl;
            sum += getPrice(it->first)*it->second;
        }
        
        cout << "-- Pirkuma saraksta beigas." << endl;
        cout << "-- Kopeejaa pirkumu summa: " << sum << " eur" << endl;
        enterToContinue();
    }

    void checkOut() // Izdrukāt čeku ar pirkuma sarakstu uz ekrāna un failā, izsaukt clear()
    {
        string vards;
        string file_name;

        if(productCount() == 0)
        {
            cout << "-- Jusu pirkumu saraksts ir tukss!" << endl;
            return;
        }

        cout << "Ludzu ievadiet savu vardu: " << endl;
        cin >> vards;
        
        fstream output(to_string(nr) + "_" + vards + ".txt", ios::out);

        output << "-------------------------------------------------" << endl;
        output << "Ceks nr: " << nr << ", pirceja vards: " << vards << endl;
        output << "-------------------------------------------------" << endl;

        cout << "-------------------------------------------------" << endl;
        cout << "Ceks nr: " << nr << ", pirceja vards: " << vards << endl;
        cout << "-------------------------------------------------" << endl;
        
        int i = 1;
        double sum = 0;
        
        for(auto it = preces.begin(); it != preces.end(); it++, i++)
        {
            output << i << ". Nosaukums: " << getName(it->first) << ", cena par vienibu: " << getPrice(it->first) << ", daudzums: " << it->second << ", cena: " << getPrice(it->first)*it->second << " eur" << endl;
            cout << i << ". Nosaukums: " << getName(it->first) << ", cena par vienibu: " << getPrice(it->first) <<  ", daudzums: " << it->second << ", cena: " << getPrice(it->first)*it->second << " eur" << endl;
            sum += getPrice(it->first)*it->second;
        }

        output << "-------------------------------------------------" << endl;
        cout << "-------------------------------------------------" << endl;
        
        output << "Kopeejaa pirkuma summa: " << sum << " eur" << endl;
        cout << "Kopeejaa pirkuma summa: " << sum << " eur" << endl;

        output << "-------------------------------------------------" << endl;
        cout << "-------------------------------------------------" << endl;

        enterToContinue();

        nr++; // Ceka numurs aug.
        clear();
    }

    void cancel() // Izsaukt clear
    {
        cout << "-- Vai Jus tiesam velaties atcelt pirkumu? (1 - jaa, jebkas cits nee.)" << endl;
        cin >> choice;
        if(choice == 1) clear();
    }

    /* Funkcijas, kas lietotājam nav pieejamas */

    void clear() // Funkcija, kurā tiek notīrīts viss saturs, lai nākamais pircējs var izmantot.
    {
        for(int i = 0; i < 250; i++)
            cout << endl;

        preces.clear();
        kods = 0;
        memset(nosaukums, 0, 51);
        cena = 0;
        amount = 0;
        cout << "-- Laipni lugti pasapkalposanas sisteemaa!" << endl;
    }

    bool checkIfKeyExists()
    {
        auto it = preces.find(kods); // Tiek sameklēts pēc atslēgas.
        if(it == preces.end()) return false;
        return true;
    }

    void addToKey()
    {
        auto it = preces.find(kods);
        it->second += amount;
        cout << "-- Prece " << nosaukums << " veiksmigi pievienota esosajaam vieniibaam, cena par " << it->second << " vienibam: " << cena*it->second << " eur" << endl;
    }

    int productCount()
    {
        int i = 0;
        for (auto it = preces.begin(); it != preces.end(); it++, i++);
        return i;
    }

    void enterToContinue()
    {
        string please;
        cout << "Ludzu ievadiet jebko un enter, lai turpinatu.." << endl;
        cin >> please;
    }
};

int main()
{
    cout << fixed;
    cout << setprecision(2); // 2 cipari aiz komata.

    /* Programmas sagatavošanās darbam */
    fstream fails("preces.bin", ios::in | ios::binary);

    if(!fails) // Ja datubāzes failu neizdevās nolasīt.
    {
        cout << "Neizdevaas nolasiit precu datubaazi." << endl;
        return 13;
    }
    
    kase k(fails); // Ja izdevās, tiek startēta klase kase, kurā ir klase datubāze, kurai tiek padots datubaazes fstreams, no kura nolasīt.

    k.izvelne(); // Ar izvēlni var izsaukt visas citas komandas.

    cout << "Jauku dienu!" << endl;

}