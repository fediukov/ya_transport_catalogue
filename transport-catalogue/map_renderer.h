#pragma once

#include <algorithm>
#include <deque>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "domain.h"
#include "geo.h"
#include "svg.h"
#include "transport_catalogue.h"

inline const double EPSILON = 1e-6;

struct RenderSettings {
    double width_ = 300.0;
    double height_ = 200.0;
    double padding_ = 50.0;
    double line_width_ = 14.0;
    double stop_radius_ = 5.0;
    int bus_label_font_size_ = 20;
    std::vector<double> bus_label_offset_ = { 7.0, 15.0 };
    int stop_label_font_size_ = 20;
    std::vector<double> stop_label_offset_ = { 7.0, -3.0 };
    svg::Color underlayer_color_ = svg::Rgba{ 255, 255, 255, 0.85 };
    double underlayer_width_ = 3.0;
    std::vector<svg::Color> color_palette_ = { "green", svg::Rgb{255, 160, 0}, "red" };
};//*/

class MapRenderer {
public:
    std::ostream& Draw(std::ostream& out, transport_catalogue::TransportCatalogue& tc);
    void SetRenderSettings(const RenderSettings& rc);

private:
    void GetBuses(transport_catalogue::TransportCatalogue& tc);
    void GetStops(transport_catalogue::TransportCatalogue& tc);

    std::ostream& DrawRoutes(std::ostream& out);

private:
    RenderSettings render_settings_;
    
    std::set<std::string> stop_names_;
    std::vector<domain::Stop*> stops_;

    std::set<std::string> bus_names_;
    std::vector<domain::Bus*> buses_;
};

class SphereProjector {
public:
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
        double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!(std::abs(max_lon - min_lon_) < EPSILON)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!(std::abs(max_lat_ - min_lat) < EPSILON)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        }
        else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        }
        else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};
