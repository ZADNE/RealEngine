# RealEngine: An engine-like library

## Features:
- A set of convenient classes to ease Vulkan 1.3 rendering
- Handling of mouse & keyboard input
- Simulation speed decoupled from variable frame rate
- A system of rooms / scenes / worlds with well defined transitions
- Supports Windows (MSVC) and Linux (GCC)

## Design goals:

- A lightweight library, not a full-fledged engine with all sorts of subsystems
- As little runtime overhead as possible
- A modern C++20 library following the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- Doxygen documented at least at class level

## The library was used to develop:

- [RealWorld: A tile-based world simulation demo](https://github.com/ZADNE/RealWorld)
- [Meadow: A real-time grass rendering demo](https://github.com/ZADNE/Meadow)

## The following libraries are required to build:

- [SDL2](https://www.libsdl.org/)
- [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)