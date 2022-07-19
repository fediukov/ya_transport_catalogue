#pragma once

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <utility>

#include "domain.h"
#include "router.h"
//#include "transport_catalogue.h"

struct RoutingSettings
{
	int bus_wait_time_ = 6;
	double bus_velocity_ = 40;
};

class TransportRouter {
public:
	// constructors
	TransportRouter() = default;

	// operator()
	TransportRouter& operator()(
		const std::vector<domain::Bus>& buses,
		const std::vector<domain::Stop>& stops,
		const std::unordered_map<std::string, std::unordered_map<std::string, size_t>>& distances,//*/
		//const JsonReader& jr,
		const RoutingSettings routing_settings
		)
	{
		routing_settings_ = routing_settings;
		SetGraph(buses, stops, distances);
		//SetGraph(jr);
		router_ = std::make_unique<graph::Router<double>>(graph_);
		return *this;
	}

	// struct of answer
	struct RouterInfo {
		std::string type_ = "";
		double time_ = 0.;
		std::string stop_name_ = "";
		std::string bus_name_ = "";
		int stop_count_ = 0;
		double total_time_ = 0.;
	};

	std::vector<RouterInfo> GetRoute(std::string stop_from, std::string stop_to);

private:
	void SetGraph(
		const std::vector<domain::Bus>& buses,
		const std::vector<domain::Stop>& stops,
		const std::unordered_map<std::string, std::unordered_map<std::string, size_t>>& distances//*/
	);
	void AddEdgesOfRoute(const domain::Bus& bus, size_t pos_from, size_t pos_to,
		const std::unordered_map<std::string, std::unordered_map<std::string, size_t>>& distances);
	double GetDistanceBetweenTwoStops(const std::string& from, const std::string& to,
		const std::unordered_map<std::string, std::unordered_map<std::string, size_t>>& distances);

private:
	graph::DirectedWeightedGraph<double> graph_;
	std::unique_ptr<graph::Router<double>> router_;

	std::unordered_map<std::string_view, size_t> name_id_of_stops_;
	std::unordered_map<size_t, std::string_view> id_name_of_stops_;
	std::unordered_map<size_t, std::string_view> id_name_of_buses_;
	std::unordered_map<size_t, int> id_count_of_stops;

	RoutingSettings routing_settings_;
};

