project(shared)

## ========================================================================== ##
## Clone dependencies for shaderc
## ========================================================================== ##

# SPIRV-Tools
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/shaderc/third_party/SPIRV-Tools)
else()
	message(STATUS "Missing SPIRV-Tools, repository will be cloned")
	execute_process(
		COMMAND
		git clone https://github.com/KhronosGroup/SPIRV-Tools src/thirdparty/shaderc/third_party/SPIRV-Tools
		WORKING_DIRECTORY
		${CMAKE_CURRENT_SOURCE_DIR}
	)
endif()

# SPIRV-Headers
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/shaderc/third_party/SPIRV-Headers)
else()
	message(STATUS "Missing SPIRV-Headers, repository will be cloned")
	execute_process(
		COMMAND
		git clone https://github.com/KhronosGroup/SPIRV-Headers src/thirdparty/shaderc/third_party/SPIRV-Headers
		WORKING_DIRECTORY
		${CMAKE_CURRENT_SOURCE_DIR}
	)
endif()

# glslang
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/shaderc/third_party/glslang)
else()
	message(STATUS "Missing glslang, repository will be cloned")
	execute_process(
		COMMAND
		git clone https://github.com/KhronosGroup/glslang src/thirdparty/shaderc/third_party/glslang
		WORKING_DIRECTORY
		${CMAKE_CURRENT_SOURCE_DIR}
	)
endif()

## ========================================================================== ##
## Flags
## ========================================================================== ##

set(CMAKE_CXX_STANDARD 17)

## ========================================================================== ##
## Sources/Headers
## ========================================================================== ##

set(SOURCES
	src/shared/file/file_system.cpp
	src/shared/file/path.cpp
	src/shared/render/shader.cpp
	src/shared/render/shader_compiler.cpp
	src/shared/app.cpp
	src/shared/assert.cpp
	src/shared/memory.cpp
	src/shared/string.cpp
	src/thirdparty/glad/glad.c
	src/thirdparty/plommon/pm_unicode.c
	)

set(HEADERS
	src/shared/file/file_system.hpp
	src/shared/file/path.hpp
	src/shared/render/common.hpp
	src/shared/render/shader.hpp
	src/shared/render/shader_compiler.hpp
	src/shared/app.hpp
	src/shared/assert.hpp
	src/shared/common.hpp
	src/shared/macros.hpp
	src/shared/memory.hpp
	src/shared/string.hpp
	src/shared/types.hpp
	src/thirdparty/glad/glad.h
	src/thirdparty/KHR/khrplatform.h
	src/thirdparty/plommon/pm_unicode.h
	)

## ========================================================================== ##
## External
## ========================================================================== ##

add_subdirectory(src/thirdparty/glfw)

add_subdirectory(src/thirdparty/fmt)

set(SHADERC_SKIP_INSTALL ON)
set(SHADERC_SKIP_TESTS ON)
add_subdirectory(src/thirdparty/shaderc)

set(SPIRV_CROSS_SKIP_INSTALL ON)
set(SPIRV_CROSS_ENABLE_TESTS OFF)
add_subdirectory(src/thirdparty/SPIRV-cross)

add_subdirectory(src/thirdparty/mimalloc)

## ========================================================================== ##
## Output
## ========================================================================== ##

add_library(${PROJECT_NAME} 
	${SOURCES}
	${HEADERS}
	)

target_link_libraries(${PROJECT_NAME} 
	glfw 
	fmt-header-only 
	mimalloc-static
	shaderc 
	spirv-cross-core
    spirv-cross-glsl)

target_include_directories(${PROJECT_NAME} PUBLIC
	src/
	src/thirdparty/
	src/thirdparty/glfw/include
	src/thirdparty/fmt/include
	src/thirdparty/shaderc/libshaderc/include
	src/thirdparty/SPIRV-Cross
	)