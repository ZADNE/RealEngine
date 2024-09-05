/*!
 *  @author    Dubsky Tomas
 */
#include <bitset>

#include <RealEngine/renderer/PhysDeviceSuitability.hpp>
#include <RealEngine/rooms/RoomDisplaySettings.hpp>
#include <RealEngine/utility/Error.hpp>

namespace re {

namespace {

bool areExtensionsSupported(vk::PhysicalDevice physicalDevice) {
    std::bitset<k_deviceExtensions.size()> supported{};
    for (const auto& ext : physicalDevice.enumerateDeviceExtensionProperties()) {
        for (size_t i = 0; i < k_deviceExtensions.size(); ++i) {
            if (std::strcmp(ext.extensionName.data(), k_deviceExtensions[i]) == 0) {
                supported[i] = true;
            }
        }
    }
    return supported.all();
}

bool isSwapchainSupported(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) {
    bool formatSupported = false;
    for (const auto& format : physicalDevice.getSurfaceFormatsKHR(surface)) {
        if (format == k_surfaceFormat) {
            formatSupported = true;
            break;
        }
    }
    return formatSupported;
}

bool findQueueFamilyIndices(
    const vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR surface,
    uint32_t& graphicsCompQueueFamIndex, uint32_t& presentationQueueFamIndex
) {
    constexpr static vk::QueueFlags k_graphicsTransfer =
        vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer;

    bool graphicsCompQueueFound = false;
    bool presentQueueFound      = false;
    std::vector<vk::QueueFamilyProperties> queueFamProperties =
        physicalDevice.getQueueFamilyProperties();

    // Iterate over all queue families
    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamProperties.size()); i++) {
        if ((queueFamProperties[i].queueFlags & k_graphicsTransfer) ==
            k_graphicsTransfer) {
            graphicsCompQueueFamIndex = i;
            graphicsCompQueueFound    = true;
        }
        if (physicalDevice.getSurfaceSupportKHR(i, surface)) {
            presentationQueueFamIndex = i;
            presentQueueFound         = true;
        }
        if (graphicsCompQueueFound && presentQueueFound) {
            return true;
        }
    }
    return false;
}

size_t structSizeBytes(vk::StructureType type) {
    switch (type) {
    case vk::StructureType::ePhysicalDeviceFeatures2:
        return sizeof(vk::PhysicalDeviceFeatures2);
    case vk::StructureType::ePhysicalDeviceVulkan11Features:
        return sizeof(vk::PhysicalDeviceVulkan11Features);
    case vk::StructureType::ePhysicalDeviceVulkan12Features:
        return sizeof(vk::PhysicalDeviceVulkan12Features);
    case vk::StructureType::ePhysicalDeviceVulkan13Features:
        return sizeof(vk::PhysicalDeviceVulkan13Features);
    default:
        throw Exception{"Unsupported struct in device create info chain"};
        break;
    }
}

using InStruct = const vk::BaseInStructure;
InStruct* toInStruct(const void* ptr) {
    return reinterpret_cast<InStruct*>(ptr);
}
using OutStruct = vk::BaseOutStructure;
OutStruct* toOutStruct(std::byte* ptr) {
    return reinterpret_cast<OutStruct*>(ptr);
}

// NOLINTBEGIN(*-c-arrays)
std::unique_ptr<std::byte[]> copyPhysDeviceCreateInfoChain(const void* chain) {
    assert(chain);
    assert(toInStruct(chain)->sType == vk::StructureType::ePhysicalDeviceFeatures2);

    // Count number of bytes required
    size_t totalSizeBytes = 0;
    for (InStruct* s = toInStruct(chain); s; s = s->pNext) {
        totalSizeBytes += structSizeBytes(s->sType);
    }

    // Allocate the buffer
    assert(totalSizeBytes > 0);
    auto buf = std::make_unique_for_overwrite<std::byte[]>(totalSizeBytes);

    // Initialize the chain
    size_t offsetBytes = 0;
    for (InStruct* s = toInStruct(chain); s; s = s->pNext) {
        OutStruct* out = toOutStruct(&buf[offsetBytes]);
        out->sType     = s->sType;
        size_t mySize  = structSizeBytes(s->sType);
        out->pNext = s->pNext ? toOutStruct(&buf[offsetBytes + mySize]) : nullptr;
        offsetBytes += mySize;
    }

    return buf;
}
// NOLINTEND(*-c-arrays)

int findPreferred(
    std::span<const vk::PhysicalDevice> physDevices, std::string_view preferredDevice
) {
    if (!preferredDevice.empty()) {
        for (int i = 0; i < static_cast<int>(physDevices.size()); ++i) {
            const auto& physDev = physDevices[i];
            auto props          = physDev.getProperties2().properties;
            std::string_view name{props.deviceName};
            if (name == preferredDevice) {
                return i;
            }
        }
    }
    return -1;
}

SelectedPhysDevice isSuitable(
    vk::PhysicalDevice physDevice, const PhysDeviceRequirements& req, std::byte* testChain
) {
    // Check basic necessities
    uint32_t graphicsCompQueueFamIndex{};
    uint32_t presentationQueueFamIndex{};
    if (!areExtensionsSupported(physDevice) ||
        !isSwapchainSupported(physDevice, req.surface) ||
        !findQueueFamilyIndices(
            physDevice, req.surface, graphicsCompQueueFamIndex, presentationQueueFamIndex
        )) {
        return SelectedPhysDevice{};
    }

    { // Check Vulkan version support
        auto props = physDevice.getProperties2().properties;
        auto major = vk::apiVersionMajor(props.apiVersion);
        auto minor = vk::apiVersionMinor(props.apiVersion);
        if (major != 1 || minor < 3) {
            return SelectedPhysDevice{};
        }
    }

    { // Get what is available
        auto* features = reinterpret_cast<vk::PhysicalDeviceFeatures2*>(testChain);
        physDevice.getFeatures2(features);
    }

    // Test all features in all structs of the chain
    InStruct* desired = toInStruct(req.deviceCreateInfoChain);
    for (InStruct* avail = toInStruct(testChain); avail;
         avail = avail->pNext, desired = desired->pNext) {
        size_t sizeBytes = structSizeBytes(avail->sType);
        size_t featureCount = (sizeBytes - sizeof(InStruct)) / sizeof(vk::Bool32);
        const vk::Bool32* desBool = reinterpret_cast<const vk::Bool32*>(&desired[1]);
        const vk::Bool32* avaBool = reinterpret_cast<const vk::Bool32*>(&avail[1]);

        // All desired features must be available
        for (size_t i = 0; i < featureCount; ++i) {
            if (desBool[i] > avaBool[i]) {
                return SelectedPhysDevice{};
            }
        }
    }

    // All checks passed, device is suitable
    return SelectedPhysDevice{
        physDevice, graphicsCompQueueFamIndex, presentationQueueFamIndex
    };
}

} // namespace

SelectedPhysDevice selectSuitablePhysDevice(
    vk::Instance instance, const PhysDeviceRequirements& req
) {
    // Create a copy of the chain to test the features
    auto testChain = copyPhysDeviceCreateInfoChain(req.deviceCreateInfoChain);

    // Enumerate physical devices
    std::vector<vk::PhysicalDevice> physDevices = instance.enumeratePhysicalDevices();

    // First try preferred device
    int prefI = findPreferred(physDevices, req.preferredDevice);
    if (prefI >= 0) {
        if (auto res = isSuitable(physDevices[prefI], req, testChain.get()); res) {
            return res; // Preferred device is suitable
        }
    }

    // Prefererred was not suitable - try other ones
    for (size_t i = 0; i < physDevices.size(); ++i) {
        if (i != prefI) {
            if (auto res = isSuitable(physDevices[i], req, testChain.get()); res) {
                return res; // Found a suitable device
            }
        }
    }

    return SelectedPhysDevice{}; // No device is suitable
}

} // namespace re
