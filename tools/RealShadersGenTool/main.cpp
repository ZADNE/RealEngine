/*!
 *  @author    Dubsky Tomas
 */
#include <RealShadersGenTool/Arguments.hpp>
#include <RealShadersGenTool/CompilerWrapper.hpp>
#include <RealShadersGenTool/CppGenerator.hpp>
#include <RealShadersGenTool/ReflectionAnalysis.hpp>

int main(int argc, char* argv[]) {
    using namespace rsg;
    CLIArguments args = parseArguments(argc, argv);

    std::vector<uint32_t> spirv = compileToSpirV(args.inputFile, args.includeDirs);

    InterfaceBlockReflection reflection =
        reflectInterfaceBlock(spirv, args.blockType, args.blockName);

    generateCppFile(reflection, args.namespace_, std::filesystem::path{args.outputFile});
}
