#pragma once

#include <map>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "geo.h"
#include "transport_catalogue.h"

class InputReader {

public:
	InputReader() = default;

	void FillTransportCatalogue(transport_catalogue::TransportCatalogue& tc);

private:
	std::vector<transport_catalogue::Bus> bus_request_;
	std::vector<transport_catalogue::Stop> stop_request_;
	std::unordered_map<std::string, std::unordered_map<std::string, size_t>> distances_between_stops_;

private:
	void CinRequest();

	void SplitDataRequest(std::string& st);
	transport_catalogue::Bus IsBusOfDataRequest(std::string& st);
	transport_catalogue::Stop IsStopOfDataRequest(std::string& st);
	std::pair<std::string, size_t> AreDistance(std::string& st);
	std::string GetNameOfRequest(std::string& st, const char end_of_name);
	void EraseSpaces(std::string& st);
};