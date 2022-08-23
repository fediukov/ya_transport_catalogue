#include "serialization.h"

void serializer::Serialize(TransportCatalogue& tc, MapRenderer& mr, TransportRouter& tr, Path& file)
{
	std::ofstream out(file, std::ios::binary);
	transport_catalogue_serialize::TransportCatalogue data_tc;

	// serialize stops
	for (const auto& stop : tc.GetAllStops())
	{
		transport_catalogue_serialize::Stop* data_stop;
		data_stop = data_tc.add_stops();
		data_stop->set_name(stop->name);
		data_stop->set_lat(stop->coordinates.lat);
		data_stop->set_lng(stop->coordinates.lng);
	}

	// serialize buses
	for (const auto& bus : tc.GetAllBuses())
	{
		transport_catalogue_serialize::Bus* data_bus;
		data_bus = data_tc.add_buses();
		data_bus->set_name(bus->name);
		data_bus->set_is_round(bus->round_route);
		int i = 0;
		for (const auto& stop : bus->route)
		{
			std::string stop_name = stop->name;
			data_bus->add_stop(std::move(stop_name));
			//std::cout << data_bus->stop(i++) << "." << std::endl;
		}
	}

	// serialize distances between stops
	for (const auto& [stop_from, stop_to_distance] : tc.GetAllDistances())
	{
		for (const auto& [stop_to, distance] : stop_to_distance)
		{
			transport_catalogue_serialize::Distance* data_distance;
			data_distance = data_tc.add_distances();
			data_distance->set_stop_from(stop_from.data());
			data_distance->set_stop_to(stop_to.data());
			data_distance->set_distance(distance);
		}
	}//*/

	// serialize render_settings
	transport_catalogue_serialize::RendererSettings data_rs;
	RenderSettings rs = mr.GetRenderSettings();
	data_rs.set_width(rs.width_);
	data_rs.set_height(rs.height_);
	data_rs.set_padding(rs.padding_);
	data_rs.set_line_width(rs.line_width_);
	data_rs.set_stop_radius(rs.stop_radius_);
	data_rs.set_underlayer_width(rs.underlayer_width_);
	data_rs.set_bus_label_font_size(rs.bus_label_font_size_);
	data_rs.set_stop_label_font_size(rs.stop_label_font_size_);
	for (const auto& offset : rs.bus_label_offset_)
	{
		data_rs.add_bus_label_offset(offset);
	}
	for (const auto& offset : rs.stop_label_offset_)
	{
		data_rs.add_stop_label_offset(offset);
	}
	*data_rs.mutable_underlayer_color() = SerializeColor(rs.underlayer_color_);
	for (const auto& color : rs.color_palette_)
	{
		*data_rs.add_color_palette() = SerializeColor(color);
	}

	*data_tc.mutable_render_settings() = data_rs;

	// serialize transport_router
	transport_catalogue_serialize::TransportRouter s_tr;

	transport_catalogue_serialize::RoutingSettings s_ros;
	RoutingSettings routing_settings = tr.GetRoutingSettings();
	s_ros.set_bus_wait_time(routing_settings.bus_wait_time_);
	s_ros.set_bus_velocity(routing_settings.bus_velocity_);

	*s_tr.mutable_routing_settings() = s_ros;

	for (size_t i = 0; i < tr.GetGraph().GetEdgeCount(); ++i)
	{
		graph::Edge<double> tr_edge = tr.GetGraph().GetEdge(i);
		transport_catalogue_serialize::Edge* s_edge;
		s_edge = s_tr.add_edges();
		s_edge->set_from(tr_edge.from);
		s_edge->set_to(tr_edge.to);
		s_edge->set_weight(tr_edge.weight);
	}
	for (const auto [name, id] : tr.GetNameIdOfStops())
	{
		transport_catalogue_serialize::StopInfo* s_stop_info;
		s_stop_info = s_tr.add_stop_info();
		s_stop_info->set_stop_name(name.data());
		s_stop_info->set_edge_id(id);
	}
	for (const auto [id, name] : tr.GetIdNameOfBuses())
	{
		transport_catalogue_serialize::BusInfo* s_bus_info;
		s_bus_info = s_tr.add_bus_info();
		s_bus_info->set_bus_name(name.data());
		s_bus_info->set_edge_id(id);
	}
	for (const auto [id, count] : tr.GetIdCountOfStops())
	{
		transport_catalogue_serialize::CountInfo* s_count_info;
		s_count_info = s_tr.add_count_info();
		s_count_info->set_stop_count(count);
		s_count_info->set_edge_id(id);
	}

	*data_tc.mutable_transport_router() = s_tr;

	// save result
	data_tc.SerializeToOstream(&out);
}

void serializer::Deserialize(Path& file, TransportCatalogue& tc, MapRenderer& mr, TransportRouter& tr)
{
	std::ifstream ifs(file, std::ios::binary);
	transport_catalogue_serialize::TransportCatalogue data;
	if (data.ParseFromIstream(&ifs))
	{
		// deserialize stops
		for (int i = 0; i < data.stops_size(); ++i)
		{
			domain::Stop stop;
			transport_catalogue_serialize::Stop s = data.stops(i);
			stop.name = s.name();
			stop.coordinates.lat = s.lat();
			stop.coordinates.lng = s.lng();

			tc.AddStop(stop);
		}

		// deserialize buses
		for (int i = 0; i < data.buses_size(); ++i)
		{
			domain::Bus bus;
			transport_catalogue_serialize::Bus b = data.buses(i);
			bus.name = b.name();
			bus.round_route = b.is_round();
			bus.route.resize(b.stop_size());
			for (int j = 0; j < b.stop_size(); ++j)
			{
				std::string s = b.stop(j);
				auto adress_of_stop = tc.FindStopPtr(s);
				bus.route[j] = const_cast<domain::Stop*>(adress_of_stop);
			}

			tc.AddBus(bus);
		}

		// deserialize distances between stops
		for (int i = 0; i < data.distances_size(); ++i)
		{
			domain::Bus bus;
			transport_catalogue_serialize::Distance d = data.distances(i);
			
			tc.SetDistances(d.stop_from(), d.stop_to(), d.distance());
		}

		// deserialize render_settings
		transport_catalogue_serialize::RendererSettings rs = data.render_settings();
		RenderSettings render_settings;
		render_settings.width_ = rs.width();
		render_settings.height_ = rs.height();
		render_settings.padding_ = rs.padding();
		render_settings.line_width_ = rs.line_width();
		render_settings.stop_radius_ = rs.stop_radius();
		render_settings.underlayer_width_ = rs.underlayer_width();
		render_settings.bus_label_font_size_ = rs.bus_label_font_size();
		render_settings.stop_label_font_size_ = rs.stop_label_font_size();
		render_settings.bus_label_offset_.clear();
		for (size_t i = 0; i < rs.bus_label_offset_size(); ++i)
		{
			render_settings.bus_label_offset_.push_back(rs.bus_label_offset(i));
		}
		render_settings.stop_label_offset_.clear();
		for (size_t i = 0; i < rs.stop_label_offset_size(); ++i)
		{
			render_settings.stop_label_offset_.push_back(rs.stop_label_offset(i));
		}
		render_settings.underlayer_color_ = DeserializeColor(rs.underlayer_color());
		render_settings.color_palette_.clear();
		for (int i = 0; i < rs.color_palette_size(); ++i)
		{
			render_settings.color_palette_.push_back(DeserializeColor(rs.color_palette(i)));
		}
		mr.SetRenderSettings(render_settings);

		// deserialize routing settings
		transport_catalogue_serialize::TransportRouter s_tr = data.transport_router();

		RoutingSettings routing_settings;
		routing_settings.bus_wait_time_ = s_tr.routing_settings().bus_wait_time();
		routing_settings.bus_velocity_ = s_tr.routing_settings().bus_velocity();
		tr.SetRoutingSettings(routing_settings);

		for (size_t i = 0; i < s_tr.stop_info_size(); ++i)
		{
			transport_catalogue_serialize::StopInfo stop_info = s_tr.stop_info(i);
			std::string s = stop_info.stop_name();
			tr.SetNameIdOfStop(s, stop_info.edge_id());
		}
		for (size_t i = 0; i < s_tr.bus_info_size(); ++i)
		{
			transport_catalogue_serialize::BusInfo bus_info = s_tr.bus_info(i);
			tr.SetIdNameOfBus(bus_info.edge_id(), bus_info.bus_name());
		}
		for (size_t i = 0; i < s_tr.count_info_size(); ++i)
		{
			transport_catalogue_serialize::CountInfo count_info = s_tr.count_info(i);
			tr.SetIdCountOfStop(count_info.edge_id(), count_info.stop_count());
		}
		tr.GetGraph().SetVertexCount(s_tr.edges_size());
		for (size_t i = 0; i < s_tr.edges_size(); ++i)
		{
			transport_catalogue_serialize::Edge s_edge = s_tr.edges(i);
			graph::Edge<double> tr_edge;
			tr_edge.from = s_edge.from();
			tr_edge.to = s_edge.to();
			tr_edge.weight = s_edge.weight();
			tr.GetGraph().AddEdge(tr_edge);
		}
		tr.SetRouter(tr.GetGraph());
	}
}

transport_catalogue_serialize::RGB serializer::SerializeColorRGB(const svg::Rgb& color)
{
	transport_catalogue_serialize::RGB rgb;
	rgb.set_red(color.red);
	rgb.set_green(color.green);
	rgb.set_blue(color.blue);
	return rgb;
}

transport_catalogue_serialize::RGBA serializer::SerializeColorRGBA(const svg::Rgba& color)
{
	transport_catalogue_serialize::RGBA rgba;
	rgba.set_red(color.red);
	rgba.set_green(color.green);
	rgba.set_blue(color.blue);
	rgba.set_opacity(color.opacity);
	return rgba;
}

transport_catalogue_serialize::Color serializer::SerializeColor(const svg::Color& color)
{
	transport_catalogue_serialize::Color serialize_color;
	if (std::holds_alternative<std::string>(color))
	{
		serialize_color.set_string_color(std::get<std::string>(color));
	}
	if (std::holds_alternative<svg::Rgb>(color))
	{
		*serialize_color.mutable_rgb_color() = SerializeColorRGB(std::get<svg::Rgb>(color));
	}
	if (std::holds_alternative<svg::Rgba>(color))
	{
		*serialize_color.mutable_rgba_color() = SerializeColorRGBA(std::get<svg::Rgba>(color));
	}
	return serialize_color;
}

svg::Rgb serializer::DeserializeRGB(const transport_catalogue_serialize::RGB& color) {
	svg::Rgb result;
	result.red = color.red();
	result.green = color.green();
	result.blue = color.blue();
	return result;
}

svg::Rgba serializer::DeserializeRGBA(const transport_catalogue_serialize::RGBA& color) {
	svg::Rgba result;
	result.red = color.red();
	result.green = color.green();
	result.blue = color.blue();
	result.opacity = color.opacity();
	return result;
}

svg::Color serializer::DeserializeColor(const transport_catalogue_serialize::Color& s_color) {
	svg::Color ds_color;
	switch (s_color.variant_case()) {
	case transport_catalogue_serialize::Color::kStringColor:
		ds_color = s_color.string_color();
		break;
	case transport_catalogue_serialize::Color::kRGBColor:
		ds_color = DeserializeRGB(s_color.rgb_color());
		break;
	case transport_catalogue_serialize::Color::kRGBAColor:
		ds_color = DeserializeRGBA(s_color.rgba_color());
		break;
	case transport_catalogue_serialize::Color::VARIANT_NOT_SET:
		ds_color = svg::NoneColor;
		break;
	}
	return ds_color;
}