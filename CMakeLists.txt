cmake_minimum_required(VERSION 3.14)
project(RealEngine)

add_subdirectory(${PROJECT_NAME})
			 
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME} PUBLIC . glm nlohmann/include)