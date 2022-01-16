cmake_minimum_required(VERSION 3.14)
project(RealEngine)

add_subdirectory(${PROJECT_NAME})
			 
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

# GLM
include(ExternalProject)
ExternalProject_Add(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ExternalProject_Get_Property(glm source_dir)
set(glmIncludeDirs ${source_dir})

# nlohmann json
include(FetchContent)
set(JSON_Install ON)
FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  GIT_PROGRESS TRUE
  GIT_SHALLOW TRUE
  GIT_TAG v3.10.5
 )
FetchContent_MakeAvailable(json)

add_dependencies(${PROJECT_NAME} glm)

target_link_libraries(${PROJECT_NAME} INTERFACE glm PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(${PROJECT_NAME} PUBLIC . ${glmIncludeDirs})