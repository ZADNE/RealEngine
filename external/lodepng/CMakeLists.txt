add_library(lodepng STATIC)
set_property(TARGET lodepng PROPERTY COMPILE_WARNING_AS_ERROR OFF)

target_sources(lodepng PRIVATE
    lodepng.hpp                 lodepng.cpp
)
