cmake_minimum_required(VERSION 3.14)
project(RealEngine)

add_subdirectory(${PROJECT_NAME})
			 
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

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

#glew
find_package(GLEW 2.0)

#SDL2
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		glm
		nlohmann_json::nlohmann_json
		GLEW::GLEW
		SDL2::Main
		SDL2::TTF
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		.
)
