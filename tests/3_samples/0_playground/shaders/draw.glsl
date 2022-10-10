#version 450 core

#include <shared.inl>

DAXA_USE_PUSH_CONSTANT(DrawPush)

const f32 scl = 0.002;

#define DO_CULLING 1

#if defined(DRAW_VERT)

layout(location = 0) out VS_O
{
    f32vec4 col;
}
vs_o;

void main()
{
#if USE_GEOM
    DrawVertex vert = push_constant.face_buffer.verts[gl_VertexIndex];
    gl_Position = f32vec4(vert.pos.xy, 0, 1);
    vs_o.col = vert.col;
#else
    u32 data_index = gl_VertexIndex / 6;
    u32 data_instance = gl_VertexIndex - data_index * 6;

    DrawVertex vert = push_constant.face_buffer.verts[data_index];

    if (abs(vert.pos.x) > 0.95) {
        gl_Position = f32vec4(-2);
    } else {
        switch (data_instance) {
        case 0: gl_Position = f32vec4(vert.pos.xy + f32vec2(0.0, 0.0), 0, 1); break;
        case 1: gl_Position = f32vec4(vert.pos.xy + f32vec2(scl, 0.0), 0, 1); break;
        case 2: gl_Position = f32vec4(vert.pos.xy + f32vec2(0.0, scl), 0, 1); break;
        case 3: gl_Position = f32vec4(vert.pos.xy + f32vec2(0.0, scl), 0, 1); break;
        case 4: gl_Position = f32vec4(vert.pos.xy + f32vec2(scl, 0.0), 0, 1); break;
        case 5: gl_Position = f32vec4(vert.pos.xy + f32vec2(scl, scl), 0, 1); break;
        }
    }
    vs_o.col = vert.col;

#endif
}

#elif defined(DRAW_GEOM)

layout(location = 0) in VS_O
{
    f32vec4 col;
}
vs_i[];

layout(location = 0) out GS_O
{
    f32vec4 col;
}
gs_o;

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;
void main()
{
    if (abs(gl_in[0].gl_Position.x) > 0.95)
        return;

    gl_Position = gl_in[0].gl_Position + f32vec4(0.0, 0.0, 0.0, 0.0);
    gs_o.col = vs_i[0].col;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + f32vec4(scl, 0.0, 0.0, 0.0);
    gs_o.col = vs_i[0].col;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + f32vec4(0.0, scl, 0.0, 0.0);
    gs_o.col = vs_i[0].col;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + f32vec4(scl, 0.0, 0.0, 0.0);
    gs_o.col = vs_i[0].col;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + f32vec4(0.0, scl, 0.0, 0.0);
    gs_o.col = vs_i[0].col;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + f32vec4(scl, scl, 0.0, 0.0);
    gs_o.col = vs_i[0].col;
    EmitVertex();

    EndPrimitive();
}

#elif defined(DRAW_FRAG)

#if USE_GEOM
layout(location = 0) in GS_O
#else
layout(location = 0) in VS_O
#endif
{
    f32vec4 col;
}
gs_i;

layout(location = 0) out f32vec4 color;

void main()
{
    color = f32vec4(gs_i.col.rgb, 1);
}

#endif
