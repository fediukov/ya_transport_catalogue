#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


#include "json_reader.h"
#include "map_renderer.h"
//#include "request_handler.h"
#include "serialization.h"
#include "transport_catalogue.h"

using namespace std;
using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv)
    {
        transport_catalogue::TransportCatalogue tc;
        JsonReader jr;
        MapRenderer mr;
        jr.FillTransportCatalogue(std::cin, tc);
        jr.ConnectMapRenderer(mr);
        serializer::Path file_path = std::filesystem::path(jr.GetSerializationSettings().at("file"));
        serializer::Serialize(tc, mr, jr.GetTransportRouter(), file_path);
    }
    else if (mode == "process_requests"sv)
    {
        transport_catalogue::TransportCatalogue tc;
        JsonReader jr;
        MapRenderer mr;
        jr.FillTransportCatalogue(std::cin, tc);
        serializer::Path file_path = std::filesystem::path(jr.GetSerializationSettings().at("file"));
        serializer::Deserialize(file_path, tc, mr, jr.GetTransportRouter());
        jr.GetStatRequest(std::cout, tc, mr);
    }
    else
    {
        PrintUsage();
        return 1;
    }
}