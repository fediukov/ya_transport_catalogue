#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"
//#include "tests.h"

using namespace std;

int main()
{
    //Test01(); // without json (using input_reader and stat_reader)
    //Test02(); // json
    //Test03(); // checking sphere projector

	/*
	*/

	transport_catalogue::TransportCatalogue tc;
	JsonReader jr;
    MapRenderer mr;

	//std::ifstream file("input.json");
	//std::istream& input = file;
	//jr.FillTransportCatalogue(input, tc);
	jr.FillTransportCatalogue(std::cin, tc);
	jr.ConnectMapRenderer(mr);
	jr.GetStatRequest(std::cout, tc, mr);

    //mr.Draw(std::cout, tc);

    return 0;
}
