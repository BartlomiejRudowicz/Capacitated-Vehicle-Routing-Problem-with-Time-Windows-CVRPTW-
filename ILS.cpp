#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <random>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace std;
using namespace chrono;
// struktura reprezentujaca sciezke
struct Route {
    vector<vector<int> > answer;
    long double cost;
};
// struktura reprezentujaca dane z pliku
struct DanePlik {
    vector<vector<int> > customers;
    int capacity;
    int ciezarowki;
    int due_time_limit;
    int x_start;
    int y_start;
};
// Odczyt danych z pliku
DanePlik Load_Data(){
    DanePlik Dane_Plik;
    int s1;
    string savezone;
    ifstream dane("spraw1000.txt");
    int przechowanie;
    getline(dane, savezone);
    getline(dane, savezone);
    getline(dane, savezone);
    getline(dane, savezone);
    dane>>Dane_Plik.ciezarowki;
    dane>>Dane_Plik.capacity;
    getline(dane, savezone);
    getline(dane, savezone);
    getline(dane, savezone);
    getline(dane, savezone);
    dane>>s1>>Dane_Plik.x_start>>Dane_Plik.y_start>>s1>>s1>>Dane_Plik.due_time_limit>>s1;
    vector<int> linia;
    int czyspacje;
    while(!dane.eof())
    {
        czyspacje=0;
        getline(dane, savezone);
        if(savezone.empty()) continue;
        for (char c : savezone) {
            if (!isspace(c)) {
                czyspacje = 1;
                break;
            }
        }

        if (!czyspacje) {
            continue;
        }
        istringstream konw(savezone);
        for(int i=1; i<=7; i++)
        {
            konw>>przechowanie;
            linia.push_back(przechowanie);
        }
        Dane_Plik.customers.push_back(linia);
        linia.clear();
    }
    dane.close();
    return Dane_Plik;
}
// Funkcja obliczajaca odleglosc Euklidesowa miedzy dwoma punktami
long double euclideanDistance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}
// sortowanie klientow wzgledem konca pracy (najwiecej na koniec)
vector<vector<int> > sorting(vector<vector<int> > customers){
    int pom;
    for(int j=(customers.size()-1); j>0; j--)
        for(int i=0; i<j; i++)
    {
        if(customers[i][5]>customers[i+1][5])
        {
            for(int k=0; k<7;k++)
            {
            pom=customers[i][k];
            customers[i][k]=customers[i+1][k];
            customers[i+1][k]=pom;
            }
        }
    }
    return customers;
}
// sortowanie wedlug rozmiaru sciezek
bool sortcol(const vector<int>& v1, const vector<int>& v2)
    {
        return v1.size() < v2.size();
    }
// sprawdza czy mozna dodac klienta
long double AddNextClient(int due_time_limit, int capacity, int x_start, int y_start, int x_curr, int y_curr, long double current_cost, int current_demand, vector<int> customer){
    // dystans od obecnego klienta do nastepnego
    long double distance1 = euclideanDistance(x_curr, y_curr, customer[1], customer[2]);
    // dystans od nastepnego do magazynu
    long double distance2 = euclideanDistance(customer[1], customer[2], x_start, y_start);
    // czas od kiedy mozna zaczac robic serwis
    long double time = max(current_cost + distance1, (long double)customer[4]);
    // if sprawdzajacy czy mozna dodac klienta
    if(time+distance2+(long double)customer[6]<=due_time_limit && current_demand+customer[3]<=capacity && time <= customer[5]){
        // mozna - zwraca obecny czas powiekszony o droge (albo czas mozliwosci rozpoczecia serwisu) i serwis nast klienta
        return time+(long double)customer[6];
    }else{
        // nie mozna - zwraca obecny czas
        return current_cost;
    }
}
// Generuje losowe rozwiazanie
Route generateRandomSolution(const vector<vector<int> > customers, int capacity, int due_time_limit, int x_start, int y_start) {
    vector<vector<int> > shuffledCustomers = customers;
    random_device rd;
    mt19937::result_type seed = random_device()()
        ^duration_cast<seconds>(system_clock::now().time_since_epoch()).count()
        ^duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count();
        mt19937 gen(seed);
    // losowa kolejnosc klientow
    shuffle(shuffledCustomers.begin(), shuffledCustomers.end(), gen);
    vector<int> single_route;
    Route route;
    int current_demand = 0;
    long double current_time = 0;
    long double saved_time = 0;
    route.cost=0;
    route.answer.clear();

    current_time=AddNextClient(due_time_limit, capacity, x_start, y_start, x_start, y_start, current_time, current_demand, shuffledCustomers[0]);
    // AddNext jestli nie moze dodac pierwszego klienta to znaczy ze odp to -1
    if(current_time==0){
        return route;
    }else{
        // mozna dodac
        current_demand+=customers[0][3];
        single_route.push_back(shuffledCustomers[0][0]);
    }
    saved_time=current_time;
    bool dodano_rcl = false;
    vector<int> kopia_dodanego_rcl;
    for (int i = 1; i < shuffledCustomers.size(); ++i) {

        current_time=AddNextClient(due_time_limit, capacity, x_start, y_start, shuffledCustomers[i-1][1], shuffledCustomers[i-1][2], current_time, current_demand, shuffledCustomers[i]);
        // jesli nie mozna dodac losowego
        if(saved_time==current_time){

            //powrot do magazynu
            long double distance2 = euclideanDistance(shuffledCustomers[i-1][1], shuffledCustomers[i-1][2], x_start, y_start);
            current_time+=distance2;
            route.cost+=current_time;
            route.answer.push_back(single_route);
            // reset danych
            current_time=0;
            current_demand=0;
            single_route.clear();
            // dodajemy nowego (ktorego nie moglismy przed chwila)
            current_time=AddNextClient(due_time_limit, capacity, x_start, y_start, x_start, y_start, current_time, current_demand, shuffledCustomers[i]);
            saved_time=current_time;
            current_demand+=shuffledCustomers[i][3];
            single_route.push_back(shuffledCustomers[i][0]);
        }else{ // mozna dodac
            saved_time=current_time;
            current_demand+=shuffledCustomers[i][3];
            single_route.push_back(shuffledCustomers[i][0]);
        }
    }
    // powrot do magazynu
    long double distance2 = euclideanDistance(shuffledCustomers.back()[1], shuffledCustomers.back()[2], x_start, y_start);
    current_time+=distance2;
    route.cost+=current_time;
    route.answer.push_back(single_route);

    current_time=0;
    current_demand=0;
    single_route.clear();


    return route;
}
// usun zioma
vector<vector<int> > usun(vector<vector<int> > customers, vector<int> ziom){
    for(int i = 0; i < customers.size(); i++){
        if (customers[i][0] == ziom[0]){
            customers.erase(customers.begin() + i);
            break;
        }
    }
    return customers;
}
// oblicza koszt usunietej sciezki
long double koszt_usunietego(const vector<vector<int> > customers, int x_start, int y_start){
    long double cost = 0;
    cost += euclideanDistance(x_start, y_start, customers[0][1], customers[0][2]);
    long double time = max(cost, (long double)customers[0][4]);
    cost=time+(long double)customers[0][6];
    for (int i = 1; i < customers.size(); i++){
        cost += euclideanDistance(customers[i-1][1], customers[i-1][2], customers[i][1], customers[i][2]);
        long double time = max(cost, (long double)customers[i][4]);
        cost=time+(long double)customers[i][6];
    }
    cost += euclideanDistance(customers.back()[1], customers.back()[2], x_start, y_start);

    return cost;
}
// wyznacza najblizszego klienta mozliwego do dodania
vector<int> najbliszy(vector<vector<int> > customers, int due_time_limit, int capacity, int x_start, int y_start, int x_curr, int y_curr, long double current_cost, int current_demand, int rekurencja){
    long double czas = current_cost;
    vector<vector<int> > customers_kopia = customers;
    vector<vector<int> > potencjalni;
    vector<int> do_return = {-1, -1, -1};
    int parametr = 5;
    if(customers.size()==0 || rekurencja == 19){
        return do_return;
    }
    // wyznaczenie najblizszego
    while(potencjalni.size()==0 && customers.size()>0){
        int spr = 0;
        for (int i = 0; i < customers.size(); i++) {
            if(((abs(x_curr-customers[i][1])) <= parametr) && ((abs(y_curr-customers[i][2])) <= parametr) && (current_demand+customers[i][3])<=capacity){
                potencjalni.push_back(customers[i]);
            }
            if((current_demand+customers[i][3])>capacity){
                spr++;
            }
            if(spr==customers.size()){
                return do_return;
            }
        }

        parametr+=10;
    }
    // sprawdzanie czy mozna go dodac
    if(potencjalni.size()==1){
        czas = AddNextClient(due_time_limit, capacity, x_start, y_start, x_curr, y_curr, czas, current_demand, potencjalni[0]);
        if (czas==current_cost) {
            customers = usun(customers, potencjalni[0]);
            vector<int> wybraniec = najbliszy(customers, due_time_limit, capacity, x_start, y_start, x_curr, y_curr, czas, current_demand, rekurencja+1);
            return wybraniec;
        }else{
            return potencjalni[0];
        }
    }else{
        srand (time(NULL));
        int wybraniec = rand() % potencjalni.size();
        czas = AddNextClient(due_time_limit, capacity, x_start, y_start, x_curr, y_curr, czas, current_demand, potencjalni[wybraniec]);
        if (czas==current_cost) {
            customers = usun(customers, potencjalni[wybraniec]);
            vector<int> wybraniec = najbliszy(customers, due_time_limit, capacity, x_start, y_start, x_curr, y_curr, czas, current_demand, rekurencja+1);
            return wybraniec;
        }else{
            return potencjalni[wybraniec];
        }
    }


}
// Generuj rozwiazanie greedy
Route generateGreedySolution(vector<vector<int> > customers, int capacity, int due_time_limit, int x_start, int y_start) {
    vector<vector<int> > customers_kopia = customers;
    vector<int> single_route;
    Route route;
    int current_demand = 0;
    long double current_time = 0;
    long double saved_time = 0;
    route.cost=0;
    route.answer.clear();

    vector<vector<int> > kolejnosc_dodawania;

    current_time=AddNextClient(due_time_limit, capacity, x_start, y_start, x_start, y_start, current_time, current_demand, customers[0]);
    // AddNext jestli nie moze dodac pierwszego klienta to znaczy ze odp to -1
    if(current_time==0){
        return route;
    }else{
        // mozna dodac
        current_demand+=customers[0][3];
        single_route.push_back(customers[0][0]);
        kolejnosc_dodawania.push_back(customers[0]);
        customers_kopia = usun(customers_kopia, customers[0]);
    }
    saved_time=current_time;

    vector<int> do_dodania;
    for (int i = 1; i < customers.size(); i++) {

        do_dodania = najbliszy(customers_kopia, due_time_limit, capacity, x_start, y_start, kolejnosc_dodawania.back()[1], kolejnosc_dodawania.back()[2], current_time, current_demand, 1);

        if(do_dodania[0]!=(-1)){
            current_time=AddNextClient(due_time_limit, capacity, x_start, y_start, kolejnosc_dodawania.back()[1], kolejnosc_dodawania.back()[2], current_time, current_demand, do_dodania);

            customers_kopia = usun(customers_kopia, do_dodania);
            saved_time=current_time;
            current_demand+=do_dodania[3];
            single_route.push_back(do_dodania[0]);
            kolejnosc_dodawania.push_back(do_dodania);
        }else{

            //powrot do magazynu
            long double distance2 = euclideanDistance(kolejnosc_dodawania.back()[1], kolejnosc_dodawania.back()[2], x_start, y_start);
            current_time+=distance2;
            route.cost+=current_time;
            route.answer.push_back(single_route);
            // reset danych
            current_time=0;
            current_demand=0;
            single_route.clear();
            // dodajemy nowego (ktorego nie moglismy przed chwila)
            current_time=AddNextClient(due_time_limit, capacity, x_start, y_start, x_start, y_start, current_time, current_demand, customers_kopia[0]);
            kolejnosc_dodawania.push_back(customers_kopia[0]);
            saved_time=current_time;
            current_demand+=kolejnosc_dodawania.back()[3];
            single_route.push_back(customers_kopia[0][0]);
            customers_kopia = usun(customers_kopia, customers_kopia[0]);
        }
    }
    // powrot do magazynu
    long double distance2 = euclideanDistance(kolejnosc_dodawania.back()[1], kolejnosc_dodawania.back()[2], x_start, y_start);
    current_time+=distance2;
    route.cost+=current_time;
    route.answer.push_back(single_route);

    current_time=0;
    current_demand=0;
    single_route.clear();

    return route;
}
// zburz iles scizek
vector<int> zburzone(Route currentSolution){
    vector<int> zburzoneID;
    int parametr = 50; // w %
    int ile_usunac = currentSolution.answer.size()*parametr/100;
    ile_usunac +=1;

    srand (time(NULL));
    int metoda = rand() % 2;
    metoda = 0;
    if (metoda==0){
        // losowo
        for (int i = 0; i < ile_usunac; i++){
            int usun = rand() % currentSolution.answer.size();
            for(int j = 0; j < currentSolution.answer[usun].size(); j++){
                zburzoneID.push_back(currentSolution.answer[usun][j]);
            }
            currentSolution.answer.erase(currentSolution.answer.begin() + usun);
        }
    }else{
        // capacity
    }

    return zburzoneID;
}

// zwraca najlepsze znalezione rozwiaznie
Route grasp(const vector<vector<int> > customers, int capacity, int due_time_limit, int x_start, int y_start, int czas, int customers_size) {
    auto start=high_resolution_clock::now();

    vector<vector<int> > sorted_customers = sorting(customers);

    Route bestSolution = generateRandomSolution(sorted_customers, capacity, due_time_limit, x_start, y_start);

    int sprawdz = 0;
    for (int i = 0; i < bestSolution.answer.size(); ++i) {
            for (int j = 0; j < bestSolution.answer[i].size(); ++j) {
                sprawdz++;
            }

        }
    if(sprawdz!=customers_size){
        return bestSolution;
    }
    Route currentSolution;
    auto ending=high_resolution_clock::now();
    auto difference = duration_cast<microseconds>(ending-start);

    Route do_dodania_random;
    Route do_dodania_greedy;

    vector<int> zburzoneID;
    vector<int> ile_klientow_w_sciezce;

    int parametr = 13; // w %
    int ile_usunac = currentSolution.answer.size()*parametr/100;
    long double cost_usunietego;


    vector<vector<int> > zburzeni_dane;
    vector<int> zburzony;

    currentSolution = generateRandomSolution(sorted_customers, capacity, due_time_limit, x_start, y_start);

    int liczba_iteracji = 0;
    int best_iteracja = 0;
    while(difference.count()<czas*1000000) {
        zburzoneID.clear();
        if (currentSolution.answer.size() < bestSolution.answer.size()) {
            bestSolution = currentSolution;
            best_iteracja = liczba_iteracji;
        }
        liczba_iteracji++;
        if(liczba_iteracji-best_iteracja>20){
            currentSolution=bestSolution;
            best_iteracja=liczba_iteracji;
        }
        ile_usunac = currentSolution.answer.size()*parametr/100;

        ile_usunac +=1;
        if(ile_usunac>currentSolution.answer.size()){
            ile_usunac=currentSolution.answer.size();
        }
        srand (time(NULL));
        int metoda = rand() % 2;
        // burrzenie
        if (metoda==0){
            // losowo
            for (int i = 0; i < ile_usunac; i++){
                int usun = rand() % currentSolution.answer.size();
                ile_klientow_w_sciezce.push_back(currentSolution.answer[usun].size());
                for(int j = 0; j < currentSolution.answer[usun].size(); j++){

                    zburzoneID.push_back(currentSolution.answer[usun][j]);
                }
                currentSolution.answer.erase(currentSolution.answer.begin() + usun);
            }
        }else{
            // capacity
            int l_usunietych=0;
            sort(currentSolution.answer.begin(), currentSolution.answer.end(), sortcol);
            for (int i = 0; i < currentSolution.answer.size(); i++){
                if (l_usunietych<ile_usunac){
                    ile_klientow_w_sciezce.push_back(currentSolution.answer[0].size());
                    for(int j = 0; j < currentSolution.answer[0].size(); j++){
                        zburzoneID.push_back(currentSolution.answer[0][j]);

                    }

                    currentSolution.answer.erase(currentSolution.answer.begin());
                    l_usunietych++;
                }else{
                    break;
                }
            }
        }
        zburzeni_dane.clear();
        // dane zburzonych klientow
        for(int i=0; i<zburzoneID.size(); i++)
        {
            for(int j=0; j<sorted_customers.size(); j++)
            {

                if(sorted_customers[j][0]==zburzoneID[i])
                {
                    for(int k=0; k<7; k++)
                    {
                        zburzony.push_back(sorted_customers[j][k]);
                    }
                    zburzeni_dane.push_back(zburzony);
                    zburzony.clear();
                    break;
                }
            }
        }

        // obliczenie i odjecie kosztu usunietych sciezek
        vector<vector<int> > usunieta_sciezka;
        int n = 0;
        long double koszt_sciezki = 0;
        for(int j=0; j<ile_klientow_w_sciezce.size(); j++){
            for (int i = 0; i<ile_klientow_w_sciezce[j]; i++){
                usunieta_sciezka.push_back(zburzeni_dane[n++]);
            }
            koszt_sciezki += koszt_usunietego(usunieta_sciezka, x_start, y_start);
            usunieta_sciezka.clear();
        }
        ile_klientow_w_sciezce.clear();
        currentSolution.cost -= koszt_sciezki;

        // dodaj usunietych
        if(zburzeni_dane.size()>0){
            do_dodania_greedy = generateGreedySolution(zburzeni_dane, capacity, due_time_limit, x_start, y_start);


            for(int i=0; i<do_dodania_greedy.answer.size(); i++)
            {
                currentSolution.answer.push_back(do_dodania_greedy.answer[i]);
            }
            currentSolution.cost+=do_dodania_greedy.cost;

        }
        if (currentSolution.answer.size()<bestSolution.answer.size()){
            bestSolution = currentSolution;
        }
        ending=high_resolution_clock::now();
        difference = duration_cast<microseconds>(ending-start);
    }

    sort(bestSolution.answer.begin(), bestSolution.answer.end(), sortcol);
    return bestSolution;
}

int main() {
    DanePlik Dane = Load_Data();
    int czas=60;
    Route solution = grasp(Dane.customers, Dane.capacity, Dane.due_time_limit, Dane.x_start, Dane.y_start, czas, Dane.customers.size());

    int czydozwolone=1;
    int liczba=0;
    int lciez=0;
    for (int i = 0; i < solution.answer.size(); ++i) {
            lciez++;
        for (int j = 0; j < solution.answer[i].size(); ++j) {
            liczba++;
        }
    }
    ofstream wyniki("wynik.txt");
    if(liczba==Dane.customers.size())
    {
    wyniki<<lciez<<" ";
    ostringstream ss;
    ss.precision(8);
    ss << fixed << ((solution.cost*100000)/100000);
    wyniki<< ss.str()<<endl;
    for (int i = 0; i < solution.answer.size(); ++i) {
        for (int j = 0; j < solution.answer[i].size(); ++j) {
            wyniki << solution.answer[i][j] << " ";
        }
        wyniki << endl;
    }
    wyniki<<endl;
    }
    else wyniki<<"-1"<<endl;
    wyniki.close();
    return 0;
}
