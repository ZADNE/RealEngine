add_subdirectory(graphics)
add_subdirectory(program)
add_subdirectory(renderer)
add_subdirectory(resources)
add_subdirectory(rooms)
add_subdirectory(user_input)
add_subdirectory(utility)
add_subdirectory(window)

real_target_sources(RealEngine_utility
    PUBLIC
        RealEngine.cppm             
)