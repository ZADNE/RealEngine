add_subdirectory(shaders)

real_target_sources(RealEngine
    PUBLIC
        GeometryBatch.hpp           GeometryBatch.cpp
        Sprite.hpp                  Sprite.cpp
        SpriteBatch.hpp             SpriteBatch.cpp
)
