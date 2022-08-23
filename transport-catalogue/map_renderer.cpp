#include "map_renderer.h"

std::ostream& MapRenderer::Draw(std::ostream& out, transport_catalogue::TransportCatalogue& tc)
{
	GetBuses(tc);
	GetStops(tc);
	DrawRoutes(out);
	return out;
}

void MapRenderer::SetRenderSettings(const RenderSettings& rc)
{
	render_settings_ = rc;
}

RenderSettings MapRenderer::GetRenderSettings()
{
	return render_settings_;
}

void MapRenderer::GetBuses(transport_catalogue::TransportCatalogue& tc)
{
	for (const auto bus : tc.GetAllBuses())
	{
		bus_names_.insert((*bus).name);
		buses_.push_back(bus);
	}
}

void MapRenderer::GetStops(transport_catalogue::TransportCatalogue& tc)
{
	for (const auto stop : tc.GetAllStops())
	{
		stop_names_.insert((*stop).name);
		stops_.push_back(stop);
	}
}

std::ostream& MapRenderer::DrawRoutes(std::ostream& out)
{
	// svg document
	svg::Document doc;
	
	// define all stops coordinates where there are buses
	std::vector<geo::Coordinates> geo_coordinates;
	for (const auto& bus_name_ : bus_names_)
	{
		// find bus (bus_ptr) by bus name
		auto* bus = *(find_if(buses_.begin(), buses_.end(),
			[&bus_name_](const auto bus) { return bus->name == bus_name_; }));
		// add geo coordinates of bus stops from route bus to vector 
		if ((*bus).route.size())
		{
			for (size_t i = 0; i < (*bus).route.size(); ++i)
			{
				geo_coordinates.push_back((*bus).route.at(i)->coordinates);
			}
		}
	}
	// create projector translated geo coordinates to screen coordinates
	const SphereProjector proj{
		geo_coordinates.begin(), geo_coordinates.end(),
		render_settings_.width_, render_settings_.height_, render_settings_.padding_
	};

	// conteiners to add to doc
	std::vector<svg::Polyline> route_lines;
	std::vector<svg::Text> route_names_bottom;
	std::vector<svg::Text> route_names_up;
	std::set<std::string> stop_names;
	std::vector<svg::Circle> stop_circles;
	std::vector<svg::Text> stop_names_bottom;
	std::vector<svg::Text> stop_names_up;
	
	//run through bus routes
	size_t pos_pallete = 0;
	for (const auto& bus_name_ : bus_names_)
	{
		// set color for route
		svg::Color route_color = render_settings_.color_palette_.at(pos_pallete++);
		pos_pallete = pos_pallete >= render_settings_.color_palette_.size()
			? pos_pallete - render_settings_.color_palette_.size()
			: pos_pallete;//*/
		// find bus (bus_ptr) by bus name
		auto* bus = *(find_if(buses_.begin(), buses_.end(),
			[&bus_name_](const auto bus) { return bus->name == bus_name_; }));
		if ((*bus).route.size())
		{
			svg::Polyline route_line;
			bool circle_route = false;
			svg::Text route_name_bottom;
			svg::Text route_name_up;
			// run through bus route stops
			for (size_t i = 0; i < (*bus).route.size(); ++i)
			{
				// add only stop name by bus for following actions
				stop_names.insert((*bus).route.at(i)->name);
				// get geo coordinates of stop
				geo::Coordinates geo_stop = (*bus).route.at(i)->coordinates;
				// get screen coordinates of stop
				svg::Point screen_coord = proj(geo_stop);
				// add point of route for line
				route_line.AddPoint(screen_coord);
								
				// check first or last stop for bus route text
				if (i == 0)
				{
					svg::Point offset{
					render_settings_.bus_label_offset_[0],
					render_settings_.bus_label_offset_[1] };
					// check wether route is circle or not
					circle_route = (*bus).round_route;
					/*for (size_t j = 0; j < ((*bus).route.size() / 2); ++j)
					{
						if ((*bus).route.at(j)->name != (*bus).route.at((*bus).route.size() - 1 - j)->name)
						{
							circle_route = true;
							break;
						}
					}//*/
					// create and add text object for first stop
					route_name_bottom.
						SetPosition(screen_coord).
						SetOffset(offset).
						SetFontSize(render_settings_.bus_label_font_size_).
						SetFontFamily("Verdana").
						SetFontWeight("bold").
						SetFillColor(render_settings_.underlayer_color_).
						SetStrokeColor(render_settings_.underlayer_color_).
						SetStrokeWidth(render_settings_.underlayer_width_).
						SetStrokeLineCap(svg::StrokeLineCap::ROUND).
						SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).
						SetData((*bus).name);
					route_name_up.
						SetPosition(screen_coord).
						SetOffset(offset).
						SetFontSize(render_settings_.bus_label_font_size_).
						SetFontFamily("Verdana").
						SetFontWeight("bold").
						SetFillColor(route_color).
						SetData((*bus).name);
					route_names_bottom.push_back(route_name_bottom);
					route_names_up.push_back(route_name_up);
				}
				// add text object for last stop of not circle route
				if (circle_route == false
					&& i == ((*bus).route.size() / 2)
					&& (*bus).route.at((*bus).route.size() / 2)->name != (*bus).route.at(0)->name)
				{
					route_name_bottom.
						SetPosition(screen_coord);
					route_name_up.
						SetPosition(screen_coord);
					route_names_bottom.push_back(route_name_bottom);
					route_names_up.push_back(route_name_up);
				}
			}
			// create and add route line for doc
			route_line.SetFillColor(svg::NoneColor).
				SetStrokeColor(route_color).
				SetStrokeWidth(render_settings_.line_width_).
				SetStrokeLineCap(svg::StrokeLineCap::ROUND).
				SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
			route_lines.push_back(route_line);//*/
		}
	}

	//run through stops routes
	for (const auto& stop_name : stop_names)
	{
		// find stop (bus_ptr) by stop name
		auto* stop = *(find_if(stops_.begin(), stops_.end(),
			[&stop_name](const auto stop) { return stop->name == stop_name; }));
		// get geo coordinates of stop
		geo::Coordinates geo_stop = (*stop).coordinates;
		// get screen coordinates of stop
		svg::Point screen_coord = proj(geo_stop);

		// add stop circle
		svg::Circle stop_circle;
		stop_circle.SetCenter(screen_coord).
			SetRadius(render_settings_.stop_radius_).
			SetFillColor("white");
		stop_circles.push_back(stop_circle);//*/

		// add text object for last stop of not circle route
		svg::Text stop_name_bottom;
		svg::Text stop_name_up;
		svg::Point offset{
			render_settings_.stop_label_offset_[0],
			render_settings_.stop_label_offset_[1] };
		stop_name_bottom.
			SetPosition(screen_coord).
			SetOffset(offset).
			SetFontSize(render_settings_.stop_label_font_size_).
			SetFontFamily("Verdana").
			SetFillColor(render_settings_.underlayer_color_).
			SetStrokeColor(render_settings_.underlayer_color_).
			SetStrokeWidth(render_settings_.underlayer_width_).
			SetStrokeLineCap(svg::StrokeLineCap::ROUND).
			SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).
			SetData((*stop).name);
		stop_name_up.
			SetPosition(screen_coord).
			SetOffset(offset).
			SetFontSize(render_settings_.stop_label_font_size_).
			SetFontFamily("Verdana").
			SetFillColor("black").
			SetData((*stop).name);
		stop_names_bottom.push_back(stop_name_bottom);
		stop_names_up.push_back(stop_name_up);//*/
	}

	// add route lines to doc
	for (const auto& line : route_lines)
	{
		doc.Add(svg::Polyline{ line });
	}

	// add route names to doc
	for (auto i = route_names_bottom.begin(), j = route_names_up.begin();
		i != route_names_bottom.end() && j != route_names_up.end();
		++i, ++j)
	{
		doc.Add(svg::Text{ *i });
		doc.Add(svg::Text{ *j });
	}

	// add stop circles to doc
	for (const auto& circle : stop_circles)
	{
		doc.Add(svg::Circle{ circle });
	}

	// add stop names to doc
	for (auto i = stop_names_bottom.begin(), j = stop_names_up.begin();
		i != stop_names_bottom.end() && j != stop_names_up.end();
		++i, ++j)
	{
		doc.Add(svg::Text{ *i });
		doc.Add(svg::Text{ *j });
	}//*/

	doc.Render(out);
	return out;//*/
}
