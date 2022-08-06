add_subdirectory(room)
add_subdirectory(window)

target_sources(${PROJECT_NAME} PUBLIC
	CommandLineArguments.hpp
	Error.hpp				Error.cpp
	MainProgram.hpp			MainProgram.cpp
	Synchronizer.hpp		Synchronizer.cpp
)
