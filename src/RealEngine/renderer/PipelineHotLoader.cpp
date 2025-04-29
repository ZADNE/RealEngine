/**
 *  @author    Dubsky Tomas
 */
#include <filewatch/FileWatch.hpp>

#include <RealEngine/graphics/pipelines/Pipeline.hpp>
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

std::string cmakeListOfExtensionsToRegex(std::string str) {
    std::replace(str.begin(), str.end(), ';', '|');
    for (size_t i = 0; i < str.size(); ++i) { // Escape dots
        if (str[i] == '.') {
            str.insert(str.begin() + i, '\\');
            ++i;
        }
    }
    return ".*(" + str + ")";
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
              hotReload.binaryDir, std::regex{".*\\.spv\\.bin"},
              [](const std::string& path, const filewatch::Event changeType) {
                  std::cout << "BINARY: " << path << "\n";
              }
          } {}

    DeletionQueue& deletionQueue;
    std::map<vk::Pipeline, PipelineReloadInfo> pipeToReloadInfo;
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
};

PipelineHotLoader::PipelineHotLoader(
    DeletionQueue& deletionQueue, const HotReloadInitInfo& hotReload
)
    : m_impl{std::make_unique<Impl>(deletionQueue, hotReload)} {
}

PipelineHotLoader::~PipelineHotLoader() = default;

void PipelineHotLoader::registerForReloading(
    vk::Pipeline initial, const PipelineGraphicsCreateInfo& createInfo,
    const PipelineGraphicsSources& srcs
) {
    m_impl->pipeToReloadInfo.try_emplace(initial, createInfo, srcs);
}

void PipelineHotLoader::registerForReloading(
    vk::Pipeline initial, const PipelineComputeCreateInfo& createInfo,
    const PipelineComputeSources& srcs
) {
    m_impl->pipeToReloadInfo.try_emplace(initial, createInfo, srcs);
}

vk::Pipeline PipelineHotLoader::hotReload(
    vk::Pipeline current, vk::ShaderStageFlagBits stages
) {
    if (auto it = m_impl->pipeToReloadInfo.find(current);
        it != m_impl->pipeToReloadInfo.end()) {
        // Reload SPIR-V from filesystem
        auto nodeHandle                = m_impl->pipeToReloadInfo.extract(it);
        PipelineReloadInfo& reloadInfo = nodeHandle.mapped();
        reloadInfo.reloadSPIRV(m_impl->binaryDir, stages);

        // Recompile SPIR-V
        vk::Pipeline newPipeline = reloadInfo.recreatePipelineFromSPIRV();

        // Delete old pipeline and return the new one
        m_impl->deletionQueue.enqueueDeletion(nodeHandle.key());
        nodeHandle.key() = newPipeline;
        m_impl->pipeToReloadInfo.insert(std::move(nodeHandle));
        return newPipeline;
    }
    return current;
}

void PipelineHotLoader::unregisterForReloading(vk::Pipeline current) {
    m_impl->pipeToReloadInfo.erase(current);
}

template<typename T>
void PipelineHotLoader::PipelineReloadInfo::reloadSPIRV(
    const std::string& binaryDir, vk::ShaderStageFlagBits stages
) {
    std::vector<unsigned char> temp;
    for (size_t i = 0; i < T::k_numStages; ++i) {
        vk::ShaderStageFlagBits thisStage = T::stageFlags(i);
        if ((thisStage & stages) && m_sources[i].relPath) {
            readBinaryFile(
                std::format(
                    "{}/{}.{}", binaryDir,
                    static_cast<const char*>(m_sources[i].relPath),
                    details::k_shaderSPIRVBinFileExt
                ),
                temp
            );
            assert((temp.size() % sizeof(uint32_t)) == 0);
            m_sources[i].vk13.assign(
                reinterpret_cast<const uint32_t*>(temp.data()), temp.size() / 4
            );
        }
    }
}

vk::Pipeline PipelineHotLoader::PipelineReloadInfo::recreatePipelineFromSPIRV() const {
    switch (m_type) {
    case PipelineType::Graphics:
        return Pipeline::create(
            m_graphics,
            PipelineGraphicsSources{
                .vert = m_sources[0],
                .tesc = m_sources[1],
                .tese = m_sources[2],
                .geom = m_sources[3],
                .frag = m_sources[4]
            }
        );
    case PipelineType::Compute:
        return Pipeline::create(
            m_compute, PipelineComputeSources{.comp = m_sources[0]}
        );
    default: std::unreachable();
    }
}

} // namespace re
