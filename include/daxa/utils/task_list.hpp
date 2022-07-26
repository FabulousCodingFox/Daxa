#pragma once

#include <daxa/core.hpp>
#include <daxa/device.hpp>

namespace daxa
{
    enum struct TaskBufferAccess
    {
        NONE,
        SHADER_READ_ONLY,
        VERTEX_SHADER_READ_ONLY,
        TESSELLATION_CONTROL_SHADER_READ_ONLY,
        TESSELLATION_EVALUATION_SHADER_READ_ONLY,
        GEOMETRY_SHADER_READ_ONLY,
        FRAGMENT_SHADER_READ_ONLY,
        COMPUTE_SHADER_READ_ONLY,
        SHADER_WRITE_ONLY,
        VERTEX_SHADER_WRITE_ONLY,
        TESSELLATION_CONTROL_SHADER_WRITE_ONLY,
        TESSELLATION_EVALUATION_SHADER_WRITE_ONLY,
        GEOMETRY_SHADER_WRITE_ONLY,
        FRAGMENT_SHADER_WRITE_ONLY,
        COMPUTE_SHADER_WRITE_ONLY,
        SHADER_READ_WRITE,
        VERTEX_SHADER_READ_WRITE,
        TESSELLATION_CONTROL_SHADER_READ_WRITE,
        TESSELLATION_EVALUATION_SHADER_READ_WRITE,
        GEOMETRY_SHADER_READ_WRITE,
        FRAGMENT_SHADER_READ_WRITE,
        COMPUTE_SHADER_READ_WRITE,
        TRANSFER_READ,
        TRANSFER_WRITE,
        HOST_TRANSFER_READ,
        HOST_TRANSFER_WRITE,
    };

    auto to_string(TaskBufferAccess const & usage) -> std::string_view;

    enum struct TaskImageAccess
    {
        NONE,
        SHADER_READ_ONLY,
        VERTEX_SHADER_READ_ONLY,
        TESSELLATION_CONTROL_SHADER_READ_ONLY,
        TESSELLATION_EVALUATION_SHADER_READ_ONLY,
        GEOMETRY_SHADER_READ_ONLY,
        FRAGMENT_SHADER_READ_ONLY,
        COMPUTE_SHADER_READ_ONLY,
        SHADER_WRITE_ONLY,
        VERTEX_SHADER_WRITE_ONLY,
        TESSELLATION_CONTROL_SHADER_WRITE_ONLY,
        TESSELLATION_EVALUATION_SHADER_WRITE_ONLY,
        GEOMETRY_SHADER_WRITE_ONLY,
        FRAGMENT_SHADER_WRITE_ONLY,
        COMPUTE_SHADER_WRITE_ONLY,
        SHADER_READ_WRITE,
        VERTEX_SHADER_READ_WRITE,
        TESSELLATION_CONTROL_SHADER_READ_WRITE,
        TESSELLATION_EVALUATION_SHADER_READ_WRITE,
        GEOMETRY_SHADER_READ_WRITE,
        FRAGMENT_SHADER_READ_WRITE,
        COMPUTE_SHADER_READ_WRITE,
        TRANSFER_READ,
        TRANSFER_WRITE,
        COLOR_ATTACHMENT,
        DEPTH_ATTACHMENT,
        STENCIL_ATTACHMENT,
        DEPTH_STENCIL_ATTACHMENT,
        DEPTH_ATTACHMENT_READ_ONLY,
        STENCIL_ATTACHMENT_READ_ONLY,
        DEPTH_STENCIL_ATTACHMENT_READ_ONLY,
        RESOLVE_WRITE,
        PRESENT,
    };

    auto to_string(TaskImageAccess const & usage) -> std::string_view;

    struct TaskGPUResourceId
    {
        u32 index = {};

        auto is_empty() const -> bool;
    };

    struct TaskBufferId : public TaskGPUResourceId
    {
    };

    struct TaskImageId : public TaskGPUResourceId
    {
    };

    struct TaskResources
    {
        std::vector<std::tuple<TaskBufferId, TaskBufferAccess>> buffers = {};
        std::vector<std::tuple<TaskImageId, TaskImageAccess>> images = {};
    };

    struct TaskList;
    struct Device;

    struct TaskInterface
    {
        auto get_device() -> Device &;
        auto get_command_list() -> CommandList &;
        auto get_resources() -> TaskResources &;
        auto get_buffer(TaskBufferId const & task_id) -> BufferId;
        auto get_image(TaskImageId const & task_id) -> ImageId;
        auto get_image_view(TaskImageId const & task_id) -> ImageViewId;

      private:
        friend struct TaskRuntime;
        TaskInterface(void * backend, TaskResources * resources);
        void * backend;
        TaskResources * resources;
    };

    using TaskCallback = std::function<void(TaskInterface &)>;
    using CreateTaskBufferCallback = std::function<BufferId(void)>;
    using CreateTaskImageCallback = std::function<ImageId(void)>;

    struct TaskBufferInfo
    {
        CreateTaskBufferCallback fetch_callback = {};
        Access last_access = AccessConsts::NONE;
    };

    struct TaskImageInfo
    {
        CreateTaskImageCallback fetch_callback = {};
        Access last_access = AccessConsts::NONE;
        ImageLayout last_layout = ImageLayout::UNDEFINED;
        ImageMipArraySlice slice = {};
    };

    struct TaskInfo
    {
        TaskResources resources = {};
        TaskCallback task = {};
        std::string debug_name = {};
    };

    struct TaskRenderAttachmentInfo
    {
        TaskImageId image = {};
        // optional field:
        ImageLayout layout_override = {};
        AttachmentLoadOp load_op = AttachmentLoadOp::DONT_CARE;
        AttachmentStoreOp store_op = AttachmentStoreOp::STORE;
        ClearValue clear_value = {};
    };

    struct TaskRenderPassBeginInfo
    {
        std::vector<TaskRenderAttachmentInfo> color_attachments = {};
        std::optional<TaskRenderAttachmentInfo> depth_attachment = {};
        std::optional<TaskRenderAttachmentInfo> stencil_attachment = {};
        Rect2D render_area = {};
    };

    struct TaskRenderInfo
    {
        TaskRenderPassBeginInfo render_info = {};
        TaskResources resources = {};
        TaskCallback task = {};
    };

    struct CommandSubmitInfo;
    struct PresentInfo;

    struct TaskListInfo
    {
        Device device;
        std::string debug_name = {};
    };

    struct TaskList : ManagedPtr
    {
        TaskList(TaskListInfo const & info);
        ~TaskList();

        auto create_task_buffer(TaskBufferInfo const & info) -> TaskBufferId;
        auto create_task_image(TaskImageInfo const & info) -> TaskImageId;

        void add_task(TaskInfo const & info);
        void add_render_task(TaskRenderInfo const & info);

        void add_present(Swapchain swapchain);

        void compile();
        void execute();
    };
} // namespace daxa