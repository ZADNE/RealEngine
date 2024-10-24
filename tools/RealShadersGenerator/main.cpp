/*!
 *  @author    Dubsky Tomas
 */
#include <RealShadersGenerator/Arguments.hpp>
#include <RealShadersGenerator/CompilerWrapper.hpp>
#include <RealShadersGenerator/CppGenerator.hpp>
#include <RealShadersGenerator/ReflectionAnalysis.hpp>

int main(int argc, char* argv[]) {
    using namespace rsg;
    CLIArguments args = parseArguments(argc, argv);

    std::vector<uint32_t> spirv = compileToSpirV(args.inputFile, args.includeDirs);

    InterfaceBlockReflection reflection = reflectInterfaceBlock(
        spirv, std::filesystem::path{args.inputFile}.stem().string()
    );

    generateCppFile(reflection, args.namespace_, std::filesystem::path{args.outputFile});
}
