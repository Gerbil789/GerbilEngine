//flatColor shader

#type vertex
#version 460 core

layout(location = 0) in vec3 Position;

layout(std140, binding = 0) uniform GlobalUniforms
{
    mat4 ViewProjection;
} GlobalData;



void main()
{
    gl_Position = GlobalData.ViewProjection * vec4(Position, 1.0);
}

#type fragment
#version 460 core

layout(location = 0) out vec4 color;
layout(location = 1) out int entityID;

layout(set = 0, binding = 1) uniform MaterialUniforms
{
    vec3 Color;
} MaterialData;

layout(std140, binding = 2) uniform ObjectUniforms
{
    int EntityID;
} ObjectData;

void main()
{
    color = vec4(MaterialData.Color, 1.0);
    entityID = ObjectData.EntityID;
}