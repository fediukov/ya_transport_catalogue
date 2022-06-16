#pragma once

#include <map>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "geo.h"
#include "domain.h"
#include "json.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"

class RequestHandler;

class JsonReader {

public:
    JsonReader() = default;

    void FillTransportCatalogue(std::istream& input, transport_catalogue::TransportCatalogue& tc);
    void ConnectMapRenderer(MapRenderer& mr);
    std::ostream& GetStatRequest(
        std::ostream& out,
        transport_catalogue::TransportCatalogue& tc,
        MapRenderer& mr);

private:
	// main functions
    void ReadJson(std::istream& input);
    std::ostream& GetTransportCatalogueData(
        std::ostream& out,
        transport_catalogue::TransportCatalogue& tc,
        json::Dict& request);

    // secondary functions
    void ParseBaseRequests(json::Array& base_requests);
    domain::Bus ParseBaseRequestsOfBus(json::Dict& request);
    domain::Stop ParseBaseRequestsOfStop(json::Dict& request);
    void ParseStatRequests(json::Array& stat_requests);
    void ParseRenderSettings(json::Dict& render_settings);
    std::vector<double>& ParseRenderSettingsArray(std::vector<double>& setting, json::Array& array);
    svg::Color& ParseRenderSettingsColor(svg::Color& setting, json::Node& array);
    
private:
    std::vector<domain::Bus> bus_request_;
	std::vector<domain::Stop> stop_request_;
	std::unordered_map<std::string, std::unordered_map<std::string, size_t>> distances_between_stops_;

	std::vector<json::Dict> stat_requests_;

    RenderSettings render_settings_;
};