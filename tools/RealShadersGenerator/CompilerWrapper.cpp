/*!
 *  @author    Dubsky Tomas
 */
#include <filesystem>
#include <format>
#include <fstream>

#include <RealShadersGenerator/CompilerWrapper.hpp>
#include <RealShadersGenerator/Utility.hpp>
#include <shaderc/shaderc.hpp>

namespace std {
namespace fs = filesystem;
}

namespace {
std::string readWholeFile(const std::fs::path& fullPath) {
    size_t size = static_cast<size_t>(std::fs::file_size(fullPath));
    std::string content(size, '\0');
    std::ifstream file(fullPath, std::ios::binary);
    file.read(content.data(), size);
    return content;
}
} // namespace

namespace rsg {

struct IncludeResult {
    shaderc_include_result res;
    std::string relName;
    std::string name;
    std::string content;
};

class IncludeResolver: public shaderc::CompileOptions::IncluderInterface {
public:
    IncludeResolver(const std::vector<std::string>& dirs)
        : m_dirs(dirs) {}

    shaderc_include_result* GetInclude(
        const char* requestedSource, shaderc_include_type type,
        const char* requestingSource, size_t includeDepth
    ) override {
        std::string requested{requestedSource};
        // Search in already read files
        for (auto& res : m_includeResults) {
            if (res.relName == requested) {
                return &(res.res);
            }
        }
        // File not read yet - find it and read it
        for (const auto& dir : m_dirs) {
            auto fullPath = std::fs::path{dir} / requested;
            if (std::fs::is_regular_file(fullPath)) {
                std::string name{fullPath.string()};
                std::string content = readWholeFile(fullPath);
                m_includeResults.emplace_back(
                    shaderc_include_result{
                        .source_name        = name.c_str(),
                        .source_name_length = name.size(),
                        .content            = content.c_str(),
                        .content_length     = content.size()
                    },
                    std::move(requested), std::move(name), std::move(content)
                );
                return &m_includeResults.back().res;
            }
        }
        // File not found...
        std::string errorMessage = std::format("File '{}' not found", requestedSource);
        m_includeResults.emplace_back(
            shaderc_include_result{
                .source_name        = "",
                .source_name_length = 0,
                .content            = errorMessage.c_str(),
                .content_length     = errorMessage.size()
            },
            std::string{}, std::string{}, std::move(errorMessage)
        );
        return &m_includeResults.back().res;
    }

    void ReleaseInclude([[maybe_unused]] shaderc_include_result* data) override {}

private:
    const std::vector<std::string>& m_dirs;
    std::vector<IncludeResult> m_includeResults;
};

std::vector<uint32_t> compileToSpirV(
    const std::string& inputFile, const std::vector<std::string>& includeDirs
) {
    // Prepare compiler inputs
    shaderc::CompileOptions options{};
    options.SetIncluder(std::make_unique<IncludeResolver>(includeDirs));
    options.SetTargetEnvironment(
        shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3
    );
    options.SetGenerateDebugInfo();
    options.SetOptimizationLevel(shaderc_optimization_level_zero);
    options.SetForcedVersionProfile(460, shaderc_profile_core);
    std::string content = readWholeFile(inputFile) + "\nvoid main(){}";

    // Compile
    shaderc::Compiler compiler{};
    shaderc::SpvCompilationResult res = compiler.CompileGlslToSpv(
        content, shaderc_vertex_shader, inputFile.c_str(), "main", options
    );
    if (res.GetCompilationStatus() != shaderc_compilation_status_success) {
        fatalError("Compilation error: {}", res.GetErrorMessage());
    }
    return std::vector<uint32_t>{res.begin(), res.end()};
}

} // namespace rsg
