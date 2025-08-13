add_subdirectory(shaders)

real_target_sources(RealEngine_graphics
    PUBLIC
        GeometryBatch.hpp           GeometryBatch.cpp
        Sprite.hpp                  Sprite.cpp
        SpriteBatch.hpp             SpriteBatch.cpp
)
