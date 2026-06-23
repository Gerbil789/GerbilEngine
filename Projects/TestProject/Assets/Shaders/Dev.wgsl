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

struct ViewUniforms
{
	view: mat4x4f,
	projection: mat4x4f,
	cameraPosition: vec3f,
	_padding: f32,
};

struct ShadowUniforms
{
	lightViewProj : array<mat4x4f, NUM_SHADOW_CASCADES>,
	cascadeSplits : array<f32, NUM_SHADOW_CASCADES>,
};

struct MaterialUniforms 
{
	albedo: vec4f, 	// @color @default(1,1,1,1)
	roughness: f32,	// @range(0,1) @default(1)
	metallic: f32,	// @range(0,1) @default(0)
	tiling: vec2f,	// @default(1,1)
	offset: vec2f,	// @default(0,0)
	_padding: vec2f,
};

// storage buffer
struct ModelBuffer
{
  models: array<mat4x4f>,
};

//view
@group(0) @binding(0) var<uniform> uView: ViewUniforms;

//environment & shadow
@group(1) @binding(0) var EnvironmentSampler: sampler;
@group(1) @binding(1) var EnvironmentMap: texture_cube<f32>;
@group(1) @binding(2) var IrradianceMap: texture_cube<f32>;
@group(1) @binding(3) var PrefilteredSpecularMap: texture_cube<f32>;
@group(1) @binding(4) var BRDFIntMap: texture_2d<f32>;
@group(1) @binding(5) var<uniform> uShadow : ShadowUniforms;
@group(1) @binding(6) var ShadowSampler : sampler_comparison;
@group(1) @binding(7) var ShadowMap : texture_depth_2d_array;

//material
@group(2) @binding(0) var<uniform> uMaterial: MaterialUniforms;
@group(2) @binding(1) var MaterialSampler: sampler;
@group(2) @binding(2) var AlbedoTexture: texture_2d<f32>;
@group(2) @binding(3) var MetallicTexture: texture_2d<f32>;
@group(2) @binding(4) var NormalTexture: texture_2d<f32>;
@group(2) @binding(5) var RoughnessTexture: texture_2d<f32>;
@group(2) @binding(6) var AmbientTexture: texture_2d<f32>;

//model
@group(3) @binding(0) var<storage, read> uModelData: ModelBuffer;

fn FresnelSchlick(cosTheta: f32, F0: vec3f) -> vec3f
{
	return F0 + (vec3f(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

@vertex
fn vs_main(in: VertexInput, @builtin(instance_index) instanceIdx: u32) -> VertexOutput 
{
	var out: VertexOutput; 

	let modelMatrix = uModelData.models[instanceIdx];
  let worldPos = modelMatrix * vec4f(in.position, 1.0);

	out.position = uView.projection * uView.view * worldPos;
	out.normal = normalize((modelMatrix * vec4f(in.normal, 0.0)).xyz);
	out.uv = in.uv;
	out.worldPos = worldPos.xyz;

	let viewPos = uView.view * worldPos;
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

	let light_uv = vec2f((lightPos.x + 1.0) * 0.5, -(lightPos.y - 1.0) * 0.5);

	var visibility: f32 = 0.0;
	let texelSize = 1.0 / 1024.0; // 1024x1024 shadow texture

	// 3x3 PCF Kernel
	for (var y: i32 = -1; y <= 1; y++) 
	{
		for (var x: i32 = -1; x <= 1; x++) 
		{
			let offset = vec2f(f32(x), f32(y)) * texelSize;
	
			visibility += textureSampleCompare(
				ShadowMap,
				ShadowSampler,
				light_uv + offset,
				cascadeIndex,
				lightPos.z
			);
		}
	}

	visibility = visibility / 9.0; // Average the 9 samples

	// check bounds
	let inBounds =
		light_uv.x >= 0.0 && light_uv.x <= 1.0 &&
		light_uv.y >= 0.0 && light_uv.y <= 1.0;

	// shadow mask
	let shadow = select(1.0, visibility, inBounds);

	let uv = in.uv * uMaterial.tiling + uMaterial.offset;
	let albedo = textureSample(AlbedoTexture, MaterialSampler, uv).rgb * uMaterial.albedo.rgb;
	let metallic = textureSample(MetallicTexture, MaterialSampler, uv).r * uMaterial.metallic;
	let roughness = textureSample(RoughnessTexture, MaterialSampler, uv).r * uMaterial.roughness;
	let ao = textureSample(AmbientTexture, MaterialSampler, uv).r;
	let normal = in.normal;

	// Sample the normal map and unpack it from [0, 1] to [-1, 1]
	let sampledNormal = textureSample(NormalTexture, MaterialSampler, uv).rgb;
	let tangentNormal = sampledNormal * 2.0 - vec3f(1.0);

	// Calculate screen-space derivatives
	let dp1 = dpdx(in.worldPos);
	let dp2 = dpdy(in.worldPos);
	let duv1 = dpdx(uv);
	let duv2 = dpdy(uv);

	// Calculate Tangent and Bitangent vectors
	let dp2perp = cross(dp2, normal);
	let dp1perp = cross(normal, dp1);
	let T = dp2perp * duv1.x + dp1perp * duv2.x;
	let B = dp2perp * duv1.y + dp1perp * duv2.y;

	let invmax = inverseSqrt(max(dot(T,T), dot(B,B)));
	let TBN = mat3x3f(T * invmax, B * invmax, normal);

	let N = normalize(TBN * tangentNormal);
	let V = normalize(uView.cameraPosition - in.worldPos); // direction from surface to camera
	let R = reflect(-V, N);
	let NdotV = max(dot(N, V), 0.0);
	let F0 = mix(vec3f(0.04), albedo, metallic);
	let F = FresnelSchlick(NdotV, F0);
	let Fd = (vec3f(1.0) - F) * (1.0 - metallic);


	// return vec4f(N * 0.5 + 0.5, 1.0); //test normals

	let irradiance = textureSample(IrradianceMap, EnvironmentSampler, N).rgb;
	let diffuse = Fd * albedo * irradiance;

	let maxMipLevel = f32(textureNumLevels(PrefilteredSpecularMap)) - 1.0;
	let lod = pow(roughness, 2.0) * maxMipLevel;
	let env = textureSampleLevel(PrefilteredSpecularMap, EnvironmentSampler, R, lod).rgb;

	let brdf = textureSample(BRDFIntMap, EnvironmentSampler, vec2(NdotV, roughness)).rg;

	let specular = env * (F0 * brdf.r + brdf.g);

	let color = diffuse * ao + specular * ao;
	// let color = ((diffuse + specular) * shadow) * ao;
	
	let gammaCorrected = pow(color, vec3f(1.0 / 2.2));
	return vec4f(gammaCorrected, uMaterial.albedo.a);

	//return vec4f(vec3f(maxMipLevel / 10.0), 1.0);
	//return vec4f(vec3f(roughness), 1.0);
	//return vec4f(vec3f(lod / 10.0), 1.0);

	//return vec4f(textureSampleLevel(PrefilteredSpecularMap, EnvironmentSampler, R, 6.0).rgb, 1.0);
}