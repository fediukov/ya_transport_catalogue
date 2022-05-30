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
	void GetAnswers(transport_catalogue::TransportCatalogue& tc);

	void PrintBusInfo(transport_catalogue::TransportCatalogue& tc, const std::string_view name);
	void PrintStopInfo(transport_catalogue::TransportCatalogue& tc, const std::string_view name);

	// my functions
	void PrintAllStopsAndBuses(transport_catalogue::TransportCatalogue& tc);

private:
	std::vector<std::pair<std::string, std::string>> answer_request_;

private:
	void CinRequest();
	void SplitAnswerRequest(std::string& st);
	void EraseSpaces(std::string& st);
};