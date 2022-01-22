add_subdirectory(details)

target_sources(${PROJECT_NAME} PUBLIC
	CommandLineArguments.hpp
	Error.hpp				Error.cpp
	MainProgram.hpp			MainProgram.cpp
	Room.hpp				Room.cpp
	RoomManager.hpp			RoomManager.cpp
	Synchronizer.hpp		Synchronizer.cpp
	Window.hpp				Window.cpp
)
