cmake_minimum_required(VERSION 3.14)
project(RealEngine)

# RealEngine library
add_library(RealEngine STATIC)

add_subdirectory(RealEngine)
add_subdirectory(external)

set_target_properties(RealEngine PROPERTIES
	CXX_STANDARD 20
)

# Vulkan

find_package(Vulkan REQUIRED)

# OpenGL
find_package(OpenGL REQUIRED)

# GLEW
find_package(GLEW 2.0 REQUIRED)

# SDL2
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
find_package(SDL2 REQUIRED)

# GLM
include(FetchContent)
FetchContent_Declare(glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_PROGRESS TRUE
  GIT_SHALLOW TRUE
)

# nlohmann json
set(JSON_Install ON)
FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent.git
  GIT_PROGRESS TRUE
  GIT_SHALLOW TRUE
  GIT_TAG v3.10.5
)

FetchContent_MakeAvailable(glm json)

target_link_libraries(RealEngine
	PUBLIC
		${CMAKE_DL_LIBS}
        Vulkan::Vulkan
		OpenGL::GL
		GLEW::GLEW
		SDL2::Main
		glm
		nlohmann_json::nlohmann_json
)

target_include_directories(RealEngine
	PUBLIC
		.
		external
)

# RTICreator  (Windows only)
if(WIN32)
	add_executable(RTICreator)
	target_include_directories(RTICreator PRIVATE .)

	set_target_properties(RTICreator PROPERTIES
		CXX_STANDARD 20
	)

	target_link_libraries(RTICreator
		PUBLIC
			RealEngine
	)
	
	add_subdirectory(RTICreator)
endif()