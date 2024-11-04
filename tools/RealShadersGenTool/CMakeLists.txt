real_target_sources(RealShadersGenTool
    PRIVATE 
        Arguments.hpp               Arguments.cpp
        Common.hpp                  
        CompilerWrapper.hpp         CompilerWrapper.cpp
        CppGenerator.hpp            CppGenerator.cpp
        InterfaceBlockReflection.hpp
                                    main.cpp
        ReflectionAnalysis.hpp      ReflectionAnalysis.cpp
        Utility.hpp                 Utility.cpp
)
