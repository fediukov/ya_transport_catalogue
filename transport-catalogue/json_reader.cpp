#include "json_reader.h"


using namespace std::string_literals;

void JsonReader::FillTransportCatalogue(std::istream& input, transport_catalogue::TransportCatalogue& tc)
{
	// read input
	ReadJson(input);

	// fill transport_catalogue
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

	// create transport_router
	tr_(bus_request_, stop_request_, distances_between_stops_, routing_settings_);
}

void JsonReader::ConnectMapRenderer(MapRenderer& mr)
{
	mr.SetRenderSettings(render_settings_);
}

void JsonReader::GetStatRequest(
	std::ostream& out,
	transport_catalogue::TransportCatalogue& tc,
	MapRenderer& mr)
{
	json::Array answers;
	for (auto& request : stat_requests_)
	{
		if (request.at("type").AsString() == "Stop")
		{
			answers.push_back(GetTransportCatalogeStopNode(tc, request));
		}
		else if (request.at("type").AsString() == "Bus")
		{
			answers.push_back(GetTransportCatalogeBusNode(tc, request));
		}
		else if (request.at("type").AsString() == "Map")
		{
			answers.push_back(GetJsonMapNode(tc, mr, request));
		}
		else if (request.at("type").AsString() == "Route")
		{
			answers.push_back(GetTransportRouterNode(/*tc,*/ request));
		}
	}

	json::Print(json::Document{ json::Builder{}.Value(answers).Build() }, out);
}

json::Dict JsonReader::GetTransportCatalogeBusNode(
	transport_catalogue::TransportCatalogue& tc,
	json::Dict& request)
{
	json::Dict result;

	domain::BusStat bus_stat = tc.GetBusInfo(request.at("name").AsString());
	if (bus_stat.count_stops != -1)
	{
		result.insert({ "curvature", bus_stat.curvature });
		result.insert({ "request_id", request.at("id").AsInt() });
		result.insert({ "route_length", bus_stat.road_distance });
		result.insert({ "stop_count", bus_stat.count_stops });
		result.insert({ "unique_stop_count", bus_stat.unique_stops });
	}
	else
	{
		result.insert({ "request_id", request.at("id").AsInt() });
		result.insert({ "error_message", "not found"s });
	}

	return result;
}

json::Dict JsonReader::GetTransportCatalogeStopNode(
	transport_catalogue::TransportCatalogue& tc,
	json::Dict& request)
{
	json::Dict result;

	std::set<domain::Bus*>* stop = tc.GetStopInfo(request.at("name").AsString());
	if (stop == nullptr)
	{
		result.insert({ "request_id", request.at("id").AsInt() });
		result.insert({ "error_message", "not found"s });
	}
	else
	{
		// sorted buses
		std::set<std::string> bus_names;
		for (auto bus : *stop)
		{
			bus_names.insert(bus->name);
		}

		json::Array buses;
		for (auto& bus_name : bus_names)
		{
			buses.push_back(bus_name);
		}

		result.insert({ "buses", buses });
		result.insert({ "request_id", request.at("id").AsInt() });
	}

	return result;
}

json::Dict JsonReader::GetJsonMapNode(
	transport_catalogue::TransportCatalogue& tc,
	MapRenderer& mr,
	json::Dict& request)
{
	json::Dict result;

	std::ostringstream o;
	mr.Draw(o, tc);
	std::string map = o.str();
	if (map.at(map.size() - 1) == '\n')
	{
		map.resize(map.size() - 1);
	}

	result.insert({ "map" , map });
	result.insert({ "request_id", request.at("id").AsInt() });

	return result;
}

json::Dict JsonReader::GetTransportRouterNode(
	//transport_catalogue::TransportCatalogue& tc,
	json::Dict& request)
{
	json::Dict result;

	auto router_info = tr_.GetRoute(request.at("from").AsString(), request.at("to").AsString());

	if (!router_info.size())
	{
		json::Array items;
		result.insert({ "request_id", request.at("id").AsInt() });
		result.insert({ "total_time", 0 });
		result.insert({ "items", items });
	}
	else if (router_info.at(0).type_ == "Not Found")
	{
		result.insert({ "request_id", request.at("id").AsInt() });
		result.insert({ "error_message", "not found"s });
	}
	else
	{
		double total_time = 0.;
		json::Array items;
		for (const auto& info : router_info)
		{
			json::Dict item;
			if (info.type_ == "Wait")
			{
				item.insert({ "type", "Wait"s });
				item.insert({ "stop_name", info.stop_name_ });
				item.insert({ "time", info.time_ });
			}
			else if (info.type_ == "Bus")
			{
				item.insert({ "type", "Bus"s });
				item.insert({ "bus", info.bus_name_ });
				item.insert({ "span_count", info.stop_count_ });
				item.insert({ "time", info.time_ });
			}

			total_time += info.time_;
			items.push_back(item);
		}

		result.insert({ "request_id", request.at("id").AsInt() });
		result.insert({ "total_time", total_time });
		result.insert({ "items", items });

	}

	return result;
}


void JsonReader::ReadJson(std::istream& input)
{
	using namespace json;

	Document doc(Load(input));
	//doc = Load(input);
	// get types of requests and run through requests
	Dict requests = doc.GetRoot().AsDict();
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
			Dict render_settings = value.AsDict();
			ParseRenderSettings(render_settings);
		}
		else if (key == "routing_settings")
		{
			Dict routing_settings = value.AsDict();
			ParseRoutingSettings(routing_settings);
		}
	}
}

void JsonReader::ParseBaseRequests(json::Array& base_requests)
{
	using namespace json;
	for (const auto& base_request : base_requests)
	{
		// run through one of request
		Dict request = base_request.AsDict();
		if (request.at("type").AsString() == "Stop")
		{
			stop_request_.push_back(ParseBaseRequestsOfStop(request));
			// run through next stops to add distances
			Dict distances = request.at("road_distances").AsDict();
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
		stat_requests_.push_back(stat_request.AsDict());
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

void JsonReader::ParseRoutingSettings(json::Dict& routing_settings)
{
	if (routing_settings.count("bus_wait_time"))
	{
		routing_settings_.bus_wait_time_ = routing_settings.at("bus_wait_time").AsInt();
	}
	if (routing_settings.count("bus_velocity"))
	{
		routing_settings_.bus_velocity_ = routing_settings.at("bus_velocity").AsDouble();
	}
}

