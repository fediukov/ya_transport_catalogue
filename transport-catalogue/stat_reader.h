#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <set>
#include <string>
#include <vector>

#include "transport_catalogue.h"
#include "input_reader.h"

class StatReader {
public:
	void PrintBusInfo(transport_catalogue::TransportCatalogue& tc, const std::string_view name);
	void PrintStopInfo(transport_catalogue::TransportCatalogue& tc, const std::string_view name);
	void GetAnswers(transport_catalogue::TransportCatalogue& tc, InputReader& input_reader);
	
	// my functions
	void PrintAllStopsAndBuses(transport_catalogue::TransportCatalogue& tc);
};