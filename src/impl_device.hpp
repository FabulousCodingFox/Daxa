#pragma once

#include <daxa/daxa.hpp>
#include <daxa/device.hpp>

#include "impl_core.hpp"
#include "impl_context.hpp"

#include "impl_pipeline.hpp"
#include "impl_command_list.hpp"
#include "impl_swapchain.hpp"
#include "impl_semaphore.hpp"
#include "impl_gpu_resources.hpp"
#include "impl_split_barrier.hpp"
#include "impl_timeline_query.hpp"

namespace daxa
{
    struct ImplDevice final : ManagedSharedState
    {
        ManagedWeakPtr impl_ctx = {};
        VkPhysicalDevice vk_physical_device = {};
        VkDevice vk_device = {};

        PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;

        PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
        PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
        PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
        PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
        PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
        PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
        PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
        PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
        PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
        PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;

        VmaAllocator vma_allocator = {};
        DeviceProperties vk_info = {};
        DeviceInfo info = {};
        VkSampler vk_null_sampler = {};
        VkBuffer buffer_device_address_buffer = {};
        u64 * buffer_device_address_buffer_host_ptr = {};
        VmaAllocation buffer_device_address_buffer_allocation = {};

        // Gpu resource table:
        GPUShaderResourceTable gpu_shader_resource_table = {};

        // Resource recycling:
        DAXA_ONLY_IF_THREADSAFETY(std::mutex main_queue_command_pool_buffer_recycle_mtx = {});
        CommandBufferPoolPool buffer_pool_pool = {};
        // Main queue:
        VkQueue main_queue_vk_queue = {};
        u32 main_queue_family_index = {};

        DAXA_ATOMIC_U64 main_queue_cpu_timeline = {};
        VkSemaphore vk_main_queue_gpu_timeline_semaphore = {};

        DAXA_ONLY_IF_THREADSAFETY(std::mutex main_queue_zombies_mtx = {});
        std::deque<std::pair<u64, std::vector<ManagedPtr>>> main_queue_submits_zombies = {};
        std::deque<std::pair<u64, CommandListZombie>> main_queue_command_list_zombies = {};
        std::deque<std::pair<u64, BufferId>> main_queue_buffer_zombies = {};
        std::deque<std::pair<u64, ImageId>> main_queue_image_zombies = {};
        std::deque<std::pair<u64, ImageViewId>> main_queue_image_view_zombies = {};
        std::deque<std::pair<u64, SamplerId>> main_queue_sampler_zombies = {};
        std::deque<std::pair<u64, AccelerationStructureId>> main_queue_acceleration_structure_zombies = {};
        std::deque<std::pair<u64, SemaphoreZombie>> main_queue_semaphore_zombies = {};
        std::deque<std::pair<u64, SplitBarrierZombie>> main_queue_split_barrier_zombies = {};
        std::deque<std::pair<u64, PipelineZombie>> main_queue_pipeline_zombies = {};
        std::deque<std::pair<u64, TimelineQueryPoolZombie>> main_queue_timeline_query_pool_zombies = {};
        void main_queue_collect_garbage();
        void wait_idle() const;

        ImplDevice(DeviceInfo info, ManagedWeakPtr impl_ctx, VkPhysicalDevice physical_device);
        virtual ~ImplDevice() override final;

        auto validate_image_slice(ImageMipArraySlice const & slice, ImageId id) -> ImageMipArraySlice;
        auto validate_image_slice(ImageMipArraySlice const & slice, ImageViewId id) -> ImageMipArraySlice;

        auto new_buffer(BufferInfo const & buffer_info) -> BufferId;
        auto new_swapchain_image(VkImage swapchain_image, VkFormat format, u32 index, ImageUsageFlags usage, ImageInfo const & info) -> ImageId;
        auto new_image(ImageInfo const & image_info) -> ImageId;
        auto new_image_view(ImageViewInfo const & image_view_info) -> ImageViewId;
        auto new_sampler(SamplerInfo const & sampler_info) -> SamplerId;
        auto new_acceleration_structure(AccelerationStructureInfo const & acceleration_structure_info) -> AccelerationStructureId;

        auto slot(BufferId id) -> ImplBufferSlot &;
        auto slot(ImageId id) -> ImplImageSlot &;
        auto slot(ImageViewId id) -> ImplImageViewSlot &;
        auto slot(SamplerId id) -> ImplSamplerSlot &;
        auto slot(AccelerationStructureId id) -> ImplAccelerationStructureSlot &;

        auto slot(BufferId id) const -> ImplBufferSlot const &;
        auto slot(ImageId id) const -> ImplImageSlot const &;
        auto slot(ImageViewId id) const -> ImplImageViewSlot const &;
        auto slot(SamplerId id) const -> ImplSamplerSlot const &;
        auto slot(AccelerationStructureId id) const -> ImplAccelerationStructureSlot const &;

        void zombify_buffer(BufferId id);
        void zombify_image(ImageId id);
        void zombify_image_view(ImageViewId id);
        void zombify_sampler(SamplerId id);
        void zombify_acceleration_structure(AccelerationStructureId id);

        void cleanup_buffer(BufferId id);
        void cleanup_image(ImageId id);
        void cleanup_image_view(ImageViewId id);
        void cleanup_sampler(SamplerId id);
        void cleanup_acceleration_structure(AccelerationStructureId id);
    };
} // namespace daxa
