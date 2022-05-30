#include "input_reader.h"

using namespace transport_catalogue;

InputReader::InputReader(std::vector<std::string>& datas, std::vector<std::string>& answers)
{
	SplitRequest(datas, answers);
}

std::pair<std::vector<Stop>, std::vector<Bus>> InputReader::CreateData()
{
	return {stop_request_, bus_request_};
}

std::unordered_map<std::string, std::unordered_map<std::string, size_t>> InputReader::CreateDistances()
{
	return distances_between_stops_;
}

std::vector<std::pair<std::string, std::string>> InputReader::CreateAnswer()
{
	return answer_request_;
}

void InputReader::SplitRequest(std::vector<std::string>& datas, std::vector<std::string>& answers)
{
	using namespace std;

	for (auto& s : datas)
	{
		SplitDataRequest(s);
	}

	for (auto& s : answers)
	{
		SplitAnswerRequest(s);
	}
}

void InputReader::SplitDataRequest(std::string& st)
{
	
	// if string is bus
	if (st[0] == 'B' && st[1] == 'u' && st[2] == 's' && st[3] == ' ')
	{
		st = st.substr(4);
		Bus bus;
		
		// get name of bus
		int i = 0;
		std::string new_name = "";
		while (st[i] != ':')
		{
			new_name += st[i++];
		}
		EraseSpaces(new_name);
		bus.name = move(new_name);

		// get bus stops
		i += 2;
		bool circle_route = false;
		while (i < st.size())
		{
			// add new stop
			new_name = ""; 
			while (st[i] != '-' && st[i] != '>' && i < st.size())
			{
				new_name += st[i++];
				if (st[i] == '>')
				{
					circle_route = true;
				}
			}
			EraseSpaces(new_name);
			Stop* stop = new Stop;
			stop->name = new_name;
			bus.route.push_back(stop);

			i += 2;
		}//*/
		if (!circle_route)
		{
			int j = bus.route.size() - 1;
			while (j--)
			{
				bus.route.push_back(bus.route.at(j));
			}
		}
		bus_request_.push_back(bus);
	}

	// if string is stop
	if (st[0] == 'S' && st[1] == 't' && st[2] == 'o' && st[3] == 'p' && st[4] == ' ')
	{
		st = st.substr(5);
		Stop stop;

		// get name of stop
		int i = 0;
		std::string new_name = "";
		while (st[i] != ':')
		{
			new_name += st[i++];
		}
		EraseSpaces(new_name);
		stop.name = move(new_name);
		
		// get lat-coordinate of stop
		new_name = "";
		i += 2;
		while (st[i] != ',')
		{
			new_name += st[i++];
		}
		if (new_name.size() > 3)
		{
			new_name += '0';
		}
		EraseSpaces(new_name); 
		stop.coordinates.lat = std::stod(move(new_name));

		// get lng-coordinate of stop
		new_name = "";
		i += 2;
		while (st[i] != ',' && i < st.size())
		{
			new_name += st[i++];
		}
		if (new_name.size() > 3)
		{
			new_name += '0';
		}
		EraseSpaces(new_name); 
		stop.coordinates.lng = std::stod(move(new_name));//*/

		stop_request_.push_back(stop);

		// get distances to stops
		i += 2;
		while (i < st.size())
		{
			size_t distance = 0;
			new_name = "";
			std::string other_stop = "";
			while (st[i] != 'm')
			{
				new_name += st[i++];
			}
			EraseSpaces(new_name);
			distance = static_cast<size_t>(std::stoi(move(new_name)));
			if (i + 4 >= st.size() || (st[i + 1] != ' ' && st[i + 2] != 't' && st[i + 3] != 'o' && st[i + 4] != ' '))
			{
				std::cout << "Distances: Something went wrong" << std::endl;
			}
			else
			{
				new_name = "";
				i += 5;
				while (st[i] != ',' && i < st.size())
				{
					new_name += st[i++];
				}
				EraseSpaces(new_name);
				other_stop = move(new_name);
			}
			distances_between_stops_[stop.name][other_stop] = distance;
			i += 2;
		}
	}
}

void InputReader::SplitAnswerRequest(std::string& st)
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