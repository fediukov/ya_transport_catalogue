#include "stat_reader.h"

using namespace transport_catalogue;

void StatReader::GetAnswers(TransportCatalogue& tc)
{
	CinRequest();

	for (const auto& [type, name] : answer_request_)
	{
		if (type == "bus")
		{
			PrintBusInfo(tc, name);
		}
		if (type == "stop")
		{
			PrintStopInfo(tc, name);
		}
	}
}

void StatReader::PrintBusInfo(TransportCatalogue& tc, const std::string_view name)
{
	/*Bus* bus = tc.GetBusInfo(name);
	if (bus != nullptr)
	{
		std::cout << "Bus " << bus->name << ": " << std::setprecision(6)
			<< bus->route.size() << " stops on route, "
			<< bus->unique_stops << " unique stops, "
			<< bus->road_distance << " route length, "
			<< bus->curvature << " curvature" << std::endl;
	}
	else
	{
		std::cout << "Bus " << name << ": not found" << std::endl;
	}//*/
	BusStat bus_stat = tc.GetBusInfo(name);
	if (bus_stat.count_stops != -1)
	{
		std::cout << "Bus " << name << ": " << std::setprecision(6)
			<< bus_stat.count_stops << " stops on route, "
			<< bus_stat.unique_stops << " unique stops, "
			<< bus_stat.road_distance << " route length, "
			<< bus_stat.curvature << " curvature" << std::endl;
	}
	else
	{
		std::cout << "Bus " << name << ": not found" << std::endl;
	}
}

void StatReader::PrintStopInfo(TransportCatalogue& tc, const std::string_view name)
{
	std::set<Bus*>* stop = tc.GetStopInfo(name);
	if (stop != nullptr)
	{
		std::cout << "Stop " << name << ": " << std::setprecision(6);
		if (!(*stop).size())
		{
			std::cout << "no buses";
		}
		else
		{
			std::set<std::string> bus_names;
			std::cout << "buses";
			for (auto bus : *stop)
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

void StatReader::CinRequest()
{
	int data_count;
	std::string s;
	std::cin >> data_count;
	for (int i = 0; i < data_count + 1; ++i)
	{
		std::getline(std::cin, s);
		SplitAnswerRequest(s);
	}
}

void StatReader::SplitAnswerRequest(std::string& st)
{
	if (st[0] == 'B' && st[1] == 'u' && st[2] == 's' && st[3] == ' ')
	{
		st = st.substr(4);
		EraseSpaces(st);
		answer_request_.push_back({ "bus", st });
	}
	if (st[0] == 'S' && st[1] == 't' && st[2] == 'o' && st[3] == 'p' && st[4] == ' ')
	{
		st = st.substr(5);
		EraseSpaces(st);
		answer_request_.push_back({ "stop", st });
	}
}

void StatReader::EraseSpaces(std::string& st)
{
	int i = 0;
	while (st[i] == ' ')
	{
		i++;
	}
	st = st.substr(i);
	int j = 0;
	while (st[st.size() - 1 - j] == ' ')
	{
		j++;
	}
	st.resize(st.size() - j);
	// space inside
	/*auto it = find(st.begin(), st.end(), ' ');
	if (it != st.end())
	{
		it++;
		while (*it == ' ')
		{
			st.erase(it);
		}
	}//*/
}


