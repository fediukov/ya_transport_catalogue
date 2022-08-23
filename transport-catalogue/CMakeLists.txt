cmake_minimum_required(VERSION 3.10)

project(Transport_catalogue CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS transport_catalogue.proto map_renderer.proto transport_router.proto graph.proto)

# Добавьте источник в исполняемый файл этого проекта.
add_executable (transport_catalogue
#	"transport_catalogue.h" "transport_catalogue.cpp"
	${PROTO_SRCS} ${PROTO_HDRS}
	"domain.h"
	"geo.h"
	"graph.h"
#	"input_reader.h" "input_reader.cpp"
	"json.h" "json.cpp"
	"json_builder.h" "json_builder.cpp"
	"json_reader.h" "json_reader.cpp"
	"main.cpp"
	"map_renderer.h" "map_renderer.cpp"
	"ranges.h"
	"router.h"
	"serialization.h" "serialization.cpp"
#	"stat_reader.h" "stat_reader.cpp"
	"svg.h" "svg.cpp"
	"transport_catalogue.h" "transport_catalogue.cpp"
	"transport_router.h" "transport_router.cpp" "serialization.cpp")

target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)