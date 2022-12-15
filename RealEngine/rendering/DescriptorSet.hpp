/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IDescriptorSet.hpp>

#include <RealEngine/rendering/Pipeline.hpp>
#include <RealEngine/rendering/buffers/Buffer.hpp>
#include <RealEngine/rendering/Renderer.hpp>

namespace RE {

/**
 * @brief Controls how vertices are rendered to screen.
 * @tparam R The renderer that will perform the commands
*/
template<Renderer R = RendererLateBind>
class DescriptorSet {
    friend class VK13Fixture;
public:

    DescriptorSet(const Pipeline<R>& pl);

    DescriptorSet(const DescriptorSet<R>&) = delete;
    DescriptorSet(DescriptorSet<R>&& other) noexcept;

    DescriptorSet<R>& operator=(const DescriptorSet<R>&) = delete;
    DescriptorSet<R>& operator=(DescriptorSet<R>&& other) noexcept;

    ~DescriptorSet();

    void write(vk::DescriptorType type, uint32_t binding, const Buffer<R>& bf, vk::DeviceSize offset, vk::DeviceSize range);

    void bind(vk::PipelineBindPoint bindPoint, const Pipeline<R>& pl) const;

private:

    DescriptorSetID m_id;

    static inline R::DescriptorSet* s_impl = nullptr;
};

}
