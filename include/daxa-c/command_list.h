#pragma once

#include <daxa-c/core.h>
#include <daxa-c/gpu_resources.h>
#include <daxa-c/pipeline.h>
#include <daxa-c/split_barrier.h>

#include <span>
#include <daxa-c/timeline_query.h>

namespace daxa
{
    struct CommandListInfo
    {
        std::string debug_name = {};
    };

    struct ImageBlitInfo
    {
        ImageId src_image = {};
        ImageLayout src_image_layout = {};
        ImageId dst_image = {};
        ImageLayout dst_image_layout = {};
        ImageArraySlice src_slice = {};
        std::array<Offset3D, 2> src_offsets = {};
        ImageArraySlice dst_slice = {};
        std::array<Offset3D, 2> dst_offsets = {};
        Filter filter = {};
    };

    struct BufferCopyInfo
    {
        BufferId src_buffer = {};
        daxa_usize src_offset = {};
        BufferId dst_buffer = {};
        daxa_usize dst_offset = {};
        daxa_usize size = {};
    };

    struct BufferImageCopy
    {
        BufferId buffer = {};
        daxa_usize buffer_offset = {};
        ImageId image = {};
        ImageLayout image_layout = {};
        ImageArraySlice image_slice = {};
        Offset3D image_offset = {};
        Extent3D image_extent = {};
    };

    struct ImageToBufferInfo
    {
        ImageId image = {};
        ImageLayout image_layout = {};
        ImageArraySlice image_slice = {};
        Offset3D image_offset = {};
        Extent3D image_extent = {};
        BufferId buffer = {};
        daxa_usize buffer_offset = {};
    };

    struct ImageCopyInfo
    {
        ImageId src_image = {};
        ImageLayout src_image_layout = {};
        ImageId dst_image = {};
        ImageLayout dst_image_layout = {};
        ImageArraySlice src_slice = {};
        Offset3D src_offset = {};
        ImageArraySlice dst_slice = {};
        Offset3D dst_offset = {};
        Extent3D extent = {};
    };

    struct ImageClearInfo
    {
        ImageLayout dst_image_layout = {};
        ClearValue clear_value;
        ImageId dst_image = {};
        ImageMipArraySlice dst_slice = {};
    };

    struct BufferClearInfo
    {
        BufferId buffer = {};
        daxa_usize offset = {};
        daxa_usize size = {};
        daxa_u32 clear_value = {};
    };

    struct RenderAttachmentInfo
    {
        ImageViewId image_view{};
        ImageLayout layout = ImageLayout::ATTACHMENT_OPTIMAL;
        AttachmentLoadOp load_op = AttachmentLoadOp::DONT_CARE;
        AttachmentStoreOp store_op = AttachmentStoreOp::STORE;
        ClearValue clear_value = {};
    };

    struct RenderPassBeginInfo
    {
        std::vector<RenderAttachmentInfo> color_attachments = {};
        std::optional<RenderAttachmentInfo> depth_attachment = {};
        std::optional<RenderAttachmentInfo> stencil_attachment = {};
        Rect2D render_area = {};
    };

    struct DispatchIndirectInfo
    {
        BufferId indirect_buffer = {};
        daxa_usize offset = {};
    };

    struct DrawInfo
    {
        daxa_u32 vertex_count = {};
        daxa_u32 instance_count = 1;
        daxa_u32 first_vertex = {};
        daxa_u32 first_instance = 0;
    };

    struct DrawIndexedInfo
    {
        daxa_u32 index_count = {};
        daxa_u32 instance_count = 1;
        daxa_u32 first_index = {};
        daxa_i32 vertex_offset = {};
        daxa_u32 first_instance = 0;
    };

    struct DrawIndirectInfo
    {
        BufferId indirect_buffer = {};
        daxa_usize offset = {};
        daxa_u32 draw_count = {};
        daxa_u32 stride = {};
    };

    struct ResetSplitBarriersInfo
    {
        SplitBarrierState & split_barrier;
        PipelineStageFlags stage;
    };

    struct WaitSplitBarriersInfo
    {
        std::span<SplitBarrierState> split_barriers;
    };

    struct WriteTimestampInfo
    {
        TimelineQueryPool & query_pool;
        PipelineStageFlags pipeline_stage = {};
        daxa_u32 query_index = {};
    };

    struct ResetTimestampsInfo
    {
        TimelineQueryPool & query_pool;
        daxa_u32 start_index = {};
        daxa_u32 count = {};
    };

    struct ResetSplitBarrierInfo
    {
        SplitBarrierState & barrier;
        PipelineStageFlags stage_masks;
    };

    struct CommandList : ManagedPtr
    {
        CommandList();

        void copy_buffer_to_buffer(BufferCopyInfo const & info);
        void copy_buffer_to_image(BufferImageCopy const & info);
        void copy_image_to_buffer(BufferImageCopy const & info);
        void copy_image_to_image(ImageCopyInfo const & info);
        void blit_image_to_image(ImageBlitInfo const & info);

        void clear_buffer(BufferClearInfo const & info);
        void clear_image(ImageClearInfo const & info);

        void pipeline_barrier(MemoryBarrierInfo const & info);
        void pipeline_barrier_image_transition(ImageBarrierInfo const & info);
        void signal_split_barrier(SplitBarrierSignalInfo const & info);
        void wait_split_barriers(std::span<SplitBarrierWaitInfo const> const & infos);
        void wait_split_barrier(SplitBarrierWaitInfo const & info);
        void reset_split_barrier(ResetSplitBarrierInfo const & info);

        void push_constant(void const * data, daxa_u32 size, daxa_u32 offset = 0);
        template <typename T>
        void push_constant(T const & constant, daxa_usize offset = 0)
        {
            push_constant(&constant, static_cast<daxa_u32>(sizeof(T)), static_cast<daxa_u32>(offset));
        }
        void set_pipeline(ComputePipeline const & pipeline);
        void set_pipeline(RasterPipeline const & pipeline);
        void dispatch(daxa_u32 group_x, daxa_u32 group_y = 1, daxa_u32 group_z = 1);
        void dispatch_indirect(DispatchIndirectInfo const & info);

        void destroy_buffer_deferred(BufferId id);
        void destroy_image_deferred(ImageId id);
        void destroy_image_view_deferred(ImageViewId id);
        void destroy_sampler_deferred(SamplerId id);

        void begin_renderpass(RenderPassBeginInfo const & info);
        void end_renderpass();
        void set_viewport(ViewportInfo const & info);
        void set_scissor(Rect2D const & info);
        void set_index_buffer(BufferId id, daxa_usize offset, daxa_usize index_type_byte_size = sizeof(daxa_u32));
        void draw(DrawInfo const & info);
        void draw_indexed(DrawIndexedInfo const & info);
        void draw_indirect(DrawIndirectInfo const & info);

        void write_timestamp(WriteTimestampInfo const & info);
        void reset_timestamps(ResetTimestampsInfo const & info);

        void complete();
        auto is_complete() const -> bool;

        auto info() const -> CommandListInfo const &;

      private:
        friend struct Device;
        explicit CommandList(ManagedPtr impl);
    };
} // namespace daxa
