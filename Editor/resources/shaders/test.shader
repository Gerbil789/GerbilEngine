//test shader
#type vertex
#version 460 core

layout(std140, binding = 0) uniform GlobalProperties
{
    mat4 ViewProjection;
} u_Global;

// VERTEX IN
layout(location = 0) in int EntityID;
layout(location = 1) in vec3 Position;
layout(location = 2) in vec3 Normal;

// VERTEX OUT
layout(location = 0) flat out int id;
layout(location = 1) out vec3 v_Normal;


void main()
{
    id = EntityID;
    v_Normal = Normal;
    gl_Position = u_Global.ViewProjection * vec4(Position, 1.0);
}

////////////////////////////////////////////////////////////////////////////////////

#type fragment
#version 460 core

layout(std140, binding = 1) uniform MaterialProperties
{
    vec3 Color;
} u_Material;


// FRAGMENT IN
layout(location = 0) flat in int id;
layout(location = 1) in vec3 v_Normal;

// FRAGMENT OUT
layout(location = 0) out vec4 color;
layout(location = 1) out int entityID;

void main()
{
    color = vec4(u_Material.Color * v_Normal, 1.0);
    entityID = id;
}