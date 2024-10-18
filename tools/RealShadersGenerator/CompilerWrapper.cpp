/*!
 *  @author    Dubsky Tomas
 */
#include <filesystem>
#include <format>

#include <RealShadersGenerator/CompilerWrapper.hpp>
#include <RealShadersGenerator/Utility.hpp>
#include <shaderc/shaderc.hpp>

namespace std {
namespace fs = filesystem;
}

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
        // Seach in open files
        for (auto& res : m_includeResults) {
            if (res.relName == requestedSource) {
                return &(res.res);
            }
        }
        // File not open yet - find it in disk
        for (const auto& dir : m_dirs) {
            auto path = std::fs::path{dir} / requestedSource;
            if (std::fs::is_regular_file(path)) {
                std::string relName{requestedSource};
                std::string name{path.string()};
                std::string content{}; // TODO
                m_includeResults.emplace_back(
                    shaderc_include_result{
                        .source_name        = name.c_str(),
                        .source_name_length = name.size(),
                        .content            = content.c_str(),
                        .content_length     = content.size()
                    },
                    std::move(relName), std::move(name), std::move(content)
                );
                return &m_includeResults.back().res;
            }
        }
        // File not found...
    }

    void ReleaseInclude([[maybe_unused]] shaderc_include_result* data) override {}

private:
    const std::vector<std::string>& m_dirs;
    std::vector<IncludeResult> m_includeResults;
};

std::vector<uint32_t> compileToSpirV(
    const std::string& inputFile, const std::vector<std::string>& includeDirs
) {
    shaderc::Compiler compiler{};
    shaderc::CompileOptions options{};
    options.SetIncluder(std::make_unique<IncludeResolver>(includeDirs));
    std::string file = std::format("#include <{}>\nvoid main(){}", inputFile);
    shaderc::SpvCompilationResult res = compiler.CompileGlslToSpv(
        file, shaderc_vertex_shader, "$injected_main_file$", "main", options
    );

    if (res.GetCompilationStatus() != shaderc_compilation_status_success) {
        fatalError("Compilation error: {}", res.GetErrorMessage());
    }

    return std::vector<uint32_t>{res.begin(), res.end()};
}

} // namespace rsg
