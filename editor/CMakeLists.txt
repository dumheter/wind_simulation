project(editor)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
	src/editor/editor.cpp
	src/editor/factory.cpp
	src/editor/main.cpp
	src/editor/ui.cpp
	)

set(HEADERS
	src/editor/editor.hpp
	src/editor/factory.hpp
	src/editor/ui.hpp
	)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} shared)

target_include_directories(${PROJECT_NAME} PRIVATE
	./src/
	../shared/src/
	)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	DLOG_FILESTAMP
	DLOG_TIMESTAMP
	MICROPROFILE_GPU_TIMERS_D3D11=1
	MICROPROFILE_ENABLED=1
	)
