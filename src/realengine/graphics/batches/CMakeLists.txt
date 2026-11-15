add_subdirectory(shaders)

real_target_sources(RealEngine_utility
    PUBLIC
        GeometryBatch.cppm          GeometryBatch.cpp
        Sprite.cppm                 Sprite.cpp
        SpriteBatch.cppm            SpriteBatch.cpp
)
