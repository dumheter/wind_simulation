project(runtime)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
	src/runtime/network/client.cpp
	src/runtime/network/connection_id.cpp
	src/runtime/network/net_common.cpp
	src/runtime/network/packet.cpp
	src/runtime/network/server.cpp
	src/runtime/network/side.cpp
	src/runtime/network/util.cpp
	src/runtime/cmyplayer.cpp
	src/runtime/main.cpp
	src/runtime/ui.cpp
	src/runtime/world.cpp
	)

set(HEADERS
	src/runtime/network/client.hpp
	src/runtime/network/connection_id.hpp
	src/runtime/network/connection_state.hpp
	src/runtime/network/net_common.hpp
	src/runtime/network/packet.hpp
	src/runtime/network/packet_header.hpp
	src/runtime/network/server.hpp
	src/runtime/network/side.hpp
	src/runtime/network/util.hpp
	src/runtime/cmyplayer.hpp
	src/runtime/ui.hpp
	src/runtime/world.hpp
	)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_directories(${PROJECT_NAME} PRIVATE deps/gamenetworkingsockets/lib)

target_link_libraries(${PROJECT_NAME} shared GameNetworkingSockets)

target_include_directories(${PROJECT_NAME} PRIVATE
	src/
	deps/gamenetworkingsockets/include
	../shared/src/
	)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	DLOG_FILESTAMP
	DLOG_TIMESTAMP
	MICROPROFILE_GPU_TIMERS_D3D11=1
	MICROPROFILE_ENABLED=1
	)
