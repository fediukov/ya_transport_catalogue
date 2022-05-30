#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "input_reader.h"
#include "transport_catalogue.h"
#include "stat_reader.h"

using namespace std;

int main()
{
    /*// test
13
Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino
Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324, 9500m to Marushkino
Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam
Stop Biryusinka: 55.581065, 37.64839, 750m to Universam
Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye
Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye
Stop Rossoshanskaya ulitsa: 55.595579, 37.605757
Stop Prazhskaya: 55.611678, 37.603831
6
Bus 256
Bus 750
Bus 751
Stop Samara
Stop Prazhskaya
Stop Biryulyovo Zapadnoye

//*/

    int data_count, answer_count;
    std::vector<std::string> data_requests;
    std::vector<std::string> answer_requests;
    std::string s;
    std::cin >> data_count;
    for (int i = 0; i < data_count+1; ++i)
    {
        getline(cin, s);
        data_requests.push_back(move(s));
    }
    std::cin >> answer_count;
    for (int i = 0; i < answer_count+1; ++i)
    {
        getline(cin, s);
        answer_requests.push_back(move(s));
    }


    InputReader input_reader(data_requests, answer_requests);

    transport_catalogue::TransportCatalogue tc(input_reader.CreateData(), input_reader.CreateDistances());

    StatReader stat_reader;
    stat_reader.GetAnswers(tc, input_reader);

    return 0;
}
