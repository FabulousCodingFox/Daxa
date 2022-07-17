#include <0_common/window.hpp>
#include <thread>

namespace tests
{
    void simple_creation()
    {
        struct App : AppWindow<App>
        {
            daxa::Context daxa_ctx = daxa::create_context({});
            daxa::Device daxa_device = daxa_ctx.create_default_device();

            daxa::Swapchain daxa_swapchain = daxa_device.create_swapchain({
                .native_window_handle = get_native_handle(),
                .width = size_x,
                .height = size_y,
                .image_usage = daxa::ImageUsageFlagBits::TRANSFER_DST,
            });

            void on_resize(u32, u32) {}
        };
        App app;
    }

    void usage()
    {
        struct App : AppWindow<App>
        {
            daxa::Context daxa_ctx = daxa::create_context({});
            daxa::Device daxa_device = daxa_ctx.create_default_device();

            daxa::Swapchain daxa_swapchain = daxa_device.create_swapchain({
                .native_window_handle = get_native_handle(),
                .width = size_x,
                .height = size_y,
                .image_usage = daxa::ImageUsageFlagBits::TRANSFER_DST,
            });

            bool update()
            {
                glfwPollEvents();
                if (glfwWindowShouldClose(glfw_window_ptr))
                {
                    return true;
                }

                if (!minimized)
                {
                    draw();
                }
                else
                {
                    using namespace std::literals;
                    std::this_thread::sleep_for(1ms);
                }

                return false;
            }

            void draw()
            {
                auto swapchain_image = daxa_swapchain.acquire_next_image();
                auto binary_semaphore = daxa_device.create_binary_semaphore({});
                auto cmd_list = daxa_device.create_command_list({});

                cmd_list.pipeline_barrier_image_transition({
                    .waiting_pipeline_access = daxa::PipelineStageAccessFlagBits::TRANSFER_WRITE,
                    .before_layout = daxa::ImageLayout::UNDEFINED,
                    .after_layout = daxa::ImageLayout::TRANSFER_DST_OPTIMAL,
                    .image_id = swapchain_image,
                });

                cmd_list.clear_image({
                    .dst_image_layout = daxa::ImageLayout::TRANSFER_DST_OPTIMAL,
                    .clear_color = {std::array<f32, 4>{1, 0, 1, 1}},
                    .dst_image = swapchain_image,
                });

                cmd_list.pipeline_barrier_image_transition({
                    .awaited_pipeline_access = daxa::PipelineStageAccessFlagBits::TRANSFER_WRITE,
                    .before_layout = daxa::ImageLayout::TRANSFER_DST_OPTIMAL,
                    .after_layout = daxa::ImageLayout::PRESENT_SRC,
                    .image_id = swapchain_image,
                });

                cmd_list.complete();

                daxa_device.submit_commands({
                    .command_lists = {std::move(cmd_list)},
                    .signal_binary_semaphores_on_completion = {binary_semaphore},
                });

                daxa_device.present_frame({
                    .wait_on_binary = binary_semaphore,
                    .swapchain = daxa_swapchain,
                });
            }

            void on_resize(u32 sx, u32 sy)
            {
                size_x = sx;
                size_y = sy;
                daxa_swapchain.resize(size_x, size_y);
                draw();
            }
        };

        App app = {};
        while (true)
        {
            if (app.update())
                break;
        }
    }
} // namespace tests

int main()
{
    tests::simple_creation();
    tests::usage();
}
