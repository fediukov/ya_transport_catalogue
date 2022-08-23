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

	transport_catalogue_serialize::RGB SerializeColorRGB(const svg::Rgb& color);
	transport_catalogue_serialize::RGBA SerializeColorRGBA(const svg::Rgba& color);
	transport_catalogue_serialize::Color SerializeColor(const svg::Color& color);

	svg::Rgb DeserializeRGB(const transport_catalogue_serialize::RGB& color);
	svg::Rgba DeserializeRGBA(const transport_catalogue_serialize::RGBA& color);
	svg::Color DeserializeColor(const transport_catalogue_serialize::Color& color);

	//transport_catalogue_serialize::RendererSettings SerializeRenderSettings(const RenderSettings& render_settings);
}