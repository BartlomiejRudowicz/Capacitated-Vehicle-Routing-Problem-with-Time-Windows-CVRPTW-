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
    string syf;
    ifstream dane("spraw1000.txt");
    int przechowanie;
    getline(dane, syf);
    getline(dane, syf);
    getline(dane, syf);
    getline(dane, syf);
    dane>>Dane_Plik.ciezarowki;
    dane>>Dane_Plik.capacity;
    getline(dane, syf);
    getline(dane, syf);
    getline(dane, syf);
    getline(dane, syf);
    dane>>s1>>Dane_Plik.x_start>>Dane_Plik.y_start>>s1>>s1>>Dane_Plik.due_time_limit>>s1;
    vector<int> linia;
    int czyspacje;
    while(!dane.eof())
    {
        czyspacje=0;
        getline(dane, syf);
        if(syf.empty()) continue;
        for (char c : syf) {
            if (!isspace(c)) {
                czyspacje = 1;
                break;
            }
        }

        if (!czyspacje) {
            continue;
        }
        istringstream konw(syf);
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
// Funkcja obliczająca odleglosc Euklidesowa miedzy dwoma punktami
double euclideanDistance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}
// sprawdza czy mozna dodac klienta
long double AddNextClient(int due_time_limit, int capacity, int x_start, int y_start, int x_curr, int y_curr, long double current_cost, int current_demand, vector<int> customer){
    //cale jak mozesz
    long double distance1 = euclideanDistance(x_curr, y_curr, customer[1], customer[2]);
    long double distance2 = euclideanDistance(customer[1], customer[2], x_start, y_start);
    long double time = max(current_cost + distance1, (long double)customer[4]);
    if(time+distance2+(long double)customer[6]<=due_time_limit && current_demand+customer[3]<=capacity && time <= customer[5]){
        return time+(long double)customer[6];
    }else{
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
    if(current_time==0){
        return route;
    }else{
        current_demand+=customers[0][3];
        single_route.push_back(shuffledCustomers[0][0]);
    }
    saved_time=current_time;

    for (int i = 1; i < shuffledCustomers.size(); ++i) {
        current_time=AddNextClient(due_time_limit, capacity, x_start, y_start, shuffledCustomers[i-1][1], shuffledCustomers[i-1][2], current_time, current_demand, shuffledCustomers[i]);

        if(saved_time==current_time){
            long double distance2 = euclideanDistance(shuffledCustomers[i-1][1], shuffledCustomers[i-1][2], x_start, y_start);
            current_time+=distance2;
            route.cost+=current_time;
            route.answer.push_back(single_route);

            current_time=0;
            current_demand=0;
            single_route.clear();
            /////////////////////
            current_time=AddNextClient(due_time_limit, capacity, x_start, y_start, x_start, y_start, current_time, current_demand, shuffledCustomers[i]);
            saved_time=current_time;
            current_demand+=shuffledCustomers[i][3];
            single_route.push_back(shuffledCustomers[i][0]);
        }else{
            saved_time=current_time;
            current_demand+=shuffledCustomers[i][3];
            single_route.push_back(shuffledCustomers[i][0]);
        }
    }

    long double distance2 = euclideanDistance(shuffledCustomers.back()[1], shuffledCustomers.back()[2], x_start, y_start);
    current_time+=distance2;
    route.cost+=current_time;
    route.answer.push_back(single_route);

    current_time=0;
    current_demand=0;
    single_route.clear();
    return route;
}
// zwraca najlepsze losowe rozwiaznie
Route grasp(const vector<vector<int> > customers, int capacity, int due_time_limit, int x_start, int y_start) {
    auto start=high_resolution_clock::now();
    Route bestSolution = generateRandomSolution(customers, capacity, due_time_limit, x_start, y_start);

    Route currentSolution;
    auto ending=high_resolution_clock::now();
    auto difference = duration_cast<microseconds>(ending-start);
    while(difference.count()<1*1000000) {
        currentSolution = generateRandomSolution(customers, capacity, due_time_limit, x_start, y_start);
        if (currentSolution.cost < bestSolution.cost) {
            bestSolution = currentSolution;
        }
        ending=high_resolution_clock::now();
        difference = duration_cast<microseconds>(ending-start);
    }


    return bestSolution;
}
int main() {
    DanePlik Dane = Load_Data();
    // Wywolaj algorytm GRASP
    Route solution = grasp(Dane.customers, Dane.capacity, Dane.due_time_limit, Dane.x_start, Dane.y_start);


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

