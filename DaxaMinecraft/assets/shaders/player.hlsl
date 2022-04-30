#pragma once

#include "utils/intersect.hlsl"

struct PlayerInput {
    float2 mouse_delta;
    float delta_time;
    float fov;
    float mouse_sens;
    float speed, sprint_speed;
    uint move_flags;

    bool is_moving() { return move_flags != 0; }
    bool move_sprint() { return move_flags & 0x01; }
    bool move_left() { return move_flags & 0x02; }
    bool move_right() { return move_flags & 0x04; }
    bool move_forward() { return move_flags & 0x08; }
    bool move_backward() { return move_flags & 0x10; }
    bool move_up() { return move_flags & 0x20; }
    bool move_down() { return move_flags & 0x40; }
};

struct Camera {
    float4x4 view_mat;
    float fov;
};

struct Player {
    float4 pos;
    float4 vel;
    float4 rot;
    Camera camera;
    uint on_ground;

    void update(in StructuredBuffer<Globals> globals, in PlayerInput input) {
        float delta_dist = input.speed * input.delta_time;

        rot.x -= input.mouse_delta.x * input.mouse_sens * 0.0001f * input.fov;
        rot.y -= input.mouse_delta.y * input.mouse_sens * 0.0001f * input.fov;

        camera.fov = tan(input.fov * 3.14159f / 360.0f);

        const float MAX_ROT = 3.14159f / 2;
        if (rot.y > MAX_ROT)
            rot.y = MAX_ROT;
        if (rot.y < -MAX_ROT)
            rot.y = -MAX_ROT;

        float sin_rot_x = sin(rot.x), cos_rot_x = cos(rot.x);
        float sin_rot_y = sin(rot.y), cos_rot_y = cos(rot.y);
        float sin_rot_z = sin(rot.z), cos_rot_z = cos(rot.z);

        // clang-format off
        camera.view_mat = float4x4(
             1,          0,          0, 0,
             0,  cos_rot_y,  sin_rot_y, 0,
             0, -sin_rot_y,  cos_rot_y, 0,
             0,          0,          0, 0
        );
        float4x4 roty_mat = float4x4(
             cos_rot_x,  0, -sin_rot_x, 0,
             0,          1,          0, 0,
             sin_rot_x,  0,  cos_rot_x, 0,
             0,          0,          0, 0
        );
        // clang-format on

        camera.view_mat = mul(roty_mat, camera.view_mat);

        float4 motion_vec = float4(0, 0, 0, 0);

        if (input.move_sprint())
            delta_dist *= input.sprint_speed;
        if (input.move_forward())
            motion_vec += float4(-sin_rot_x * delta_dist, 0, cos_rot_x * delta_dist, 0);
        if (input.move_backward())
            motion_vec += float4(sin_rot_x * delta_dist, 0, -cos_rot_x * delta_dist, 0);
        if (input.move_left())
            motion_vec += float4(-cos_rot_x * delta_dist, 0, -sin_rot_x * delta_dist, 0);
        if (input.move_right())
            motion_vec += float4(cos_rot_x * delta_dist, 0, sin_rot_x * delta_dist, 0);
        if (input.move_up())
            motion_vec += float4(0, -delta_dist, 0, 0);
        if (input.move_down())
            motion_vec += float4(0, delta_dist, 0, 0);

        // if (input.move_up() && on_ground)
        //     vel.y = -1000;
        // else
        //     vel.y += 9.8 * input.delta_time * 10;
        // motion_vec.y = vel.y * input.delta_time;
        // float3 b_min = float3(0, 0, 0), b_max = float3(int(CHUNK_NX * CHUNK_INDEX_REPEAT_X), int(CHUNK_NY * CHUNK_INDEX_REPEAT_Y), int(CHUNK_NZ * CHUNK_INDEX_REPEAT_Z)) * int(CHUNK_SIZE);
        // bool inside_world = point_box_contains(pos.xyz, b_min, b_max);
        // Ray ray;
        // ray.o = pos.xyz;
        // ray.nrm = normalize(motion_vec.xyz);
        // ray.inv_nrm = 1 / ray.nrm;
        // DDA_RunState dda_run_state = default_dda_run_state();
        // float vec_length = length(motion_vec.xyz);
        // if (inside_world) {
        //     on_ground = false;
        //     run_dda_main(globals, ray, dda_run_state, b_min, b_max, 2);
        //     if (vec_length != 0) {
        //         motion_vec /= vec_length;
        //         if (dda_run_state.hit) {
        //             if (vec_length > dda_run_state.dist - 0.4) {
        //                 on_ground = true;
        //                 vel.y = 0;
        //             }
        //             vec_length = clamp(vec_length, 0, dda_run_state.dist - 0.4);
        //             // if (dda_run_state.side == 1) {
        //             // }
        //         }
        //         motion_vec *= vec_length;
        //     }
        // }

        pos += motion_vec;

        // if (inside_world && vec_length != 0) {
        //     ray.o = pos.xyz;
        //     if (ray_chunk_intersection.hit) {
        //         ray.nrm = normalize(-ray_chunk_intersection.nrm);
        //         ray.inv_nrm = 1 / ray.nrm;
        //         ray_chunk_intersection = trace_chunks(globals, ray);
        //         if (ray_chunk_intersection.hit) {
        //             pos -= float4(ray.nrm * max(0.5 - ray_chunk_intersection.dist, 0), 0);
        //         }
        //     }
        //     dda_run_state = default_dda_run_state();
        //     ray.nrm = float3(sign(motion_vec.x), 0.0, 0.0);
        //     ray.inv_nrm = 1 / ray.nrm;
        //     run_dda_main(globals, ray, dda_run_state, b_min, b_max, 2);
        //     if (dda_run_state.hit) {
        //         pos -= float4(ray.nrm * max(0.4 - dda_run_state.dist, 0), 0);
        //     }
        //     dda_run_state = default_dda_run_state();
        //     ray.nrm = float3(0.0, sign(motion_vec.y), 0.0);
        //     ray.inv_nrm = 1 / ray.nrm;
        //     run_dda_main(globals, ray, dda_run_state, b_min, b_max, 2);
        //     if (dda_run_state.hit) {
        //         pos -= float4(ray.nrm * max(0.4 - dda_run_state.dist, 0), 0);
        //     }
        //     dda_run_state = default_dda_run_state();
        //     ray.nrm = float3(0.0, 0.0, sign(motion_vec.z));
        //     ray.inv_nrm = 1 / ray.nrm;
        //     run_dda_main(globals, ray, dda_run_state, b_min, b_max, 2);
        //     if (dda_run_state.hit) {
        //         pos -= float4(ray.nrm * max(0.4 - dda_run_state.dist, 0), 0);
        //     }
        // }

        pos.w = 0;
    }
};

struct PlayerBuffer {
    PlayerInput input;

    // ---- GPU ONLY ----

    Player player;
};

DAXA_DEFINE_BA_BUFFER(PlayerBuffer)
