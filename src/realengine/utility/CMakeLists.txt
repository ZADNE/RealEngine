add_subdirectory(details)

real_target_sources(RealEngine_utility
    PUBLIC
        BuildType.cppm              
        CompTimeString.cppm         
        DebugString.cppm            
        Endianness.cppm             
        Error.cppm                  Error.cpp
        Math.cppm                   
        OffsetOfArr.hpp             
        SDLProperties.cppm          
        Unicode.cppm                
        UniqueCPtr.cppm             
        Version.cppm                
)
