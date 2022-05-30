#include "stat_reader.h"

using namespace transport_catalogue;

void StatReader::PrintBusInfo(TransportCatalogue& tc, const std::string_view name)
{
	std::pair<bool, Bus&> bus = tc.GetBusInfo(name);
	if (bus.first)
	{
		std::cout << "Bus " << bus.second.name << ": " << std::setprecision(6)
			<< bus.second.route.size() << " stops on route, "
			<< bus.second.unique_stops << " unique stops, "
			<< bus.second.road_distance << " route length, "
			<< bus.second.curvature << " curvature" << std::endl;
	}
	else
	{
		std::cout << "Bus " << name << ": not found" << std::endl;
	}
}

void StatReader::PrintStopInfo(TransportCatalogue& tc, const std::string_view name)
{
	std::pair<bool, std::set<Bus*>> stop = tc.GetStopInfo(name);
	if (stop.first)
	{
		std::cout << "Stop " << name << ": " << std::setprecision(6);
		if (!stop.second.size())
		{
			std::cout << "no buses";
		}
		else
		{
			std::set<std::string> bus_names;
			std::cout << "buses";
			for (auto bus : stop.second)
			{
				bus_names.insert(bus->name);
			}//*/
			for (const auto& name : bus_names)
			{
				std::cout << " " << name;
			}
		}
		std::cout << std::endl;
	}
	else
	{
		std::cout << "Stop " << name << ": not found" << std::endl;
	}
}

void StatReader::GetAnswers(TransportCatalogue& tc, InputReader& input_reader)
{
	for (const auto& request : input_reader.CreateAnswer())
	{
		if (request.first == "bus")
		{
			PrintBusInfo(tc, request.second);
		}
		if (request.first == "stop")
		{
			PrintStopInfo(tc, request.second);
		}

	}
}

void StatReader::PrintAllStopsAndBuses(TransportCatalogue& tc)
{
    std::cout << "Stops" << std::endl;
    for (const auto& s : tc.GetAllStops())
    {
        std::cout << (*s).name << ": " << std::setprecision(15) << (*s).coordinates.lat << " " << (*s).coordinates.lng << std::endl;
    }

    std::cout << "Buses" << std::endl;
    for (const auto& bus : tc.GetAllBuses())
    {
        std::cout << (*bus).name << ":";
        for (const auto& stop : (*bus).route)
        {
            std::cout << (*stop).name << "-";
        }
        std::cout << std::endl;
    }
}