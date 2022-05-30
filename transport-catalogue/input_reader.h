#pragma once

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "geo.h"
#include "transport_catalogue.h"

class InputReader {

public:
	InputReader() = default;
	InputReader(std::vector<std::string>& datas, std::vector<std::string>& answers);

	std::pair<std::vector<transport_catalogue::Stop>, std::vector<transport_catalogue::Bus>> CreateData();
	std::unordered_map<std::string, std::unordered_map<std::string, size_t>> CreateDistances();
	std::vector<std::pair<std::string, std::string>> CreateAnswer();

private:
	std::vector<transport_catalogue::Bus> bus_request_;
	std::vector<transport_catalogue::Stop> stop_request_;
	std::unordered_map<std::string, std::unordered_map<std::string, size_t>> distances_between_stops_;

	std::vector<std::pair<std::string, std::string>> answer_request_;

private:
	void SplitRequest(std::vector<std::string>& datas, std::vector<std::string>& answers);
	void SplitDataRequest(std::string& st);
	void SplitAnswerRequest(std::string& st);
	void EraseSpaces(std::string& st);
};