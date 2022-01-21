cmake_minimum_required(VERSION 3.14)
project(RealEngine)

add_library(${PROJECT_NAME} STATIC)

add_subdirectory(${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

# OpenGL
find_package(OpenGL REQUIRED)

# GLEW
find_package(GLEW 2.0 REQUIRED)

# SDL2 + SDL2_ttf
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

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

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		OpenGL::GL
		GLEW::GLEW
		SDL2::Main
		SDL2::TTF
		glm
		nlohmann_json::nlohmann_json
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		.
)
