add_subdirectory(hot_reload)

real_target_sources(RealEngine_utility
    PUBLIC
        FileIO.cppm                 FileIO.cpp
        PackageConstants.cppm       
        ResourceLoader.cppm         ResourceLoader.cpp
        PNGLoader.cppm              PNGLoader.cpp
        ResourceCache.cppm          ResourceCache.cpp
        ResourceID.cppm             
        ResourceManager.cppm        ResourceManager.cpp
)
