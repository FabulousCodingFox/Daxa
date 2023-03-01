#include <daxa/daxa.hpp>
using namespace daxa::types;

#include <iostream>

#define APPNAME "Daxa API Sample Raytrace"
#define APPNAME_PREFIX(x) ("[" APPNAME "] " x)

namespace tests
{
    auto simplest(daxa::Device & device) -> i32
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
            .debug_name = APPNAME_PREFIX("simplest tlas"),
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
            .debug_name = APPNAME_PREFIX("simplest blas"),
        });
        device.wait_idle();
        device.destroy_acceleration_structure(tlas_id);
        device.destroy_acceleration_structure(blas_id);
        return 0;
    }
} // namespace tests

auto main() -> int
{
    daxa::Context daxa_ctx = daxa::create_context({
        .enable_validation = false,
    });
    daxa::Device device = daxa_ctx.create_device({
        .enable_raytracing_api = true,
        .debug_name = APPNAME_PREFIX("device"),
    });

    i32 ret = 0;

    if ((ret = tests::simplest(device)))
    {
        return ret;
    }

    std::cout << "Success!" << std::endl;
    return ret;
}
