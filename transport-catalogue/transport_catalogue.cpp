#include "transport_catalogue.h"

using namespace transport_catalogue;

TransportCatalogue::~TransportCatalogue()
{
	while (!stops_.empty())
	{
		delete stops_.back();
		stops_.pop_back();
	}
	while (!buses_.empty())
	{
		delete buses_.back();
		buses_.pop_back();
	}
}

void TransportCatalogue::AddStop(const Stop& stop)
{
	Stop* stop_tc = new Stop(stop);
	stops_.push_back(stop_tc);
	map_stops_[(*stop_tc).name] = stop_tc;
}

void TransportCatalogue::AddBus(const Bus& bus)
{
	Bus* bus_tc = new Bus(bus);
	for (auto& stop : bus_tc->route)
	{
		if (map_stops_.find((*stop).name) != map_stops_.end())
		{
			stop = map_stops_.at((*stop).name);
		}
		else
		{
			std::cout << "The Stop " << (*stop).name << " of " << bus_tc->name << " is not found!" << std::endl;
			assert(false);
		}//*/
	}
	buses_.push_back(bus_tc);
	map_buses_[(*bus_tc).name] = bus_tc;
}

void TransportCatalogue::SetDistances(std::string_view from, std::string_view to, size_t distance)
{
	auto p = std::make_pair(map_stops_.at(from), map_stops_.at(to));
	map_distances_[p] = distance;
}

/*Bus* TransportCatalogue::GetBusInfo(const std::string_view name)
{
	if (map_buses_.count(name))
	{
		if (map_buses_.at(name)->geo_distance == -1)
		{
			map_buses_.at(name)->geo_distance = CalcGeoDistance(name);
		}
		if (map_buses_.at(name)->unique_stops == -1)
		{
			map_buses_.at(name)->unique_stops = CountRouteUniqueStops(name);
		}
		if (map_buses_.at(name)->road_distance == -1)
		{
			map_buses_.at(name)->road_distance = CalcRoadDistance(name);
		}
		map_buses_.at(name)->curvature = map_buses_.at(name)->curvature == -1
			? map_buses_.at(name)->road_distance / map_buses_.at(name)->geo_distance
			: map_buses_.at(name)->curvature;
			
		return map_buses_.at(name);
	}
	else
	{
		return nullptr;
	}
}//*/

const BusStat TransportCatalogue::GetBusInfo(const std::string_view name)
{
	BusStat bus_stat;
	if (map_buses_.count(name))
	{
		bus_stat.count_stops = (*map_buses_.at(name)).route.size();
		bus_stat.unique_stops = CountRouteUniqueStops(name);
		bus_stat.geo_distance = CalcGeoDistance(name);
		bus_stat.road_distance = CalcRoadDistance(name);
		bus_stat.curvature = bus_stat.road_distance / bus_stat.geo_distance;
	}
	return bus_stat;
}

std::set<Bus*>* TransportCatalogue::GetStopInfo(const std::string_view name)
{
	if (map_stops_.count(name))
	{
		std::set<Bus*>* result = new std::set<Bus*>({});
		for (auto& bus : buses_)
		{
			int count = std::count_if(bus->route.begin(), bus->route.end(),
				[name](const auto stop) { return stop->name == name; });
			if (count)
			{
				(*result).insert(bus);
			}//*/
		}
		return result;
	}
	else
	{
		return nullptr;
	}
}

const Stop& TransportCatalogue::FindStop(const std::string_view name)
{
	return *map_stops_.at(name);
}

const Bus& TransportCatalogue::FindBus(const std::string_view name)
{
	return *map_buses_.at(name);
}

std::deque<Stop*> TransportCatalogue::GetAllStops()
{
	return stops_;
}

std::deque<Bus*> TransportCatalogue::GetAllBuses()
{
	return buses_;
}

double TransportCatalogue::CalcGeoDistance(const std::string_view name)
{
	double distance = 0.;
	for (int i = 1; i < (*map_buses_.at(name)).route.size(); ++i)
	{
		Coordinates from = (*map_buses_.at(name)).route[i - 1]->coordinates,
			to = (*map_buses_.at(name)).route[i]->coordinates;
		distance += ComputeDistance(from, to);
	}
	return distance;
}

double TransportCatalogue::CalcRoadDistance(const std::string_view name)
{
	double distance = 0.;
	for (int i = 1; i < map_buses_.at(name)->route.size(); ++i)
	{
		auto ptr_stop = map_buses_.at(name)->route.at(i - 1);
		auto ptr_next_stop = map_buses_.at(name)->route.at(i);
		auto p = std::make_pair(ptr_stop, ptr_next_stop);
		if (map_distances_.count(p))
		{
			distance += map_distances_.at(p) * 1.;
		}
		else
		{
			p = std::make_pair(ptr_next_stop, ptr_stop);
			distance += map_distances_.at(p) * 1.;
		}
	}
	return distance;
}

int TransportCatalogue::CountRouteUniqueStops(const std::string_view name)
{
	std::set<Stop*> unique_stops;
	for (const auto& stop : (*map_buses_.at(name)).route)
	{
		unique_stops.insert(stop);
	}
	return unique_stops.size();
}
