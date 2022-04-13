add_subdirectory(room)

target_sources(${PROJECT_NAME} PUBLIC
	CommandLineArguments.hpp
	Error.hpp				Error.cpp
	RealEngine.hpp			RealEngine.cpp
	MainProgram.hpp			MainProgram.cpp
	Synchronizer.hpp		Synchronizer.cpp
	Window.hpp				Window.cpp
	WindowSettings.hpp		WindowSettings.cpp
)
