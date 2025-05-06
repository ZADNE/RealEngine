/**
 *  @author    Dubsky Tomas
 */
#include <set>

#include <filewatch/FileWatch.hpp>

#include <RealEngine/graphics/pipelines/Pipeline.hpp>
#include <RealEngine/graphics/synchronization/DoubleBuffered.hpp>
#include <RealEngine/renderer/PipelineHotLoader.hpp>
#include <RealEngine/resources/FileIO.hpp>
#include <RealEngine/utility/details/CMakeConstants.hpp>

namespace re {

namespace {

const char* nulFile() {
    if constexpr (k_buildOS == BuildOS::Windows) {
        return "nul";
    } else if constexpr (k_buildOS == BuildOS::Linux) {
        return "/dev/null";
    } else {
        std::unreachable();
    }
}

constexpr void escapeDots(std::string& str) {
    for (size_t i = 0; i < str.size(); ++i) { // Escape dots
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
              hotReload.targetSourceDir,
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
            if (lastSourceChange.load() > lastCMakeExecution) {
                lastCMakeExecution = std::chrono::steady_clock::now();
                std::system(recompileShadersCommand.c_str());
            }

            if (std::chrono::steady_clock::now() > lastIteration + 1s) {
                std::this_thread::sleep_until(lastIteration + 1s);
                lastIteration = std::chrono::steady_clock::now();
            }
        }
    }};

    using TimePoint = std::chrono::steady_clock::time_point;
    using Duration  = std::chrono::steady_clock::duration;
    std::atomic<TimePoint> lastSourceChange;
    std::atomic<TimePoint> lastCMakeRun;

    auto findInRegister(vk::Pipeline& pipeline) {
        return std::find_if(
            pipelineRegister.begin(), pipelineRegister.end(),
            [&](const PipelineReloadInfo& info) -> bool {
                return info.targetsPipeline(pipeline);
            }
        );
    }
};

PipelineHotLoader::PipelineHotLoader(
    DeletionQueue& deletionQueue, const HotReloadInitInfo& hotReload
)
    : m_impl{std::make_unique<Impl>(deletionQueue, hotReload)} {
}

PipelineHotLoader::~PipelineHotLoader() = default;

void PipelineHotLoader::registerForReloading(
    vk::Pipeline& initial, const PipelineGraphicsCreateInfo& createInfo,
    const PipelineGraphicsSources& srcs
) {
    m_impl->pipelineRegister.emplace_back(initial, createInfo, srcs);
}

void PipelineHotLoader::registerForReloading(
    vk::Pipeline& initial, const PipelineComputeCreateInfo& createInfo,
    const PipelineComputeSources& srcs
) {
    m_impl->pipelineRegister.emplace_back(initial, createInfo, srcs);
}

void PipelineHotLoader::moveRegisteredPipeline(
    vk::Pipeline& original, vk::Pipeline& moved
) {
    auto it = m_impl->findInRegister(original);
    if (it != m_impl->pipelineRegister.end()) {
        it->updateTargetPipeline(moved);
    }
}

void PipelineHotLoader::reloadChangedPipelines() {
    auto& paths = m_impl->pathsToReload.read();
    std::set<PipelineReloadInfo*> pipelinesToRecompile;
    std::vector<unsigned char> loadedFile;
    for (const auto& path : paths) { // For each modified source
        // Load the SPIRV
        loadedFile = readBinaryFile(m_impl->binaryDir + '/' + path);
        assert((loadedFile.size() % sizeof(uint32_t)) == 0);

        // Search all pipelines that use the source file
        for (PipelineReloadInfo& info : m_impl->pipelineRegister) {
            // Search all stages of the pipeline
            for (ShaderSource& source : info.sources()) {
                const char* sourcePath = source.relPath;
                if (sourcePath && sourcePath == path) { // TODO: file extension
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
    }
    paths.clear();

    // Recreate all affected pipelines
    for (const auto& info : pipelinesToRecompile) {
        info->recreatePipelineFromSources(m_impl->deletionQueue);
    }
}

void PipelineHotLoader::unregisterForReloading(vk::Pipeline& current) {
    auto it = m_impl->findInRegister(current);
    if (it != m_impl->pipelineRegister.end()) {
        m_impl->pipelineRegister.erase(it); // O(n) shift
    }
}

void PipelineHotLoader::PipelineReloadInfo::recreatePipelineFromSources(
    DeletionQueue& deletionQueue
) const {
    deletionQueue.enqueueDeletion(*m_pipeline);
    switch (m_type) {
    case PipelineType::Graphics:
        *m_pipeline = Pipeline::create(
            m_graphics,
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
            m_compute, PipelineComputeSources{.comp = m_sources[0]}
        );
        break;
    default: break;
    }
}

} // namespace re
