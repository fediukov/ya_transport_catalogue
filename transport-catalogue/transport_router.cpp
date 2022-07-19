#include "transport_router.h"

void TransportRouter::SetGraph(
	const std::vector<domain::Bus>& buses,
	const std::vector<domain::Stop>& stops,
	const std::unordered_map<std::string, std::unordered_map<std::string, size_t>>& distances
)
{
	// get ids for all stops - it will be vertexes
	for (size_t i = 0; i < stops.size(); ++i)
	{
		name_id_of_stops_[stops.at(i).name] = i;
		id_name_of_stops_[i] = stops.at(i).name;
	}

	graph_.SetVertexCount(name_id_of_stops_.size());

	// add edges to graph
	for (const auto& bus : buses)
	{
		double speed = routing_settings_.bus_velocity_ * 1000. / 60;
		if (bus.round_route)
		{
			// (bus, begin_pos, end_pos + 1, distances)
			AddEdgesOfRoute(bus, 0, bus.route.size(), distances);
		}
		else // if (!bus.round_route)
		{
			// right direction of route
			AddEdgesOfRoute(bus, 0, (bus.route.size() / 2) + 1, distances);
			// back direction of route
			AddEdgesOfRoute(bus, (bus.route.size() / 2), bus.route.size(), distances);
		}
	}
}

void TransportRouter::AddEdgesOfRoute(const domain::Bus& bus, size_t pos_from, size_t pos_to,
	const std::unordered_map<std::string, std::unordered_map<std::string, size_t>>& distances)
{
	for (size_t i = pos_from + 1; i < pos_to; ++i)
	{
		for (size_t j = pos_from; j < i; ++j)
		{
			size_t distance = 0.;
			double weight = routing_settings_.bus_wait_time_;
			double speed = routing_settings_.bus_velocity_ * 1000. / 60;
			//
			size_t vertex_id_from = name_id_of_stops_.at(bus.route.at(j)->name);
			size_t vertex_id_to = name_id_of_stops_.at(bus.route.at(i)->name);

			// look for a distance
			for (size_t k = j + 1; k <= i; ++k)
			{
				distance += GetDistanceBetweenTwoStops(bus.route.at(k - 1)->name, bus.route.at(k)->name, distances);
			}
			// compute weight considering bus_wait_time
			weight += (distance * 1. / speed);
			// create edge
			graph::Edge<double> edge = { vertex_id_from , vertex_id_to, weight };

			size_t edge_id = graph_.AddEdge(edge);
			id_name_of_buses_[edge_id] = bus.name;
			id_count_of_stops[edge_id] = i - j;
		}
	}
}

double TransportRouter::GetDistanceBetweenTwoStops(const std::string& from, const std::string& to,
	const std::unordered_map<std::string, std::unordered_map<std::string, size_t>>& distances)
{
	double distance = 0.;

	auto it_from = distances.find(from);
	if (it_from != distances.end())
	{
		auto it_to = (*it_from).second.find(to);
		if (it_to != (*it_from).second.end())
		{
			distance = (*it_to).second;
		}
		else
		{
			it_from = distances.find(to);
			if (it_from != distances.end())
			{
				it_to = (*it_from).second.find(from);
				if (it_to != (*it_from).second.end())
				{
					distance = (*it_to).second;
				}
			}
		}
	}
	else
	{
		it_from = distances.find(to);
		if (it_from != distances.end())
		{
			auto it_to = (*it_from).second.find(from);
			if (it_to != (*it_from).second.end())
			{
				distance = (*it_to).second;
			}
		}
	}//*/

	return distance;
}

std::vector<TransportRouter::RouterInfo> TransportRouter::GetRoute(std::string stop_from, std::string stop_to)
{
	std::vector<RouterInfo> result;

	size_t id_from = name_id_of_stops_.at(stop_from);
	size_t id_to = name_id_of_stops_.at(stop_to);

	std::optional<graph::Router<double>::RouteInfo> info;
	if ((*router_).BuildRoute(id_from, id_to).has_value())
	{
		info = *router_->BuildRoute(id_from, id_to);
	}

	RouterInfo router_info;

	if (id_from == id_to)
	{
		return result;
	}
	else if (info.has_value())
	{
		router_info.total_time_ = info.value().weight;

		for (size_t i = 0; i < info.value().edges.size(); ++i)
		{
			router_info.type_ = "Wait";
			router_info.time_ = routing_settings_.bus_wait_time_;
			router_info.stop_name_ = id_name_of_stops_[graph_.GetEdge(info.value().edges.at(i)).from];
			router_info.bus_name_ = "";
			router_info.stop_count_ = 0;

			result.push_back(router_info);

			router_info.type_ = "Bus";
			router_info.time_ = graph_.GetEdge(info.value().edges.at(i)).weight - routing_settings_.bus_wait_time_;
			router_info.bus_name_ = id_name_of_buses_.at(info.value().edges.at(i));;
			router_info.stop_count_ = id_count_of_stops.at(info.value().edges.at(i)); //!!!
			router_info.stop_name_ = "";

			result.push_back(router_info);
		}
	}//*/
	else
	{
		router_info.type_ = "Not Found";
		result.push_back(router_info);
	}//*/

	return result;
}