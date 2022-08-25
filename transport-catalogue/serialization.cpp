#include "serialization.h"


// ----------------------------------------------------------------------------
// main methods
void serializer::Serialize(TransportCatalogue& tc, MapRenderer& mr, TransportRouter& tr, Path& file)
{
	std::ofstream out(file, std::ios::binary);

	transport_catalogue_serialize::Data data;
	transport_catalogue_serialize::TransportCatalogue data_tc = SerializeTransportCatalogue(tc);
	*data.mutable_transport_catalogue() = data_tc;
	transport_catalogue_serialize::MapRenderer data_mr = SerializeMapRenderer(mr);
	*data.mutable_map_renderer() = data_mr;
	transport_catalogue_serialize::Router data_tr = SerializeTransportRouter(tr);
	*data.mutable_router() = data_tr;

	data.SerializeToOstream(&out);
}

void serializer::Deserialize(Path& file, TransportCatalogue& tc, MapRenderer& mr, TransportRouter& tr)
{
	std::ifstream ifs(file, std::ios::binary);
	transport_catalogue_serialize::Data data;
	if (data.ParseFromIstream(&ifs))
	{
		transport_catalogue_serialize::TransportCatalogue data_tc = data.transport_catalogue();
		DeserializeTransportCatalogue(data_tc, tc);
		transport_catalogue_serialize::MapRenderer data_mr = data.map_renderer();
		DeserializeMapRenderer(data_mr, mr);
		transport_catalogue_serialize::Router data_tr = data.router();
		DeserializeTransportRouter(data_tr, tr);
	}
}

// ----------------------------------------------------------------------------
// secondary methods
transport_catalogue_serialize::TransportCatalogue serializer::SerializeTransportCatalogue(TransportCatalogue& tc)
{
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

	return data_tc;
}

transport_catalogue_serialize::MapRenderer serializer::SerializeMapRenderer(MapRenderer& mr)
{
	transport_catalogue_serialize::RendererSettings data_rs;
	transport_catalogue_serialize::MapRenderer data_mr;

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

	*data_mr.mutable_render_settings() = data_rs;
	
	return data_mr;
}

transport_catalogue_serialize::Router serializer::SerializeTransportRouter(TransportRouter& tr)
{
	transport_catalogue_serialize::Router data_r;
	transport_catalogue_serialize::TransportRouter data_tr;
	
	transport_catalogue_serialize::RoutingSettings data_rs;
	data_rs.set_bus_wait_time(tr.GetRoutingSettings().bus_wait_time_);
	data_rs.set_bus_velocity(tr.GetRoutingSettings().bus_velocity_);

	*data_tr.mutable_routing_settings() = data_rs;

	for (size_t i = 0; i < tr.GetGraph().GetEdgeCount(); ++i)
	{
		graph::Edge<double> tr_edge = tr.GetGraph().GetEdge(i);
		transport_catalogue_serialize::Edge* s_edge;
		s_edge = data_tr.add_edges();
		s_edge->set_from(tr_edge.from);
		s_edge->set_to(tr_edge.to);
		s_edge->set_weight(tr_edge.weight);
	}
	for (const auto [name, id] : tr.GetNameIdOfStops())
	{
		transport_catalogue_serialize::StopInfo* s_stop_info;
		s_stop_info = data_tr.add_stop_info();
		s_stop_info->set_stop_name(name.data());
		s_stop_info->set_edge_id(id);
	}
	for (const auto [id, name] : tr.GetIdNameOfBuses())
	{
		transport_catalogue_serialize::BusInfo* s_bus_info;
		s_bus_info = data_tr.add_bus_info();
		s_bus_info->set_bus_name(name.data());
		s_bus_info->set_edge_id(id);
	}
	for (const auto [id, count] : tr.GetIdCountOfStops())
	{
		transport_catalogue_serialize::CountInfo* s_count_info;
		s_count_info = data_tr.add_count_info();
		s_count_info->set_stop_count(count);
		s_count_info->set_edge_id(id);
	}

	*data_tr.mutable_routing_settings() = data_rs;
	*data_r.mutable_transport_router() = data_tr;

	return data_r;
}


void serializer::DeserializeTransportCatalogue(transport_catalogue_serialize::TransportCatalogue& data, TransportCatalogue& tc)
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
	int u = data.distances_size();
	for (int i = 0; i < u; ++i)
	{
		transport_catalogue_serialize::Distance d = data.distances(i);
		std::string from = d.stop_from();
		std::string to = d.stop_to();
		tc.SetDistances(d.stop_from(), d.stop_to(), d.distance());
	}
}

void serializer::DeserializeMapRenderer(transport_catalogue_serialize::MapRenderer& data, MapRenderer& mr)
{
	
	transport_catalogue_serialize::RendererSettings data_rs = data.render_settings();
	
	RenderSettings render_settings;
	render_settings.width_ = data_rs.width();
	render_settings.height_ = data_rs.height();
	render_settings.padding_ = data_rs.padding();
	render_settings.line_width_ = data_rs.line_width();
	render_settings.stop_radius_ = data_rs.stop_radius();
	render_settings.underlayer_width_ = data_rs.underlayer_width();
	render_settings.bus_label_font_size_ = data_rs.bus_label_font_size();
	render_settings.stop_label_font_size_ = data_rs.stop_label_font_size();
	render_settings.bus_label_offset_.clear();
	for (size_t i = 0; i < data_rs.bus_label_offset_size(); ++i)
	{
		render_settings.bus_label_offset_.push_back(data_rs.bus_label_offset(i));
	}
	render_settings.stop_label_offset_.clear();
	for (size_t i = 0; i < data_rs.stop_label_offset_size(); ++i)
	{
		render_settings.stop_label_offset_.push_back(data_rs.stop_label_offset(i));
	}
	render_settings.underlayer_color_ = DeserializeColor(data_rs.underlayer_color());
	render_settings.color_palette_.clear();
	for (int i = 0; i < data_rs.color_palette_size(); ++i)
	{
		render_settings.color_palette_.push_back(DeserializeColor(data_rs.color_palette(i)));
	}
	mr.SetRenderSettings(render_settings);
}

void serializer::DeserializeTransportRouter(transport_catalogue_serialize::Router& data, TransportRouter& tr)
{
	// deserialize routing settings
	transport_catalogue_serialize::TransportRouter data_tr = data.transport_router();

	RoutingSettings routing_settings;
	routing_settings.bus_wait_time_ = data_tr.routing_settings().bus_wait_time();
	routing_settings.bus_velocity_ = data_tr.routing_settings().bus_velocity();
	tr.SetRoutingSettings(routing_settings);

	for (size_t i = 0; i < data_tr.stop_info_size(); ++i)
	{
		transport_catalogue_serialize::StopInfo stop_info = data_tr.stop_info(i);
		std::string s = stop_info.stop_name();
		tr.SetNameIdOfStop(s, stop_info.edge_id());
	}
	for (size_t i = 0; i < data_tr.bus_info_size(); ++i)
	{
		transport_catalogue_serialize::BusInfo bus_info = data_tr.bus_info(i);
		tr.SetIdNameOfBus(bus_info.edge_id(), bus_info.bus_name());
	}
	for (size_t i = 0; i < data_tr.count_info_size(); ++i)
	{
		transport_catalogue_serialize::CountInfo count_info = data_tr.count_info(i);
		tr.SetIdCountOfStop(count_info.edge_id(), count_info.stop_count());
	}
	tr.GetGraph().SetVertexCount(data_tr.edges_size());
	for (size_t i = 0; i < data_tr.edges_size(); ++i)
	{
		transport_catalogue_serialize::Edge s_edge = data_tr.edges(i);
		graph::Edge<double> tr_edge;
		tr_edge.from = s_edge.from();
		tr_edge.to = s_edge.to();
		tr_edge.weight = s_edge.weight();
		tr.GetGraph().AddEdge(tr_edge);
	}
	tr.SetRouter(tr.GetGraph());
}

// ----------------------------------------------------------------------------
// serialize/deserialize svg

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