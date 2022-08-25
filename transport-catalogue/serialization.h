#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <string_view>
#include <variant>

#include "graph.h"
#include "graph.pb.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "map_renderer.pb.h"
#include "transport_catalogue.h"
#include "transport_catalogue.pb.h"
#include "transport_router.h"
#include "transport_router.pb.h"

namespace serializer
{
	using TransportCatalogue = transport_catalogue::TransportCatalogue;
	using Path = std::filesystem::path;

	void Serialize(TransportCatalogue& tc, MapRenderer& mr, TransportRouter& tr, Path& file);
	void Deserialize(Path& file, TransportCatalogue& tc, MapRenderer& mr, TransportRouter& tr);

	//
	transport_catalogue_serialize::TransportCatalogue SerializeTransportCatalogue(TransportCatalogue& tc);
	transport_catalogue_serialize::MapRenderer SerializeMapRenderer(MapRenderer& mr);
	transport_catalogue_serialize::Router SerializeTransportRouter(TransportRouter& tr);

	void DeserializeTransportCatalogue(transport_catalogue_serialize::TransportCatalogue& data, TransportCatalogue& tc);
	void DeserializeMapRenderer(transport_catalogue_serialize::MapRenderer& data, MapRenderer& mr);
	void DeserializeTransportRouter(transport_catalogue_serialize::Router& data, TransportRouter& tr);

	// serialize/deserialize svg
	transport_catalogue_serialize::RGB SerializeColorRGB(const svg::Rgb& color);
	transport_catalogue_serialize::RGBA SerializeColorRGBA(const svg::Rgba& color);
	transport_catalogue_serialize::Color SerializeColor(const svg::Color& color);

	svg::Rgb DeserializeRGB(const transport_catalogue_serialize::RGB& color);
	svg::Rgba DeserializeRGBA(const transport_catalogue_serialize::RGBA& color);
	svg::Color DeserializeColor(const transport_catalogue_serialize::Color& color);
}