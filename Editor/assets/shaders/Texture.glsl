#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in vec2 a_TexCoord;	
layout(location = 4) in float a_TexIndex;
layout(location = 5) in vec2 a_TilingFactor;
layout(location = 6) in int a_EntityID; 		// for mouse picking

uniform mat4 u_ViewProjection;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec4 v_Color;
flat out float v_TexIndex;
out vec2 v_TilingFactor;
out vec3 v_WorldPos;
flat out int v_EntityID;

void main()
{
	v_TexCoord = a_TexCoord;
	v_Normal = a_Normal;
	v_Color = a_Color;
	v_TexIndex = a_TexIndex;
	v_TilingFactor = a_TilingFactor;
    v_WorldPos = a_Position;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
	v_EntityID = a_EntityID;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#type fragment
#version 330 core

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
};

layout(location = 0) out vec4 color;
layout(location = 1) out int entity;

in vec2 v_TexCoord;	
in vec3 v_Normal;
in vec4 v_Color;
flat in float v_TexIndex;
in vec2 v_TilingFactor;
in vec3 v_WorldPos;
flat in int v_EntityID;

uniform sampler2D u_Textures[32];

//uniform PointLight u_PointLight;
uniform vec3 u_CameraPos;

uniform PointLight u_PointLights[10];
uniform int u_NumLights;

void main()
{
	vec3 ambient = vec3(0.1);

	vec3 result = vec3(0.0);
	for (int i = 0; i < u_NumLights; i++)
	{
		vec3 lightDir = normalize(u_PointLights[i].position - v_WorldPos);
		float distance = length(u_PointLights[i].position - v_WorldPos);
		 float attenuation = 1.0 / (0.1 + 1.0 * distance + 0.032 * distance * distance);
		vec3 lightColor = u_PointLights[i].color * u_PointLights[i].intensity;
		vec3 diffuse = max(dot(lightDir, normalize(v_Normal)), 0.0) * lightColor;
		result += diffuse * attenuation;
	}
	

	color = (vec4(ambient, 1.0) + vec4(result, 1.0)) * texture(u_Textures[int(v_TexIndex)], v_TexCoord * v_TilingFactor) * v_Color;
	
	entity = v_EntityID;
}