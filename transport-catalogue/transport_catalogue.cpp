#include "transport_catalogue.h"

using namespace transport_catalogue;

TransportCatalogue::TransportCatalogue(std::pair<std::vector<Stop>, std::vector<Bus>> stops_and_buses,
	std::unordered_map<std::string, std::unordered_map<std::string, size_t>> distances_between_stops)
{
	empty_stop_ = new Stop();
	empty_bus_ = new Bus();
	for (auto& stop : stops_and_buses.first)
	{
		AddStop(stop);
	}

	for (auto& bus : stops_and_buses.second)
	{
		AddBus(bus);
	}

	for (auto& stop : distances_between_stops)
	{
		if (map_stops_.count(stop.first))
		{
			for (auto& other_stop : stop.second)
			{
				if (map_stops_.count(other_stop.first))
				{
					AddDistances(map_stops_.at(stop.first), map_stops_.at(other_stop.first), other_stop.second);
				}
			}
		}
	}
}

TransportCatalogue::~TransportCatalogue()
{
	delete empty_stop_;
	delete empty_bus_;
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
//	auto it = names_.insert(std::string(stop.name));
//	std::string_view name_sv = *(it.first);
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

void TransportCatalogue::AddDistances(Stop* stop1, Stop* stop2, size_t distance)
{
	auto p = std::make_pair(stop1, stop2);
	map_distances_[p] = distance;
}

const std::pair<bool, Bus&> TransportCatalogue::GetBusInfo(const std::string_view name)
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
			
		return { true, *map_buses_.at(name) };
	}
	else
	{
		return { false, *empty_bus_ };
	}
}

const std::pair<bool, std::set<Bus*>> TransportCatalogue::GetStopInfo(const std::string_view name)
{
	if (map_stops_.count(name))
	{
		std::set<Bus*> result;
		for (auto& bus : buses_)
		{
			int count = std::count_if(bus->route.begin(), bus->route.end(),
				[name](const auto stop) { return stop->name == name; });
			if (count)
			{
				result.insert(bus);
			}//*/
		}
		return { true, result };
	}
	else
	{
		return { false, {} };
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

