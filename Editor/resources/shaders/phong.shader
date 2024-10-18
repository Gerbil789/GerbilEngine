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

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
} CameraData;

struct VertexOutput
{
	vec4 Color;
    vec3 Normal;
    mat3 TBN;
	vec2 TexCoord;
	vec2 Tiling;
    vec2 Offset;
    vec3 WorldPos;
};

//layout(location = 0) out vec2 v_TexCoord;
//layout(location = 1) out vec3 v_Normal;
//layout(location = 2) out vec4 v_Color;
//layout(location = 3) out mat3 TBN;
//layout(location = 4) out vec2 v_Tiling;
//layout(location = 5) out vec2 v_Offset;
//layout(location = 6) out vec3 v_WorldPos;
layout (location = 0) out VertexOutput Output;
layout(location = 20) flat out int v_EntityID;

void main()
{
	Output.TexCoord = a_TexCoord;
	Output.Normal = a_Normal;
	Output.Color = a_Color;
    vec3 T = normalize(a_Tangent);
    vec3 B = normalize(a_Bitangent);
    vec3 N = normalize(a_Normal);
    Output.TBN = mat3(T, B, N);
	Output.Tiling = a_Tiling;
    Output.Offset = a_Offset;
    Output.WorldPos = a_Position;

	gl_Position = CameraData.u_ViewProjection * vec4(a_Position, 1.0);
	v_EntityID = a_EntityID;
}


#type fragment
#version 460 core

struct Light {
	int type; // 0 = point, 1 = directional, 2 = spot
    vec3 position;
    vec3 color;
    float intensity;
	float range;
	vec3 attenuation;
	vec3 direction;
	float innerAngle;
	float outerAngle;
};

struct VertexOutput
{
	vec4 Color;
    vec3 Normal;
    mat3 TBN;
	vec2 TexCoord;
	vec2 Tiling;
    vec2 Offset;
    vec3 WorldPos;
};

layout(location = 0) out vec4 color;   
layout(location = 1) out int entity;

layout(location = 0) in VertexOutput Input;
layout(location = 20) flat in int v_EntityID;

layout(binding = 0) uniform sampler2D u_Textures[6]; //color, metallic, roughness, normal, height, ao
layout(std140, binding = 1) uniform CameraData
{
    vec3 u_CameraPos;
};

layout(std140, binding = 2) uniform LightData
{
    Light u_Lights[16];
    int u_NumLights;
};

vec3 calculatePhongLighting(Light light, vec3 fragPos, vec3 normal, vec3 viewDir) {
    // Calculate the direction from fragment to light source
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Ambient lighting
    vec3 ambient = light.color * 0.1;

    // Diffuse lighting (Lambertian reflection)
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color;

    // Specular lighting (Phong reflection)
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0); 
    vec3 specular = spec * light.color;

    // Attenuation based on distance
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * distance * distance);

    // Final result combining ambient, diffuse, and specular with attenuation
    return (ambient + diffuse + specular) * attenuation * light.intensity;
}


void main()
{
	vec3 ambient = vec3(0.1);

    vec2 UV = Input.TexCoord * Input.Tiling + Input.Offset;

	vec3 albedo = texture(u_Textures[0], UV).rgb;
    albedo *= Input.Color.rgb;
    float metallic = texture(u_Textures[1], UV).r;
    float roughness = texture(u_Textures[2], UV).r;
	vec3 normalMap = texture(u_Textures[3], UV).rgb;
	normalMap = normalMap * 2.0 - 1.0; 
    vec3 normal = normalize(Input.TBN * normalMap);
    float height = texture(u_Textures[4], UV).r;
    float ao = texture(u_Textures[5], UV).r;

    vec3 viewDir = normalize(u_CameraPos - Input.WorldPos);
	vec3 result = vec3(0.0);


    for (int i = 0; i < u_NumLights; i++) 
    {
        vec3 lightDir = normalize(u_Lights[i].position - Input.WorldPos);
        vec3 viewDir = normalize(u_CameraPos - Input.WorldPos);

        float NdotL = max(dot(normal, lightDir), 0.0);  // Only proceed with lighting calculations if the normal is facing the light
        if(NdotL > 0.0) 
        {
            if (u_Lights[i].type == 0) 
            {
                 result += calculatePhongLighting(u_Lights[i], Input.WorldPos, normal, viewDir);
            } 
            else if (u_Lights[i].type == 1) 
            {
                
            } 
            else if (u_Lights[i].type == 2) 
            {
                
            }
        }
    }

    result *= albedo;

    color = (vec4(ambient, 1.0) + vec4(result, 1.0));
	entity = v_EntityID;
}