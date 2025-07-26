/**
 *  @author    Dubsky Tomas
 */
#include <set>

#include <filewatch/FileWatch.hpp>

#include <RealEngine/graphics/pipelines/Pipeline.hpp>
#include <RealEngine/graphics/synchronization/DoubleBuffered.hpp>
#include <RealEngine/program/MainProgram.hpp>
#include <RealEngine/resources/FileIO.hpp>
#include <RealEngine/resources/hot_reload/PipelineHotLoader.hpp>
#include <RealEngine/utility/details/CMakeConstants.hpp>

namespace re {

namespace {

const char* nulFile() {
#if RE_BUILDING_FOR_WINDOWS
    return "nul";
#elif RE_BUILDING_FOR_LINUX // ^^^ RE_BUILDING_FOR_WINDOWS
    return "/dev/null";
#else
#    error "Unhandled OS"
#endif
}

using StrDiffType = std::string::difference_type;

constexpr void escapeDots(std::string& str) {
    for (StrDiffType i = 0; i < static_cast<StrDiffType>(str.size()); ++i) {
        if (str[i] == '.') {
            str.insert(str.begin() + i, '\\');
            ++i;
        }
    }
}

std::string cmakeListOfExtensionsToRegex(std::string str) {
    std::replace(str.begin(), str.end(), ';', '|');
    escapeDots(str);
    return ".*(" + str + ")$";
}

constexpr std::string spirvBinFileRegex() {
    auto str = std::string{"."} + details::k_shaderSPIRVBinFileExt;
    escapeDots(str);
    return ".*" + str + '$';
}

consteval StrDiffType strLength(const char* str) {
    StrDiffType len = 0;
    while (*str != '\0') {
        str++;
        len++;
    }
    return len;
}

} // namespace

using namespace std::chrono_literals;

struct PipelineHotLoader::Impl {
    Impl(DeletionQueue& deletionQueue, const HotReloadInitInfo& hotReload)
        : deletionQueue{deletionQueue}
        , recompileShadersCommand{std::format(
              "\"\"{}\" --build \"{}\" -t \"{}{}\" > {}\"", hotReload.cmakePath,
              hotReload.binaryDir, hotReload.targetName,
              details::k_shaderTargetSuffix, nulFile()
          )}
        , binaryDir{hotReload.binaryDir}
        , sourceDirWatch(
              hotReload.sourceDir,
              std::regex{cmakeListOfExtensionsToRegex(hotReload.shaderFileExtensions)},
              [this](const std::string& path, const filewatch::Event changeType) {
                  lastSourceChange = std::chrono::steady_clock::now();
              }
          )
        , binaryDirWatch{
              hotReload.binaryDir, std::regex{spirvBinFileRegex()},
              [this](const std::string& path, const filewatch::Event changeType) {
                  pathsToReload.write().emplace(path);
              }
          } {}

    DeletionQueue& deletionQueue;
    std::vector<PipelineReloadInfo> pipelineRegister;
    FrameDoubleBuffered<std::set<std::string>> pathsToReload;
    std::string recompileShadersCommand;
    std::string binaryDir;
    filewatch::FileWatch<std::string> sourceDirWatch;
    filewatch::FileWatch<std::string> binaryDirWatch;
    std::jthread cmakeThread{[this](std::stop_token stopToken) {
        TimePoint lastCMakeExecution = std::chrono::steady_clock::now();
        TimePoint lastIteration      = lastCMakeExecution;
        while (!stopToken.stop_requested()) {
            TimePoint locLastSourceChange = lastSourceChange.load();
            if (locLastSourceChange > lastCMakeExecution) {
                // Ensure at least a second has passed since the source edit to
                // stabilize the filesystem and avoid duplicate runs
                if (std::chrono::steady_clock::now() < locLastSourceChange + 1s) {
                    std::this_thread::sleep_until(locLastSourceChange + 1s);
                }
                // Run CMake
                lastCMakeExecution = std::chrono::steady_clock::now();
                if (std::system(recompileShadersCommand.c_str()) > 0) {
                    re::error("Shader compilation failed");
                }
            }

            // Check at most once a second to avoid active waiting
            if (std::chrono::steady_clock::now() < lastIteration + 1s) {
                std::this_thread::sleep_until(lastIteration + 1s);
            }
            lastIteration = std::chrono::steady_clock::now();
        }
    }};

    using TimePoint = std::chrono::steady_clock::time_point;
    using Duration  = std::chrono::steady_clock::duration;
    std::atomic<TimePoint> lastSourceChange;
    std::atomic<TimePoint> lastCMakeRun;

    auto findInRegister(vk::Pipeline pipeline) {
        return std::find_if(
            pipelineRegister.begin(), pipelineRegister.end(),
            [&](const PipelineReloadInfo& info) -> bool {
                return info.targetsPipeline(pipeline);
            }
        );
    }
};

PipelineHotLoader::PipelineHotLoader(
    DeletionQueue& deletionQueue, const HotReloadInitInfo* hotReload
)
    : m_impl{[&]() -> std::unique_ptr<Impl> {
        if (hotReload) {
            return std::make_unique<Impl>(deletionQueue, *hotReload);
        } else {
            re::log("Hot reload of shaders disabled: init info not provided");
            return nullptr;
        }
    }()} {
}

PipelineHotLoader::~PipelineHotLoader() = default;

void PipelineHotLoader::registerPipelineForReloading(
    vk::Pipeline& initial, const PipelineGraphicsCreateInfo& createInfo,
    const PipelineGraphicsSources& srcs
) {
    if (!m_impl)
        return;
    m_impl->pipelineRegister.emplace_back(initial, createInfo, srcs);
}

void PipelineHotLoader::registerPipelineForReloading(
    vk::Pipeline& initial, const PipelineComputeCreateInfo& createInfo,
    const PipelineComputeSources& srcs
) {
    if (!m_impl)
        return;
    m_impl->pipelineRegister.emplace_back(initial, createInfo, srcs);
}

void PipelineHotLoader::moveRegisteredPipeline(
    vk::Pipeline& original, vk::Pipeline& moved
) {
    if (!m_impl)
        return;
    auto it = m_impl->findInRegister(original);
    if (it != m_impl->pipelineRegister.end()) {
        it->updateTargetPipeline(moved);
    }
}

void PipelineHotLoader::setPipelineIdentifier(vk::Pipeline pipeline, int identifier) {
    if (!m_impl)
        return;
    auto it = m_impl->findInRegister(pipeline);
    if (it != m_impl->pipelineRegister.end()) {
        it->setIdentifier(identifier);
    }
}

size_t PipelineHotLoader::reloadChangedPipelines(
    const std::function<void(vk::Pipeline, int)>& reloadedCallback
) {
    if (!m_impl)
        return 0;
    auto& binPaths = m_impl->pathsToReload.read();
    std::set<PipelineReloadInfo*> pipelinesToRecompile;
    std::vector<unsigned char> loadedFile;
    for (const auto& binPath : binPaths) { // For each modified source
        try {
            // Load the SPIRV
            loadedFile = readBinaryFile(m_impl->binaryDir + '/' + binPath);
            assert((loadedFile.size() % sizeof(uint32_t)) == 0);

            // Search all pipelines that use the source file
            std::string path{
                binPath.begin(),
                binPath.end() - strLength(details::k_shaderSPIRVBinFileExt) - 1 // 1 for '.'
            };
            for (PipelineReloadInfo& info : m_impl->pipelineRegister) {
                // Search all stages of the pipeline
                auto sources = info.sources();
                for (ShaderSource& source : sources) {
                    const char* sourcePath = source.relPath;
                    if (sourcePath && sourcePath == path) {
                        // Matching path - replace SPIRV
                        source.vk13.assign(
                            reinterpret_cast<const uint32_t*>(loadedFile.data()),
                            loadedFile.size() / 4
                        );
                        pipelinesToRecompile.emplace(&info);
                        // The same source cannot be used in multiple stages of pipeline
                        break;
                    }
                }
            }
        } catch (...) {}
    }
    binPaths.clear();

    // Recreate all affected pipelines
    for (const auto& info : pipelinesToRecompile) {
        info->recreatePipelineFromSources(m_impl->deletionQueue);
        reloadedCallback(info->targetPipeline(), info->indentifier());
    }

    return pipelinesToRecompile.size();
}

void PipelineHotLoader::unregisterPipelineForReloading(vk::Pipeline& pipeline) {
    if (!m_impl)
        return;
    auto it = m_impl->findInRegister(pipeline);
    if (it != m_impl->pipelineRegister.end()) {
        m_impl->pipelineRegister.erase(it); // O(n) shift
    }
}

bool PipelineHotLoader::PipelineReloadInfo::recreatePipelineFromSources(
    DeletionQueue& deletionQueue
) const {
    try {
        vk::Pipeline original = *m_pipeline;
        switch (m_type) {
        case PipelineType::Graphics:
            *m_pipeline = Pipeline::create(
                m_graphicsCreateInfo,
                PipelineGraphicsSources{
                    .vert = m_sources[0],
                    .tesc = m_sources[1],
                    .tese = m_sources[2],
                    .geom = m_sources[3],
                    .frag = m_sources[4]
                }
            );
            break;
        case PipelineType::Compute:
            *m_pipeline = Pipeline::create(
                m_computeCreateInfo, PipelineComputeSources{.comp = m_sources[0]}
            );
            break;
        default: break;
        }
        deletionQueue.enqueueDeletion(original);
        return true;
    } catch (...) {}
    return false;
}

std::span<ShaderSource> PipelineHotLoader::PipelineReloadInfo::sources() {
    size_t count = 0;
    switch (m_type) {
    case PipelineType::Graphics:
        count = PipelineGraphicsSources::k_numStages;
        break;
    case PipelineType::Compute:
        count = PipelineComputeSources::k_numStages;
        break;
    default: break;
    }
    return {m_sources.begin(), m_sources.begin() + count};
}

} // namespace re
