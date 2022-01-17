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

#glew
FetchContent_Declare(GLEW
  GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
  GIT_PROGRESS TRUE
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm json GLEW)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		glm
		nlohmann_json::nlohmann_json
		libglew_static
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		.
)