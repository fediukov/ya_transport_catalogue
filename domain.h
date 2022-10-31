#pragma once

#include <string>
#include <vector>

#include "geo.h"

namespace domain {

	struct Stop
	{
		std::string name;
		geo::Coordinates coordinates;
	};

	struct Bus
	{
		Bus() = default;

		std::string name;
		std::vector<Stop*> route;
		bool round_route;
	};

	struct BusStat
	{
		int count_stops = -1;
		int unique_stops = -1;
		double geo_distance = -1.;
		double road_distance = -1.;
		double curvature = -1.;
	};

} // namespace domain