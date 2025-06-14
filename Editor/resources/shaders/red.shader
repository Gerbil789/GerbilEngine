//test shader
#type vertex
#version 460 core

layout(std140, binding = 0) uniform GlobalProperties {
    mat4 ViewProjection;
} u_Global;

layout(location = 0) in int EntityID;
layout(location = 1) in vec3 Position;

layout(location = 0) flat out int id;

void main() {
    id = EntityID;
    gl_Position = u_Global.ViewProjection * vec4(Position, 1.0);
}

#type fragment
#version 460 core

layout(location = 0) flat in int id;

layout(location = 0) out vec4 color;
layout(location = 1) out int entityID;

void main() {
    color = vec4(1.0, 0.0, 0.0, 1.0);
    entityID = id;
}