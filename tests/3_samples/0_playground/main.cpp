#define DAXA_SHADERLANG DAXA_SHADERLANG_GLSL
#define APPNAME "Daxa Sample: Playground"
#include <0_common/base_app.hpp>

using namespace daxa::types;
#include "shaders/shared.inl"

constexpr auto MAX_VERTS = 1'000'000;

struct App : BaseApp<App>
{
    // clang-format off
    daxa::RasterPipeline vert_raster_pipeline = pipeline_compiler.create_raster_pipeline({
        .vertex_shader_info = {.source = daxa::ShaderFile{"draw.glsl"}, .compile_options = {.defines = {{"DRAW_VERT"}}}},
        .fragment_shader_info = {.source = daxa::ShaderFile{"draw.glsl"}, .compile_options = {.defines = {{"DRAW_FRAG"}}}},
        .color_attachments = {{.format = swapchain.get_format()}},
        .raster = {.face_culling = daxa::FaceCullFlagBits::BACK_BIT, .topology = false},
        .push_constant_size = sizeof(DrawPush),
        .debug_name = APPNAME_PREFIX("vert_raster_pipeline"),
    }).value();
    // daxa::ComputePipeline mesh_raster_pipeline = pipeline_compiler.create_compute_pipeline({
    //     .shader_info = {.source = daxa::ShaderFile{"mesh.glsl"}},
    //     .debug_name = APPNAME_PREFIX("mesh_raster_pipeline"),
    // }).value();
    daxa::RasterPipeline geom_raster_pipeline = pipeline_compiler.create_raster_pipeline({
        .vertex_shader_info = {.source = daxa::ShaderFile{"draw.glsl"}, .compile_options = {.defines = {{"DRAW_VERT"}, {"USE_GEOM", "1"}}}},
        .geometry_shader_info = {.source = daxa::ShaderFile{"draw.glsl"}, .compile_options = {.defines = {{"DRAW_GEOM"}, {"USE_GEOM", "1"}}}},
        .fragment_shader_info = {.source = daxa::ShaderFile{"draw.glsl"}, .compile_options = {.defines = {{"DRAW_FRAG"}, {"USE_GEOM", "1"}}}},
        .color_attachments = {{.format = swapchain.get_format()}},
        .raster = {.face_culling = daxa::FaceCullFlagBits::BACK_BIT, .topology = true},
        .push_constant_size = sizeof(DrawPush),
        .debug_name = APPNAME_PREFIX("geom_raster_pipeline"),
    }).value();
    // clang-format on

    bool mesh_invalid = true;
    bool use_geom = false;

    daxa::BufferId vertex_buffer = device.create_buffer(daxa::BufferInfo{
        .size = sizeof(DrawVertex) * MAX_VERTS,
        .debug_name = APPNAME_PREFIX("vertex_buffer"),
    });
    daxa::TaskBufferId task_vertex_buffer;

    daxa::TaskList loop_task_list = record_loop_task_list();

    ~App()
    {
        device.wait_idle();
        device.collect_garbage();
        device.destroy_buffer(vertex_buffer);
    }

    void ui_update()
    {
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowMetricsWindow();
        ImGui::Begin("Settings");
        ImGui::Checkbox("use Geometry shader", &use_geom);
        ImGui::End();
        ImGui::Render();
    }
    void on_update()
    {
        reload_pipeline(vert_raster_pipeline);
        reload_pipeline(geom_raster_pipeline);
        ui_update();

        swapchain_image = swapchain.acquire_next_image();
        if (swapchain_image.is_empty())
            return;
        loop_task_list.execute();
    }

    void on_mouse_move(f32, f32) {}
    void on_mouse_button(i32, i32) {}
    void on_key(i32, i32) {}
    void on_resize(u32 sx, u32 sy)
    {
        minimized = (sx == 0 || sy == 0);
        if (!minimized)
        {
            swapchain.resize();
            size_x = swapchain.info().width;
            size_y = swapchain.info().height;
            base_on_update();
        }
    }

    void record_tasks(daxa::TaskList & new_task_list)
    {
        task_vertex_buffer = new_task_list.create_task_buffer({.buffer = &vertex_buffer, .debug_name = APPNAME_PREFIX("task_vertex_buffer")});
        new_task_list.add_task({
            .used_buffers = {
                {task_vertex_buffer, daxa::TaskBufferAccess::VERTEX_SHADER_READ_ONLY},
            },
            .task = [this](daxa::TaskRuntime runtime)
            {
                if (mesh_invalid)
                {
                    auto cmd_list = runtime.get_command_list();
                    auto vertex_staging_buffer = device.create_buffer({
                        .memory_flags = daxa::MemoryFlagBits::HOST_ACCESS_RANDOM,
                        .size = sizeof(DrawVertex) * MAX_VERTS,
                        .debug_name = APPNAME_PREFIX("vertex_staging_buffer"),
                    });
                    cmd_list.destroy_buffer_deferred(vertex_staging_buffer);
                    auto buffer_ptr = device.map_memory_as<DrawVertex>(vertex_staging_buffer);

                    for (u32 i = 0; i < MAX_VERTS; ++i)
                    {
                        f32vec2 pos = {0.0f, 0.0f};
                        pos.x = ((rand() % 1000) * 0.001f - 0.5f) * 2.0f;
                        pos.y = ((rand() % 1000) * 0.001f - 0.5f) * 2.0f;
                        *buffer_ptr = DrawVertex{{pos.x, pos.y, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}};
                        ++buffer_ptr;
                    }

                    device.unmap_memory(vertex_staging_buffer);
                    cmd_list.copy_buffer_to_buffer({
                        .src_buffer = vertex_staging_buffer,
                        .dst_buffer = vertex_buffer,
                        .size = sizeof(DrawVertex) * MAX_VERTS,
                    });
                    mesh_invalid = false;
                }
            },
            .debug_name = APPNAME_PREFIX("Upload vertices"),
        });

        // new_task_list.add_task({
        //     .used_buffers = {
        //         {task_vertex_buffer, daxa::TaskBufferAccess::COMPUTE_SHADER_READ_ONLY},
        //     },
        //     .used_images = {
        //         {task_swapchain_image, daxa::TaskImageAccess::COLOR_ATTACHMENT, daxa::ImageMipArraySlice{}},
        //     },
        //     .task = [this](daxa::TaskRuntime runtime)
        //     {
        //         if (!use_geom) {
        //             auto cmd_list = runtime.get_command_list();
        //             cmd_list.set_pipeline(mesh_raster_pipeline);
        //             cmd_list.push_constant(ComputePush{
        //                 .face_buffer = this->device.buffer_reference(vertex_buffer),
        //             });
        //             cmd_list.dispatch(MAX_VERTS);
        //             cmd_list.end_renderpass();
        //         }
        //     },
        //     .debug_name = APPNAME_PREFIX("'mesh' shader"),
        // });

        new_task_list.add_task({
            .used_buffers = {
                {task_vertex_buffer, daxa::TaskBufferAccess::VERTEX_SHADER_READ_ONLY},
            },
            .used_images = {
                {task_swapchain_image, daxa::TaskImageAccess::COLOR_ATTACHMENT, daxa::ImageMipArraySlice{}},
            },
            .task = [this](daxa::TaskRuntime runtime)
            {
                auto cmd_list = runtime.get_command_list();
                cmd_list.begin_renderpass({
                    .color_attachments = {{.image_view = swapchain_image.default_view(), .load_op = daxa::AttachmentLoadOp::CLEAR, .clear_value = std::array<f32, 4>{0.1f, 0.0f, 0.5f, 1.0f}}},
                    .render_area = {.x = 0, .y = 0, .width = size_x, .height = size_y},
                });
                cmd_list.set_pipeline(use_geom ? geom_raster_pipeline : vert_raster_pipeline);
                cmd_list.push_constant(DrawPush{
                    .face_buffer = this->device.buffer_reference(vertex_buffer),
                });
                cmd_list.draw({.vertex_count = use_geom ? MAX_VERTS : (MAX_VERTS * 6u)});
                cmd_list.end_renderpass();
            },
            .debug_name = APPNAME_PREFIX("Draw to swapchain"),
        });
    }
};

int main()
{
    App app = {};
    while (true)
    {
        if (app.update())
            break;
    }
}
