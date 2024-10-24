real_target_sources(RealShadersGenerator
    PRIVATE 
        Arguments.hpp               Arguments.cpp
        CompilerWrapper.hpp         CompilerWrapper.cpp
        CppGenerator.hpp            CppGenerator.cpp
        InterfaceBlockReflection.hpp
                                    main.cpp
        ReflectionAnalysis.hpp      ReflectionAnalysis.cpp
        Utility.hpp                 Utility.cpp
)
