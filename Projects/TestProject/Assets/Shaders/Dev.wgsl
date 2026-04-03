const NUM_SHADOW_CASCADES: i32 = 4;
const PI: f32 = 3.14159265;

struct VertexInput
{
	@location(0) position: vec3f,
	@location(1) normal: vec3f,
	@location(2) uv: vec2f,
};

struct VertexOutput
{
	@builtin(position) position: vec4f,
	@location(0) uv: vec2f,
	@location(1) normal: vec3f,
	@location(2) worldPos: vec3f,
	@location(3) viewDepth: f32,
};

struct FrameUniforms 
{
	view: mat4x4f,
	projection: mat4x4f,
	cameraPosition: vec3f,
	_padding: f32,
};

// uses dynamic offset
struct ModelUniforms 
{
	model: mat4x4f,
};

struct MaterialUniforms 
{
	albedo: vec4f,
	roughness: f32,
	metallic: f32,
	tiling: vec2f,
};

const LIGHT_TYPE_DIRECTIONAL: u32 = 0u;
const LIGHT_TYPE_POINT: u32       = 1u;
const LIGHT_TYPE_SPOT: u32        = 2u;

struct LightUniforms
{
    position: vec3f,   // point/spot
    lightType: u32,
    direction: vec3f,  // directional/spot
    range: f32,        // point/spot
    color: vec3f,
    intensity: f32,
    spotAngle: f32,    // spot (cos angle or radians)
    spotBlend: f32,    // soft edge
    _pad: vec2f,
};


struct ShadowUniforms 
{
	lightViewProj : array<mat4x4f, NUM_SHADOW_CASCADES>,
	cascadeSplits : array<f32, NUM_SHADOW_CASCADES>,
};

@group(0) @binding(0) var<uniform> uFrame: FrameUniforms;
@group(0) @binding(1) var EnvSampler: sampler;
@group(0) @binding(2) var IrradianceMap: texture_cube<f32>;
@group(0) @binding(3) var BRDFIntMap: texture_2d<f32>;
@group(0) @binding(4) var PrefilteredEnvMap: texture_cube<f32>;
@group(0) @binding(5) var shadowMap : texture_depth_2d_array;
@group(0) @binding(6) var shadowSampler : sampler_comparison;
@group(0) @binding(7) var<uniform> uShadow : ShadowUniforms;
@group(0) @binding(8) var<uniform> uLight: LightUniforms;

@group(1) @binding(0) var<uniform> uModel: ModelUniforms;

@group(2) @binding(0) var<uniform> uMaterial: MaterialUniforms;
@group(2) @binding(1) var MaterialSampler: sampler;
@group(2) @binding(2) var AlbedoTexture: texture_2d<f32>;
@group(2) @binding(3) var NormalTexture: texture_2d<f32>;
@group(2) @binding(4) var RoughnessTexture: texture_2d<f32>;

fn FresnelSchlick(cosTheta: f32, F0: vec3f) -> vec3f
{
  return F0 + (vec3f(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

@vertex
fn vs_main(in: VertexInput) -> VertexOutput 
{
	var out: VertexOutput; 

	let worldPos = uModel.model * vec4f(in.position, 1.0);

	out.position = uFrame.projection * uFrame.view * worldPos;
	out.normal = normalize((uModel.model * vec4f(in.normal, 0.0)).xyz);
	out.uv = in.uv;
	out.worldPos = worldPos.xyz;


	let viewPos = uFrame.view * worldPos;
	out.viewDepth = viewPos.z;
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f
{
	var cascadeIndex: i32 = 0;
	let depth = in.viewDepth;

	for (var i: i32 = 0; i < NUM_SHADOW_CASCADES; i = i + 1)
	{
    if (depth < uShadow.cascadeSplits[i])
    {
        cascadeIndex = i;
        break;
    }
	}

	let lightPos4 = uShadow.lightViewProj[cascadeIndex] * vec4f(in.worldPos, 1.0);
	let lightPos = lightPos4.xyz / lightPos4.w;


	let light_uv = vec2f(
    (lightPos.x + 1.0) * 0.5,
    -(lightPos.y - 1.0) * 0.5
	);

	let visibility = textureSampleCompare(
    shadowMap,
    shadowSampler,
    light_uv,
    cascadeIndex,
    lightPos.z
	);



	// check bounds
	let inBounds =
    light_uv.x >= 0.0 && light_uv.x <= 1.0 &&
    light_uv.y >= 0.0 && light_uv.y <= 1.0;

	// shadow mask
	let shadow = select(1.0, visibility, inBounds);

  let uv = in.uv * uMaterial.tiling;
  let albedoTex = textureSample(AlbedoTexture, MaterialSampler, uv).rgb;
  let albedo = albedoTex * uMaterial.albedo.rgb;

	// 1. Get the base geometry normal
	let geomNormal = normalize(in.normal);

	// 2. Sample the normal map and unpack it from [0, 1] to [-1, 1]
	let sampledNormal = textureSample(NormalTexture, MaterialSampler, uv).rgb;
	let tangentNormal = sampledNormal * 2.0 - vec3f(1.0);

	// 3. Calculate screen-space derivatives
	let dp1 = dpdx(in.worldPos);
	let dp2 = dpdy(in.worldPos);
	let duv1 = dpdx(uv);
	let duv2 = dpdy(uv);

	// 4. Calculate Tangent and Bitangent vectors
	let dp2perp = cross(dp2, geomNormal);
	let dp1perp = cross(geomNormal, dp1);
	let T = dp2perp * duv1.x + dp1perp * duv2.x;
	let B = dp2perp * duv1.y + dp1perp * duv2.y;

	// 5. Construct the TBN (Tangent, Bitangent, Normal) matrix
	let invmax = inverseSqrt(max(dot(T,T), dot(B,B)));
	let TBN = mat3x3f(T * invmax, B * invmax, geomNormal);

	// 6. Transform the tangent-space normal to world-space
	let N = normalize(TBN * tangentNormal);


	let roughness = textureSample(RoughnessTexture, MaterialSampler, uv).r * uMaterial.roughness;

  let V = normalize(uFrame.cameraPosition - in.worldPos);
	let R = reflect(-V, N);
  let NdotV = max(dot(N, V), 0.0);
  let F0 = mix(vec3f(0.04), albedo, uMaterial.metallic);
  let F = FresnelSchlick(NdotV, F0);
  let Fd = (vec3f(1.0) - F) * (1.0 - uMaterial.metallic);
  let irradiance = textureSample(IrradianceMap, EnvSampler, N).rgb;
  let diffuse = Fd * (albedo / PI) * irradiance;


	let maxMipLevel = f32(textureNumLevels(PrefilteredEnvMap)) - 1.0;
	let lod = pow(roughness, 2.0) * maxMipLevel;
	let env = textureSampleLevel(PrefilteredEnvMap, EnvSampler, R, lod).rgb;

  let brdf = textureSample(BRDFIntMap, EnvSampler, vec2(NdotV, roughness)).rg;

  let specular = env * (F0 * brdf.r + brdf.g);

  let color = diffuse + specular * shadow;
	let gammaCorrected = pow(color, vec3f(1.0 / 2.2));
  return vec4f(gammaCorrected, uMaterial.albedo.a);
}