#include <daxa/daxa.hpp>
using namespace daxa::types;

#include <iostream>

namespace tests
{
    auto create_accel_structure(daxa::Device & device) -> i32
    {
        using namespace daxa::types;
        struct Aabb
        {
            f32vec3 min;
            f32vec3 max;
        };
        auto data = Aabb{.min = {0, 0, 0}, .max = {1, 1, 1}};
        auto tlas_id = device.create_acceleration_structure({
            .type = daxa::AccelerationStructureType::TOP_LEVEL,
            .data_info = daxa::AccelerationStructureAabbDataInfo{
                .data = &data,
                .stride = sizeof(Aabb),
            },
            .max_primitives = 1,
            .name = "simplest tlas",
        });
        auto vertices = std::array{
            f32vec3{1.0f, 0.0f, 0.0f},
            f32vec3{0.0f, 1.0f, 0.0f},
            f32vec3{0.0f, 0.0f, 1.0f},
        };
        auto blas_id = device.create_acceleration_structure({
            .type = daxa::AccelerationStructureType::TOP_LEVEL,
            // Why not?
            // .type = daxa::AccelerationStructureType::BOTTOM_LEVEL,
            .data_info = daxa::AccelerationStructureTriangleDataInfo{
                .vertex_format = daxa::Format::R32G32B32_SFLOAT,
                .vertex_data = vertices.data(),
                .vertex_stride = sizeof(f32vec3),
                .index_data = {}, // none
                .index_type_byte_size = 0,
                .transform_data = {}, // none
            },
            .max_primitives = 1,
            .name = "simplest blas",
        });
        device.wait_idle();
        device.destroy_acceleration_structure(tlas_id);
        device.destroy_acceleration_structure(blas_id);
        return 0;
    }

    auto create_rt_pipeline(daxa::Device & device) -> i32
    {
        using namespace daxa::types;

    }
} // namespace tests

auto main() -> int
{
    daxa::Context daxa_ctx = daxa::create_context({
        .enable_validation = false,
    });
    daxa::Device device = daxa_ctx.create_device({
        .enable_raytracing_api = true,
        .name = "test device",
    });

    i32 ret = 0;

    if ((ret = tests::create_accel_structure(device)))
    {
        return ret;
    }

    if ((ret = tests::create_rt_pipeline(device)))
    {
        return ret;
    }

    std::cout << "Success!" << std::endl;
    return ret;
}
