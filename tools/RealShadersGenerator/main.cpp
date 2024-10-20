/*!
 *  @author    Dubsky Tomas
 */
#include <spirv_glsl.hpp>

#include <RealShadersGenerator/Arguments.hpp>
#include <RealShadersGenerator/CompilerWrapper.hpp>
#include <RealShadersGenerator/CppGenerator.hpp>

int main(int argc, char* argv[]) {
    using namespace rsg;
    CLIArguments args = parseArguments(argc, argv);
    std::vector<uint32_t> spirv = compileToSpirV(args.inputFile, args.includeDirs);
    generateCppReflection(spirv);
}
