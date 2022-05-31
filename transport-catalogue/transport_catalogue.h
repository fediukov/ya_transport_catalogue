#pragma once

#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>


#include "geo.h"

namespace transport_catalogue {

	struct Stop
	{
		std::string name;
		Coordinates coordinates;
	};

	struct Bus
	{
		Bus() = default;

		std::string name;
		std::vector<Stop*> route;
		/*double geo_distance = -1.;
		double road_distance = -1.;
		double curvature = -1.;
		int unique_stops = -1;//*/
	};

	struct BusStat
	{
		int count_stops = -1;
		int unique_stops = -1;
		double geo_distance = -1.;
		double road_distance = -1.;
		double curvature = -1.;
	};


	class TransportCatalogue {
	public:
		TransportCatalogue() = default;
		~TransportCatalogue();

		// create db
		void AddStop(const Stop& stop);
		void AddBus(const Bus& bus);
		void SetDistances(std::string_view from, std::string_view to, size_t distance);

		// get db parts
		//Bus* GetBusInfo(const std::string_view name);
		const BusStat GetBusInfo(const std::string_view name);
		std::set<Bus*>* GetStopInfo(const std::string_view name);
		const Stop& FindStop(const std::string_view name);
		const Bus& FindBus(const std::string_view name);

		// my functions
		std::deque<Stop*> GetAllStops();
		std::deque<Bus*> GetAllBuses();

	public:
		struct PtrsHasher
		{
			size_t operator()(std::pair<Stop*, Stop*> two_stops) const {
				return std::hash<Stop*>()(two_stops.first) + 47 * std::hash<Stop*>()(two_stops.second); ;
			}
		};

	private:
		double CalcGeoDistance(const std::string_view name);
		double CalcRoadDistance(const std::string_view name);
		int CountRouteUniqueStops(const std::string_view name);

	private:
		// std::set<std::string, std::less<>> names_;

		std::deque<Stop*> stops_;
		std::unordered_map<std::string_view, Stop*, std::hash<std::string_view>> map_stops_;
		std::unordered_map<std::pair<Stop*, Stop*>, size_t, PtrsHasher> map_distances_;

		std::deque<Bus*> buses_;
		std::unordered_map<std::string_view, Bus*, std::hash<std::string_view>> map_buses_;
	};

} // namespace transport_cataloge