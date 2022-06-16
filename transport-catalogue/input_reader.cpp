#include "input_reader.h"

using namespace transport_catalogue;

void InputReader::FillTransportCatalogue(transport_catalogue::TransportCatalogue& tc)
{
	CinRequest();

	for (const auto& stop : stop_request_)
	{
		tc.AddStop(stop);
	}

	for (const auto& bus : bus_request_)
	{
		tc.AddBus(bus);
	}

	for (const auto& [stop_name, other_stops] : distances_between_stops_)
	{
		for(const auto& [other_stop_name, distance] : other_stops)
		tc.SetDistances(stop_name, other_stop_name, distance);
	}
}

void InputReader::CinRequest()
{
	int data_count;
	std::string s;
	std::cin >> data_count;
	for (int i = 0; i < data_count + 1; ++i)
	{
		std::getline(std::cin, s);
		SplitDataRequest(s);
	}
}

void InputReader::SplitDataRequest(std::string& st)
{
	// if string is bus
	if (st[0] == 'B' && st[1] == 'u' && st[2] == 's' && st[3] == ' ')
	{
		st = st.substr(4);
		bus_request_.push_back(IsBusOfDataRequest(st));
	}

	// if string is stop
	if (st[0] == 'S' && st[1] == 't' && st[2] == 'o' && st[3] == 'p' && st[4] == ' ')
	{
		st = st.substr(5);
		domain::Stop stop = IsStopOfDataRequest(st);
		stop_request_.push_back(stop);

		// get distances to neighboring stops if they exist
		while (!st.empty())
		{
			st = st.substr(1);
			auto [other_stop_name, distance] = move(AreDistance(st));
			distances_between_stops_[stop.name][other_stop_name] = distance;
		}//*/
	}
}

domain::Bus InputReader::IsBusOfDataRequest(std::string& st)
{
	domain::Bus bus;

	// get name of bus
	bus.name = move(GetNameOfRequest(st, ':'));
	st = st.substr(1);

	// check circle route
	char type_route;
	if (std::find(st.begin(), st.end(), '>') != st.end())
	{
		type_route = '>'; // circle route
	}
	else if (std::find(st.begin(), st.end(), '-') != st.end())
	{
		type_route = '-'; // not circle route
	}
	else
	{
		std::cout << "Definition of route: something went wrong" << std::endl;
		assert(false);
	}
	
	// add stops
	while (!st.empty())
	{
		domain::Stop* stop = new domain::Stop;
		stop->name = move(GetNameOfRequest(st, type_route));
		if (!st.empty())
		{
			st = st.substr(1);
		}
		bus.route.push_back(stop);
	}

	// add stops because of not circle route
	if (type_route == '-')
	{
		int j = bus.route.size() - 1;
		while (j--)
		{
			bus.route.push_back(bus.route.at(j));
		}
	}
	
	return bus;
}

domain::Stop InputReader::IsStopOfDataRequest(std::string& st)
{
	domain::Stop stop;

	// get name of bus
	stop.name = move(GetNameOfRequest(st, ':'));
	st = st.substr(1);

	// get coordinates of stop
	stop.coordinates.lat = std::stod(move(GetNameOfRequest(st, ',')));
	st = st.substr(1);
	stop.coordinates.lng = std::stod(move(GetNameOfRequest(st, ',')));

	return stop;
}

std::pair<std::string, size_t> InputReader::AreDistance(std::string& st)
{
	// get distance
	size_t distance = static_cast<size_t>(std::stoi(move(GetNameOfRequest(st, 'm'))));
	if (st[0] == 'm' && st[1] == ' ' && st[2] == 't' && st[3] == 'o' && st[4] == ' ')
	{
		st = st.substr(5);
	}
	else
	{
		std::cout << "Distances: Something went wrong" << std::endl;
		assert(false);
	}
	
	// get name of other stop
	std::string other_stop = move(GetNameOfRequest(st, ','));
	if (!st.empty())
	{
		st = st.substr(1);
	}

	return { other_stop, distance };
}

std::string InputReader::GetNameOfRequest(std::string& st, const char end_of_name)
{
	int i = 0;
	std::string name = "";
	while (st[i] != end_of_name && i < st.size())
	{
		name += st[i++];
	}
	st = st.substr(i);
	EraseSpaces(name);
	return name;
}

void InputReader::EraseSpaces(std::string& st)
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