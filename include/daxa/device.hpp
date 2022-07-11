#pragma once

#include <memory>

#include <daxa/gpu_resources.hpp>
#include <daxa/pipeline.hpp>
#include <daxa/swapchain.hpp>

namespace daxa
{
    enum class DeviceType
    {
        OTHER = 0,
        INTEGRATED_GPU = 1,
        DISCRETE_GPU = 2,
        VIRTUAL_GPU = 3,
        CPU = 4,
    };

    struct DeviceLimits
    {
        u32 max_image_dimension1d;
        u32 max_image_dimension2d;
        u32 max_image_dimension3d;
        u32 max_image_dimension_cube;
        u32 max_image_array_layers;
        u32 max_texel_buffer_elements;
        u32 max_uniform_buffer_range;
        u32 max_storage_buffer_range;
        u32 max_push_constants_size;
        u32 max_memory_allocation_count;
        u32 max_sampler_allocation_count;
        u64 buffer_image_granularity;
        u64 sparse_address_space_size;
        u32 max_bound_descriptor_sets;
        u32 max_per_stage_descriptor_samplers;
        u32 max_per_stage_descriptor_uniform_buffers;
        u32 max_per_stage_descriptor_storage_buffers;
        u32 max_per_stage_descriptor_sampled_images;
        u32 max_per_stage_descriptor_storage_images;
        u32 max_per_stage_descriptor_input_attachments;
        u32 max_per_stage_resources;
        u32 max_descriptor_set_samplers;
        u32 max_descriptor_set_uniform_buffers;
        u32 max_descriptor_set_uniform_buffers_dynamic;
        u32 max_descriptor_set_storage_buffers;
        u32 max_descriptor_set_storage_buffers_dynamic;
        u32 max_descriptor_set_sampled_images;
        u32 max_descriptor_set_storage_images;
        u32 max_descriptor_set_input_attachments;
        u32 max_vertex_input_attributes;
        u32 max_vertex_input_bindings;
        u32 max_vertex_input_attribute_offset;
        u32 max_vertex_input_binding_stride;
        u32 max_vertex_output_components;
        u32 max_tessellation_generation_level;
        u32 max_tessellation_patch_size;
        u32 max_tessellation_control_per_vertex_input_components;
        u32 max_tessellation_control_per_vertex_output_components;
        u32 max_tessellation_control_per_patch_output_components;
        u32 max_tessellation_control_total_output_components;
        u32 max_tessellation_evaluation_input_components;
        u32 max_tessellation_evaluation_output_components;
        u32 max_geometry_shader_invocations;
        u32 max_geometry_input_components;
        u32 max_geometry_output_components;
        u32 max_geometry_output_vertices;
        u32 max_geometry_total_output_components;
        u32 max_fragment_input_components;
        u32 max_fragment_output_attachments;
        u32 max_fragment_dual_src_attachments;
        u32 max_fragment_combined_output_resources;
        u32 max_compute_shared_memory_size;
        u32 max_compute_work_group_count[3];
        u32 max_compute_work_group_invocations;
        u32 max_compute_work_group_size[3];
        u32 sub_pixel_precision_bits;
        u32 sub_texel_precision_bits;
        u32 mipmap_precision_bits;
        u32 max_draw_indexed_index_value;
        u32 max_draw_indirect_count;
        f32 max_sampler_lod_bias;
        f32 max_sampler_anisotropy;
        u32 max_viewports;
        u32 max_viewport_dimensions[2];
        f32 viewport_bounds_range[2];
        u32 viewport_sub_pixel_bits;
        usize min_memory_map_alignment;
        u64 min_texel_buffer_offset_alignment;
        u64 min_uniform_buffer_offset_alignment;
        u64 min_storage_buffer_offset_alignment;
        i32 min_texel_offset;
        u32 max_texel_offset;
        i32 min_texel_gather_offset;
        u32 max_texel_gather_offset;
        f32 min_interpolation_offset;
        f32 max_interpolation_offset;
        u32 sub_pixel_interpolation_offset_bits;
        u32 max_framebuffer_width;
        u32 max_framebuffer_height;
        u32 max_framebuffer_layers;
        u32 framebuffer_color_sample_counts;
        u32 framebuffer_depth_sample_counts;
        u32 framebuffer_stencil_sample_counts;
        u32 framebuffer_no_attachments_sample_counts;
        u32 max_color_attachments;
        u32 sampled_image_color_sample_counts;
        u32 sampled_image_integer_sample_counts;
        u32 sampled_image_depth_sample_counts;
        u32 sampled_image_stencil_sample_counts;
        u32 storage_image_sample_counts;
        u32 max_sample_mask_words;
        u32 timestamp_compute_and_graphics;
        f32 timestamp_period;
        u32 max_clip_distances;
        u32 max_cull_distances;
        u32 max_combined_clip_and_cull_distances;
        u32 discrete_queue_priorities;
        f32 point_size_range[2];
        f32 line_width_range[2];
        f32 point_size_granularity;
        f32 line_width_granularity;
        u32 strict_lines;
        u32 standard_sample_locations;
        u64 optimal_buffer_copy_offset_alignment;
        u64 optimal_buffer_copy_row_pitch_alignment;
        u64 non_coherent_atom_size;
    };

    struct DeviceInfo
    {
        u32 vulkan_api_version;
        u32 driver_version;
        u32 vendor_id;
        u32 device_id;
        DeviceType device_type;
        u8 device_name[256U];
        u8 pipeline_cache_uuid[16U];
        DeviceLimits limits;
    };

    struct Device : public Handle
    {
        auto create_buffer(BufferInfo const & info) -> BufferId;
        auto create_image(ImageInfo const & info) -> ImageId;
        auto create_image_view(ImageViewInfo const & info) -> ImageViewId;
        auto create_sampler(SamplerInfo const & info) -> SamplerId;

        void destroy_buffer(BufferId id);
        void destroy_image(ImageId id);
        void destroy_image(ImageViewId id);
        void destroy_sampler(SamplerId id);

        auto info_buffer(BufferId id) const -> BufferInfo;
        auto info_image(ImageId id) const -> ImageInfo;
        auto info_image_view(ImageViewId id) const -> ImageViewInfo;
        auto info_sampler(SamplerId id) const -> SamplerInfo;

        auto create_pipeline_compiler(PipelineCompilerInfo const & info) -> PipelineCompiler;
        auto create_swapchain(SwapchainInfo const & info) -> Swapchain;
        // auto create_semaphore(SemaphoreInfo const & info) -> Semaphore;
        // auto create_signal(SignalInfo const & info) -> Signal;

        auto info() const -> DeviceInfo const &;
        void wait_idle();

        void submit_commands();
        void present_frame();
        void end_command_batch();

      private:
        friend struct Context;
        Device(std::shared_ptr<void> impl);
    };
} // namespace daxa