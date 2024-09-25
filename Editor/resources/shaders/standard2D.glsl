#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in vec2 a_TexCoord;	
layout(location = 4) in vec3 a_TexIndex;
layout(location = 5) in vec2 a_TilingFactor;
layout(location = 6) in int a_EntityID; 		// for mouse picking

uniform mat4 u_ViewProjection;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec4 v_Color;
flat out vec3 v_TexIndex;
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


#type fragment
#version 330 core

#include "lighting.glsl"

layout(location = 0) out vec4 color;
layout(location = 1) out int entity;

in vec2 v_TexCoord;	
in vec3 v_Normal;
in vec4 v_Color;
flat in vec3 v_TexIndex;
in vec2 v_TilingFactor;
in vec3 v_WorldPos;
flat in int v_EntityID;

uniform sampler2D u_Textures[32];
uniform vec3 u_CameraPos;

uniform Light u_Lights[16];
uniform int u_NumLights;

void main()
{
	vec3 ambient = vec3(0.1);
	vec3 result = vec3(0.0);

	vec3 albedo = texture(u_Textures[int(v_TexIndex[0])], v_TexCoord * v_TilingFactor).rgb;
	vec3 normal = texture(u_Textures[int(v_TexIndex[1])], v_TexCoord * v_TilingFactor).rgb;
	normal = normalize(normal * 2.0 - 1.0); // Convert from [0, 1] to [-1, 1]
    float roughness = texture(u_Textures[int(v_TexIndex[2])], v_TexCoord * v_TilingFactor).r;


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
            float spec = pow(max(dot(normal, halfwayDir), 0.0), (1.0 - roughness) * 256.0); // Adjust shininess based on roughness
            
            // Combine contributions
            result += lightContribution + spec * vec3(1.0); // Use white light for specular
        }
    }

	color = (vec4(ambient, 1.0) + vec4(result, 1.0)) * vec4(albedo, 1.0) * v_Color;
	entity = v_EntityID;
}