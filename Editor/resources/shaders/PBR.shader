#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in vec2 a_TexCoord;	
layout(location = 4) in vec3 a_Tangent;
layout(location = 5) in vec3 a_Bitangent;
layout(location = 6) in vec2 a_Tiling;
layout(location = 7) in vec2 a_Offset;
layout(location = 8) in int a_EntityID;

uniform mat4 u_ViewProjection;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec4 v_Color;
out mat3 TBN;  // Tangent-Bitangent-Normal matrix
out vec2 v_Tiling;
out vec2 v_Offset;
out vec3 v_WorldPos;
flat out int v_EntityID;

void main()
{
	v_TexCoord = a_TexCoord;
	v_Normal = a_Normal;
	v_Color = a_Color;
    vec3 T = normalize(a_Tangent);
    vec3 B = normalize(a_Bitangent);
    vec3 N = normalize(a_Normal);
    TBN = mat3(T, B, N);
	v_Tiling = a_Tiling;
    v_Offset = a_Offset;
    v_WorldPos = a_Position;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
	v_EntityID = a_EntityID;
}


#type fragment
#version 330 core

#include "lighting.shader"

layout(location = 0) out vec4 color;
layout(location = 1) out int entity;

in vec2 v_TexCoord;	
in vec3 v_Normal;
in vec4 v_Color;
in mat3 TBN;
in vec2 v_Tiling;
in vec2 v_Offset;
in vec3 v_WorldPos;
flat in int v_EntityID;

uniform sampler2D u_Textures[6]; //color, metallic, roughness, normal, height, ao
uniform vec3 u_CameraPos;

uniform Light u_Lights[16];
uniform int u_NumLights;

void main()
{
	vec3 ambient = vec3(0.1);
	vec3 result = vec3(0.0);

    vec2 UV = v_TexCoord * v_Tiling + v_Offset;

	vec3 albedo = texture(u_Textures[0], UV).rgb;
    float metallic = texture(u_Textures[1], UV).r;
    float roughness = texture(u_Textures[2], UV).r;
	vec3 normalMap = texture(u_Textures[3], UV).rgb;
	normalMap = normalMap * 2.0 - 1.0; 
    vec3 normal = normalize(TBN * normalMap);
    float height = texture(u_Textures[4], UV).r;
    float ao = texture(u_Textures[5], UV).r;

	for (int i = 0; i < u_NumLights; i++) 
    {
        vec3 lightDir = normalize(u_Lights[i].position - v_WorldPos);
        vec3 viewDir = normalize(u_CameraPos - v_WorldPos);

        // Only proceed with lighting calculations if the normal is facing the light
        float NdotL = max(dot(normal, lightDir), 0.0);

        if (NdotL > 0.0) // Only add lighting if facing the light
        {
            vec3 lightContribution = vec3(0.0);

            if (u_Lights[i].type == 0) {
                lightContribution += calculatePointLight(u_Lights[i], v_WorldPos, normal);
            } 
            else if (u_Lights[i].type == 1) 
            {
                lightContribution += calculateDirectionalLight(u_Lights[i], normal);
            } 
            else if (u_Lights[i].type == 2) 
            {
                lightContribution += calculateSpotLight(u_Lights[i], v_WorldPos, normal);
            }

            // Calculate specular contribution
            vec3 reflectDir = reflect(-lightDir, normal);
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float shininess = mix(8.0, 256.0, 1.0 - roughness); // Interpolate between a range of shininess values
            float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
            
            // Combine contributions
            result += lightContribution + spec * vec3(1.0); // Use white light for specular
        }
    }
    color = (vec4(ambient, 1.0) + vec4(result, 1.0)) * vec4(albedo, 1.0) * v_Color;

	entity = v_EntityID;
}