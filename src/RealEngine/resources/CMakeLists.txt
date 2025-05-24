add_subdirectory(hot_reload)

real_target_sources(RealEngine
    PUBLIC
        FileIO.hpp                  FileIO.cpp
        PackageConstants.hpp        
        ResourceLoader.hpp          ResourceLoader.cpp
        PNGLoader.hpp               PNGLoader.cpp
        ResourceCache.hpp           ResourceCache.cpp
        ResourceID.hpp              
        ResourceManager.hpp         ResourceManager.cpp
)
