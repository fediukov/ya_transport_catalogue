#include "json_reader.h"

void JsonReader::FillTransportCatalogue(std::istream& input, transport_catalogue::TransportCatalogue& tc)
{
	ReadJson(input);
	
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
		for (const auto& [other_stop_name, distance] : other_stops)
			tc.SetDistances(stop_name, other_stop_name, distance);
	}
}

void JsonReader::ConnectMapRenderer(MapRenderer& mr)
{
	mr.SetRenderSettings(render_settings_);
}

std::ostream& JsonReader::GetStatRequest(
	std::ostream& out,
	transport_catalogue::TransportCatalogue& tc,
	MapRenderer& mr)
{
	out << "[";
	bool first_request = true;
	for (auto& request : stat_requests_)
	{
		if (first_request)
		{
			out << "\n{\n";
			first_request = false;
		}
		else {
			out << ",\n{\n";
		}

		if (request.at("type").AsString() == "Stop" || request.at("type").AsString() == "Bus")
		{
			GetTransportCatalogueData(out, tc, request);
		}
		else if (request.at("type").AsString() == "Map")
		{
			out << '"' << "map" << '"' << ": " << '"';
			std::ostringstream o;
			mr.Draw(o, tc);
			std::string s = o.str();
			for (size_t i = 0; i < s.size(); ++i)
			{
				if (s[i] == '\"')
				{
					out << "\\\"";
				}
				else if (s[i] == '\n')
				{
					if (i != s.size() - 1)
					{
						out << "\\n";
					}
				}
				else if (s[i] == '\\')
				{
					out << '\\';
				}
				else if (s[i] == '\r')
				{
					out << "\\r";
				}
				else
				{
					out << s[i];
				}
			}
			out << '"' << ",\n"
				<< '"' << "request_id" << '"' << ": " << request.at("id").AsInt() << "\n";
		}
		out << "}";
	}
	out << "\n]\n";
	return out;
}


std::ostream& JsonReader::GetTransportCatalogueData(
	std::ostream& out,
	transport_catalogue::TransportCatalogue& tc,
	json::Dict& request)
{
	if (request.at("type").AsString() == "Stop")
	{
		std::set<domain::Bus*>* stop = tc.GetStopInfo(request.at("name").AsString());
		if (stop == nullptr)
		{
			out << '"' << "request_id" << '"' << ": " << request.at("id").AsInt() << ",\n"
				<< '"' << "error_message" << '"' << ": " << '"' << "not found" << '"' << "\n";
		}
		else
		{
			std::set<std::string> bus_names;
			out << '"' << "buses" << '"' << ": [\n";
			for (auto bus : *stop)
			{
				bus_names.insert(bus->name);
			}//*/
			bool first_bus = true;
			for (const auto& name : bus_names)
			{
				if (first_bus)
				{
					first_bus = false;
					out << '"';
				}
				else
				{
					out << ", " << '"';
				}
				out << name << '"';
			}
			out << "\n], \n"
				<< '"' << "request_id" << '"' << ": " << request.at("id").AsInt() << "\n";
		}
	}
	else if (request.at("type").AsString() == "Bus")
	{
		domain::BusStat bus_stat = tc.GetBusInfo(request.at("name").AsString());
		if (bus_stat.count_stops != -1)
		{
			out << '"' << "curvature" << '"' << ": " << bus_stat.curvature << ",\n" // << std::setprecision(6)
				<< '"' << "request_id" << '"' << ": " << request.at("id").AsInt() << ",\n"
				<< '"' << "route_length" << '"' << ": " << bus_stat.road_distance << ",\n"
				<< '"' << "stop_count" << '"' << ": " << bus_stat.count_stops << ",\n"
				<< '"' << "unique_stop_count" << '"' << ": " << bus_stat.unique_stops << "\n";
		}
		else
		{
			out << '"' << "request_id" << '"' << ": " << request.at("id").AsInt() << ",\n"
				<< '"' << "error_message" << '"' << ": " << '"' << "not found" << '"' << "\n";
		}
	}
	return out;
}

void JsonReader::ReadJson(std::istream& input)
{
	using namespace json;

	Document doc;
	doc = Load(input);
	// get types of requests and run through requests
	Dict requests = doc.GetRoot().AsMap();
	for (const auto& [key, value] : requests)
	{
		if (key == "base_requests")
		{
			Array base_requests = value.AsArray();
			ParseBaseRequests(base_requests);
		}
		else if (key == "stat_requests")
		{
			Array stat_requests = value.AsArray();
			ParseStatRequests(stat_requests);
		}
		else if (key == "render_settings")
		{
			Dict render_settings = value.AsMap();
			ParseRenderSettings(render_settings);
		}
	}
}

void JsonReader::ParseBaseRequests(json::Array& base_requests)
{
	using namespace json; 
	for (const auto& base_request : base_requests)
	{
		// run through one of request
		Dict request = base_request.AsMap();
		if (request.at("type").AsString() == "Stop")
		{
			stop_request_.push_back(ParseBaseRequestsOfStop(request));
			// run through next stops to add distances
			Dict distances = request.at("road_distances").AsMap();
			for (const auto& [next_stop, distance] : distances)
			{
				distances_between_stops_[request.at("name").AsString()][next_stop] = distance.AsDouble();
			}
		}
		else if (request.at("type").AsString() == "Bus")
		{
			bus_request_.push_back(ParseBaseRequestsOfBus(request));
		}
	}
}

domain::Bus JsonReader::ParseBaseRequestsOfBus(json::Dict& request)
{
	domain::Bus bus;
	bus.name = request.at("name").AsString();
	bus.round_route = true;
	// run through stops
	for (const auto& stop : request.at("stops").AsArray())
	{
		domain::Stop* stop_ = new domain::Stop;
		stop_->name = stop.AsString();
		bus.route.push_back(stop_);
	}

	// add stops because of not circle route
	if (request.at("is_roundtrip").AsBool() == false)
	{
		bus.round_route = false;
		int j = bus.route.size() - 1;
		while (j--)
		{
			bus.route.push_back(bus.route.at(j));
		}
	}
	return bus;
}

domain::Stop JsonReader::ParseBaseRequestsOfStop(json::Dict& request)
{
	domain::Stop stop;
	stop.name = request.at("name").AsString();
	stop.coordinates = { request.at("latitude").AsDouble(), request.at("longitude").AsDouble() };
	return stop;
}

void JsonReader::ParseStatRequests(json::Array& stat_requests)
{
	for (const auto& stat_request : stat_requests)
	{
		stat_requests_.push_back(stat_request.AsMap());
	}
}

void JsonReader::ParseRenderSettings(json::Dict& render_settings)
{
	if (render_settings.count("width"))
	{
		render_settings_.width_ = render_settings.at("width").AsDouble();
	}
	if (render_settings.count("height"))
	{
		render_settings_.height_ = render_settings.at("height").AsDouble();
	}
	if (render_settings.count("padding"))
	{
		render_settings_.padding_ = render_settings.at("padding").AsDouble();
	}
	if (render_settings.count("line_width"))
	{
		render_settings_.line_width_ = render_settings.at("line_width").AsDouble();
	}
	if (render_settings.count("stop_radius"))
	{
		render_settings_.stop_radius_ = render_settings.at("stop_radius").AsDouble();
	}
	if (render_settings.count("bus_label_font_size"))
	{
		render_settings_.bus_label_font_size_ = render_settings.at("bus_label_font_size").AsInt();
	}
	if (render_settings.count("bus_label_offset"))
	{
		json::Array values = render_settings.at("bus_label_offset").AsArray();
		ParseRenderSettingsArray(render_settings_.bus_label_offset_, values);
	}
	if (render_settings.count("stop_label_font_size"))
	{
		render_settings_.stop_label_font_size_ = render_settings.at("stop_label_font_size").AsInt();
	}
	if (render_settings.count("stop_label_offset"))
	{
		json::Array values = render_settings.at("stop_label_offset").AsArray();
		ParseRenderSettingsArray(render_settings_.stop_label_offset_, values);
	}
	if (render_settings.count("underlayer_color"))
	{
		ParseRenderSettingsColor(render_settings_.underlayer_color_, render_settings.at("underlayer_color"));
	}
	if (render_settings.count("underlayer_width"))
	{
		render_settings_.underlayer_width_ = render_settings.at("underlayer_width").AsDouble();
	}
	if (render_settings.count("color_palette"))
	{
		render_settings_.color_palette_.clear();
		json::Array values = render_settings.at("color_palette").AsArray();
		for (size_t i = 0; i < values.size(); ++i)
		{
			svg::Color setting;
			render_settings_.color_palette_.push_back(ParseRenderSettingsColor(setting, values.at(i)));
		}
	}
}

std::vector<double>& JsonReader::ParseRenderSettingsArray(std::vector<double>& setting, json::Array& array)
{
	setting.clear();
	for (size_t i = 0; i < array.size(); ++i)
	{
		setting.push_back(array.at(i).AsDouble());
	}
	return setting;
}

svg::Color& JsonReader::ParseRenderSettingsColor(svg::Color& setting, json::Node& array)
{
	if (array.IsString())
	{
		setting = array.AsString();
	}
	else if (array.IsArray() && array.AsArray().size() == 3)
	{
		setting = svg::Rgb{
			static_cast<uint8_t>(array.AsArray().at(0).AsInt()),
			static_cast<uint8_t>(array.AsArray().at(1).AsInt()),
			static_cast<uint8_t>(array.AsArray().at(2).AsInt())
		};
	}
	else if (array.IsArray() && array.AsArray().size() == 4)
	{
		setting = svg::Rgba{
			static_cast<uint8_t>(array.AsArray().at(0).AsInt()),
			static_cast<uint8_t>(array.AsArray().at(1).AsInt()),
			static_cast<uint8_t>(array.AsArray().at(2).AsInt()),
			array.AsArray().at(3).AsDouble()
		};
	}
	return setting;
}

