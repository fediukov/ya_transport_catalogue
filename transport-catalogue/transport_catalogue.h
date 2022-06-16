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
#include "domain.h"

namespace transport_catalogue {

	class TransportCatalogue {
	public:
		
		TransportCatalogue() = default;
		~TransportCatalogue();

		// create db
		void AddStop(const domain::Stop& stop);
		void AddBus(const domain::Bus& bus);
		void SetDistances(std::string_view from, std::string_view to, size_t distance);

		// get db parts
		//Bus* GetBusInfo(const std::string_view name);
		const domain::BusStat GetBusInfo(const std::string_view name);
		std::set<domain::Bus*>* GetStopInfo(const std::string_view name);
		const domain::Stop& FindStop(const std::string_view name);
		const domain::Bus& FindBus(const std::string_view name);

		// my functions
		std::deque<domain::Stop*> GetAllStops();
		std::deque<domain::Bus*> GetAllBuses();

	public:
		struct PtrsHasher
		{
			size_t operator()(std::pair<domain::Stop*, domain::Stop*> two_stops) const {
				return std::hash<domain::Stop*>()(two_stops.first)
					+ 47 * std::hash<domain::Stop*>()(two_stops.second); ;
			}
		};

	private:
		double CalcGeoDistance(const std::string_view name);
		double CalcRoadDistance(const std::string_view name);
		int CountRouteUniqueStops(const std::string_view name);

	private:
		// std::set<std::string, std::less<>> names_;

		std::deque<domain::Stop*> stops_;
		std::unordered_map<std::string_view, domain::Stop*, std::hash<std::string_view>> map_stops_;
		std::unordered_map<std::pair<domain::Stop*, domain::Stop*>, size_t, PtrsHasher> map_distances_;

		std::deque<domain::Bus*> buses_;
		std::unordered_map<std::string_view, domain::Bus*, std::hash<std::string_view>> map_buses_;
	};

} // namespace transport_cataloge