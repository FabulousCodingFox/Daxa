#pragma once

#include <daxa-c/core.h>

typedef struct
{
    daxa_u32 index_version;
} daxa_GpuResourceId;

typedef struct
{
    daxa_u32 index_version;
} daxa_BufferId;

typedef struct
{
    daxa_u32 index_version;
} daxa_ImageViewId;

typedef struct
{
    daxa_u32 index_version;
} daxa_ImageId;

typedef struct
{
    daxa_u32 index_version;
} daxa_SamplerId;

DAXA_EXTERN_C daxa_ImageViewId daxa_image_default_view(daxa_ImageId self);
DAXA_EXTERN_C daxa_bool daxa_gpu_resource_is_empty(daxa_GpuResourceId const * self);
DAXA_EXTERN_C daxa_bool daxa_gpu_resource_equals(daxa_GpuResourceId const * a, daxa_GpuResourceId const * b);

// auto to_string(types::ImageId image_id) -> std::string;
// auto to_string(types::BufferId buffer_id) -> std::string;

typedef struct 
{
    daxa_MemoryFlags memory_flags;
    daxa_u32 size;
    // std::string debug_name = {};
} daxa_BufferInfo;

typedef struct 
{
    daxa_u32 dimensions;
    daxa_Format format;
    daxa_ImageAspectFlags aspect;
    daxa_u32 size[3];
    daxa_u32 mip_level_count;
    daxa_u32 array_layer_count;
    daxa_u32 sample_count;
    daxa_ImageUsageFlags usage;
    daxa_MemoryFlags memory_flags;
    // std::string debug_name = {};
} daxa_ImageInfo;

typedef struct 
{
    daxa_ImageViewType type;
    daxa_Format format;
    daxa_ImageId image;
    daxa_ImageMipArraySlice slice;
    // std::string debug_name = {};
} daxa_ImageViewInfo;

typedef struct 
{
    daxa_Filter magnification_filter;
    daxa_Filter minification_filter;
    daxa_Filter mipmap_filter;
    daxa_SamplerAddressMode address_mode_u;
    daxa_SamplerAddressMode address_mode_v;
    daxa_SamplerAddressMode address_mode_w;
    daxa_f32 mip_lod_bias;
    daxa_bool enable_anisotropy;
    daxa_f32 max_anisotropy;
    daxa_bool enable_compare;
    daxa_CompareOp compare_op;
    daxa_f32 min_lod;
    daxa_f32 max_lod;
    daxa_bool enable_unnormalized_coordinates;
    // std::string debug_name = {};
} daxa_SamplerInfo;
