#ifndef DAXA_DEVICE_H
#define DAXA_DEVICE_H

#include <daxa-c/core.h>
#include <daxa-c/gpu_resources.h>
// #include <daxa-c/pipeline.h>
// #include <daxa-c/swapchain.h>
// #include <daxa-c/command_list.h>
// #include <daxa-c/semaphore.h>
// #include <daxa-c/split_barrier.h>
// #include <daxa-c/timeline_query.h>

typedef struct
{
    daxa_u32 max_image_dimension1d;
    daxa_u32 max_image_dimension2d;
    daxa_u32 max_image_dimension3d;
    daxa_u32 max_image_dimension_cube;
    daxa_u32 max_image_array_layers;
    daxa_u32 max_texel_buffer_elements;
    daxa_u32 max_uniform_buffer_range;
    daxa_u32 max_storage_buffer_range;
    daxa_u32 max_push_constants_size;
    daxa_u32 max_memory_allocation_count;
    daxa_u32 max_sampler_allocation_count;
    daxa_u64 buffer_image_granularity;
    daxa_u64 sparse_address_space_size;
    daxa_u32 max_bound_descriptor_sets;
    daxa_u32 max_per_stage_descriptor_samplers;
    daxa_u32 max_per_stage_descriptor_uniform_buffers;
    daxa_u32 max_per_stage_descriptor_storage_buffers;
    daxa_u32 max_per_stage_descriptor_sampled_images;
    daxa_u32 max_per_stage_descriptor_storage_images;
    daxa_u32 max_per_stage_descriptor_input_attachments;
    daxa_u32 max_per_stage_resources;
    daxa_u32 max_descriptor_set_samplers;
    daxa_u32 max_descriptor_set_uniform_buffers;
    daxa_u32 max_descriptor_set_uniform_buffers_dynamic;
    daxa_u32 max_descriptor_set_storage_buffers;
    daxa_u32 max_descriptor_set_storage_buffers_dynamic;
    daxa_u32 max_descriptor_set_sampled_images;
    daxa_u32 max_descriptor_set_storage_images;
    daxa_u32 max_descriptor_set_input_attachments;
    daxa_u32 max_vertex_input_attributes;
    daxa_u32 max_vertex_input_bindings;
    daxa_u32 max_vertex_input_attribute_offset;
    daxa_u32 max_vertex_input_binding_stride;
    daxa_u32 max_vertex_output_components;
    daxa_u32 max_tessellation_generation_level;
    daxa_u32 max_tessellation_patch_size;
    daxa_u32 max_tessellation_control_per_vertex_input_components;
    daxa_u32 max_tessellation_control_per_vertex_output_components;
    daxa_u32 max_tessellation_control_per_patch_output_components;
    daxa_u32 max_tessellation_control_total_output_components;
    daxa_u32 max_tessellation_evaluation_input_components;
    daxa_u32 max_tessellation_evaluation_output_components;
    daxa_u32 max_geometry_shader_invocations;
    daxa_u32 max_geometry_input_components;
    daxa_u32 max_geometry_output_components;
    daxa_u32 max_geometry_output_vertices;
    daxa_u32 max_geometry_total_output_components;
    daxa_u32 max_fragment_input_components;
    daxa_u32 max_fragment_output_attachments;
    daxa_u32 max_fragment_dual_src_attachments;
    daxa_u32 max_fragment_combined_output_resources;
    daxa_u32 max_compute_shared_memory_size;
    daxa_u32 max_compute_work_group_count[3];
    daxa_u32 max_compute_work_group_invocations;
    daxa_u32 max_compute_work_group_size[3];
    daxa_u32 sub_pixel_precision_bits;
    daxa_u32 sub_texel_precision_bits;
    daxa_u32 mipmap_precision_bits;
    daxa_u32 max_draw_indexed_index_value;
    daxa_u32 max_draw_indirect_count;
    daxa_f32 max_sampler_lod_bias;
    daxa_f32 max_sampler_anisotropy;
    daxa_u32 max_viewports;
    daxa_u32 max_viewport_dimensions[2];
    daxa_f32 viewport_bounds_range[2];
    daxa_u32 viewport_sub_pixel_bits;
    daxa_usize min_memory_map_alignment;
    daxa_u64 min_texel_buffer_offset_alignment;
    daxa_u64 min_uniform_buffer_offset_alignment;
    daxa_u64 min_storage_buffer_offset_alignment;
    daxa_i32 min_texel_offset;
    daxa_u32 max_texel_offset;
    daxa_i32 min_texel_gather_offset;
    daxa_u32 max_texel_gather_offset;
    daxa_f32 min_interpolation_offset;
    daxa_f32 max_interpolation_offset;
    daxa_u32 sub_pixel_interpolation_offset_bits;
    daxa_u32 max_framebuffer_width;
    daxa_u32 max_framebuffer_height;
    daxa_u32 max_framebuffer_layers;
    daxa_u32 framebuffer_color_sample_counts;
    daxa_u32 framebuffer_depth_sample_counts;
    daxa_u32 framebuffer_stencil_sample_counts;
    daxa_u32 framebuffer_no_attachments_sample_counts;
    daxa_u32 max_color_attachments;
    daxa_u32 sampled_image_color_sample_counts;
    daxa_u32 sampled_image_integer_sample_counts;
    daxa_u32 sampled_image_depth_sample_counts;
    daxa_u32 sampled_image_stencil_sample_counts;
    daxa_u32 storage_image_sample_counts;
    daxa_u32 max_sample_mask_words;
    daxa_u32 timestamp_compute_and_graphics;
    daxa_f32 timestamp_period;
    daxa_u32 max_clip_distances;
    daxa_u32 max_cull_distances;
    daxa_u32 max_combined_clip_and_cull_distances;
    daxa_u32 discrete_queue_priorities;
    daxa_f32 point_size_range[2];
    daxa_f32 line_width_range[2];
    daxa_f32 point_size_granularity;
    daxa_f32 line_width_granularity;
    daxa_u32 strict_lines;
    daxa_u32 standard_sample_locations;
    daxa_u64 optimal_buffer_copy_offset_alignment;
    daxa_u64 optimal_buffer_copy_row_pitch_alignment;
    daxa_u64 non_coherent_atom_size;
} daxa_DeviceLimits;

typedef struct
{
    daxa_u32 vulkan_api_version;
    daxa_u32 driver_version;
    daxa_u32 vendor_id;
    daxa_u32 device_id;
    daxa_DeviceType device_type;
    daxa_u8 device_name[256U];
    daxa_u8 pipeline_cache_uuid[16U];
    daxa_DeviceLimits limits;
} daxa_DeviceProperties;

DAXA_EXTERN_C void daxa_default_device_score(daxa_DeviceProperties const *);

typedef struct
{
    daxa_i32 (*selector)(daxa_DeviceProperties const *);
    daxa_u8 use_scalar_layout;
    char const * debug_name;
} daxa_DeviceInfo;

// typedef struct
// {
//     PipelineStageFlags src_stages = {};
//     std::vector<CommandList> command_lists = {};
//     std::vector<BinarySemaphore> wait_binary_semaphores = {};
//     std::vector<BinarySemaphore> signal_binary_semaphores = {};
//     std::vector<std::pair<TimelineSemaphore, daxa_u64>> wait_timeline_semaphores = {};
//     std::vector<std::pair<TimelineSemaphore, daxa_u64>> signal_timeline_semaphores = {};
// } daxa_CommandSubmitInfo;

// typedef struct
// {
//     std::vector<BinarySemaphore> wait_binary_semaphores = {};
//     Swapchain swapchain;
// } daxa_PresentInfo;

typedef void daxa_Device;

DAXA_EXTERN_C void daxa_destroy_device(daxa_Device * self);

DAXA_EXTERN_C daxa_BufferId daxa_device_create_buffer(daxa_Device * self, daxa_BufferInfo const * info);
DAXA_EXTERN_C daxa_ImageId daxa_device_create_image(daxa_Device * self, daxa_ImageInfo const * info);
DAXA_EXTERN_C daxa_ImageViewId daxa_device_create_image_view(daxa_Device * self, daxa_ImageViewInfo const * info);
DAXA_EXTERN_C daxa_SamplerId daxa_device_create_sampler(daxa_Device * self, daxa_SamplerInfo const * info);
// DAXA_EXTERN_C daxa_TimelineQueryPool daxa_device_create_timeline_query_pool(daxa_Device * self, daxa_TimelineQueryPoolInfo const * info);
DAXA_EXTERN_C void daxa_device_destroy_buffer(daxa_Device * self, daxa_BufferId id);
DAXA_EXTERN_C void daxa_device_destroy_image(daxa_Device * self, daxa_ImageId id);
DAXA_EXTERN_C void daxa_device_destroy_image_view(daxa_Device * self, daxa_ImageViewId id);
DAXA_EXTERN_C void daxa_device_destroy_sampler(daxa_Device * self, daxa_SamplerId id);
DAXA_EXTERN_C daxa_BufferInfo daxa_device_info_buffer(daxa_Device const * self, daxa_BufferId id);
DAXA_EXTERN_C daxa_u64 daxa_device_buffer_reference(daxa_Device const * self, daxa_BufferId id);
DAXA_EXTERN_C daxa_ImageInfo daxa_device_info_image(daxa_Device const * self, daxa_ImageId id);
DAXA_EXTERN_C daxa_ImageViewInfo daxa_device_info_image_view(daxa_Device const * self, daxa_ImageViewId id);
DAXA_EXTERN_C daxa_SamplerInfo daxa_device_info_sampler(daxa_Device const * self, daxa_SamplerId id);
// DAXA_EXTERN_C daxa_PipelineCompiler daxa_device_create_pipeline_compiler(daxa_Device * self, daxa_PipelineCompilerInfo const * info);
// DAXA_EXTERN_C daxa_Swapchain daxa_device_create_swapchain(daxa_Device * self, daxa_SwapchainInfo const * info);
// DAXA_EXTERN_C daxa_CommandList daxa_device_create_command_list(daxa_Device * self, daxa_CommandListInfo const * info);
// DAXA_EXTERN_C daxa_BinarySemaphore daxa_device_create_binary_semaphore(daxa_Device * self, daxa_BinarySemaphoreInfo const * info);
// DAXA_EXTERN_C daxa_TimelineSemaphore daxa_device_create_timeline_semaphore(daxa_Device * self, daxa_TimelineSemaphoreInfo const * info);
// DAXA_EXTERN_C daxa_SplitBarrierState daxa_device_create_split_barrier(daxa_Device * self, daxa_SplitBarrierInfo const * info);
DAXA_EXTERN_C void daxa_device_map_memory(daxa_Device * self, daxa_BufferId id);
DAXA_EXTERN_C void daxa_device_unmap_memory(daxa_Device * self, daxa_BufferId id);
DAXA_EXTERN_C daxa_DeviceInfo const * daxa_device_info(daxa_Device const * self);
DAXA_EXTERN_C daxa_DeviceProperties const * daxa_device_properties(daxa_Device const * self);
DAXA_EXTERN_C void daxa_device_wait_idle(daxa_Device * self);
// DAXA_EXTERN_C void daxa_device_submit_commands(daxa_Device * self, daxa_CommandSubmitInfo const * submit_info);
// DAXA_EXTERN_C void daxa_device_present_frame(daxa_Device * self, daxa_PresentInfo const * info);
DAXA_EXTERN_C void daxa_device_collect_garbage(daxa_Device * self);

#endif
