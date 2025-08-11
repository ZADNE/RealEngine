add_subdirectory(details)

real_target_sources(RealEngine
    PUBLIC
        BuildType.hpp               
        CompTimeString.hpp          
        DebugString.hpp             
        Endianness.hpp              
        Error.hpp                   Error.cpp
        Math.hpp                    
        OffsetOfArr.hpp             
        Unicode.hpp                 
        UniqueCPtr.hpp              
        Version.hpp                 
)
