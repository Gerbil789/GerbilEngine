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

const NUM_CASCADES: i32 = 4;

struct ShadowUniforms 
{
	lightViewProj : array<mat4x4f, NUM_CASCADES>,
	cascadeSplits : array<f32, NUM_CASCADES>,
};

const PI: f32 = 3.14159265;

@group(0) @binding(0) var<uniform> uFrame: FrameUniforms;

@group(0) @binding(1) var EnvSampler: sampler;
@group(0) @binding(2) var IrradianceMap: texture_2d<f32>;
@group(0) @binding(3) var BRDFIntMap: texture_2d<f32>;
@group(0) @binding(4) var PrefilteredEnvMap: texture_2d<f32>;

@group(0) @binding(5) var shadowMap : texture_depth_2d_array;
@group(0) @binding(6) var shadowSampler : sampler_comparison;
@group(0) @binding(7) var<uniform> uShadow : ShadowUniforms;

@group(0) @binding(8) var<uniform> uLight: LightUniforms;


@group(1) @binding(0) var<uniform> uModel: ModelUniforms;

@group(2) @binding(0) var<uniform> uMaterial: MaterialUniforms;
@group(2) @binding(1) var Sampler: sampler;
@group(2) @binding(2) var AlbedoTexture: texture_2d<f32>;
@group(2) @binding(3) var NormalTexture: texture_2d<f32>;
@group(2) @binding(4) var RoughnessTexture: texture_2d<f32>;

fn DirectionToEquirectUV(dir: vec3f) -> vec2f
{
  let d = normalize(dir);

  let phi = atan2(d.z, d.x);
  let theta = acos(d.y);

  let u = phi / (2.0 * PI) + 0.5;
  let v = theta / PI;

  return vec2f(u, v);
}

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
	// var cascadeIndex: i32 = 0;
	// let depth = in.viewDepth;

	// for (var i: i32 = 0; i < NUM_CASCADES; i = i + 1)
	// {
  //   if (depth < uShadow.cascadeSplits[i])
  //   {
  //       cascadeIndex = i;
  //       break;
  //   }
	// }




// 	if(cascadeIndex == 0)
// 	{
// 		  return vec4f(1.0, 1.0, 1.0, uMaterial.albedo.a);
// 	}

// 	if(cascadeIndex == 1)
// 	{
// 		  return vec4f(1.0, 0.0, 0.0, uMaterial.albedo.a);
// 	}

// 	if(cascadeIndex == 2)
// 	{
// 		  return vec4f(0.0, 1.0, 0.0, uMaterial.albedo.a);
// 	}

// 	if(cascadeIndex == 3)
// 	{
// 		  return vec4f(0.0, 0.0, 1.0, uMaterial.albedo.a);
// 	}

// return vec4f(1.0, 0.0, 1.0, uMaterial.albedo.a);





// 	let lightPos4 = uShadow.lightViewProj[cascadeIndex] * vec4f(in.worldPos, 1.0);
// 	let lightPos = lightPos4.xyz / lightPos4.w;


// 	let light_uv = vec2f(
//     (lightPos.x + 1.0) * 0.5,
//     -(lightPos.y - 1.0) * 0.5
// 	);

// 	let visibility = textureSampleCompare(
//     shadowMap,
//     shadowSampler,
//     light_uv,
//     cascadeIndex,
//     lightPos.z
// 	);



// 	// check bounds
// let inBounds =
//     light_uv.x >= 0.0 && light_uv.x <= 1.0 &&
//     light_uv.y >= 0.0 && light_uv.y <= 1.0;

// 	// shadow mask
// 	let shadow = select(1.0, visibility, inBounds);

//   let uv = in.uv * uMaterial.tiling;
//   let albedoTex = textureSample(AlbedoTexture, Sampler, uv).rgb;
//   let albedo = albedoTex * uMaterial.albedo.rgb;
   let N = normalize(in.normal);
   let V = normalize(uFrame.cameraPosition - in.worldPos);
//   let NdotV = max(dot(N, V), 0.0);
//   let F0 = mix(vec3f(0.04), albedo, uMaterial.metallic);
//   let F = FresnelSchlick(NdotV, F0);
//   let Fd = (vec3f(1.0) - F) * (1.0 - uMaterial.metallic);
//   let hdr_uv = DirectionToEquirectUV(N);
//   let irradiance = textureSample(IrradianceMap, EnvSampler, hdr_uv).rgb;
//   let diffuse = Fd * (albedo / PI) * irradiance;
   let R = reflect(-V, N);
   let specUV = DirectionToEquirectUV(R);

	 

	//let roughness = textureSample(RoughnessTexture, Sampler, uv).r; * uMaterial.roughness;
	let roughness = uMaterial.roughness;
	// let maxMipLevel: f32 = 6.0;
	// let lod = pow(roughness, 2.0) * maxMipLevel;

	return vec4f(textureSampleLevel(PrefilteredEnvMap, EnvSampler, specUV, 6.0));

// sample directly from mip chain
	// let prefiltered = textureSampleLevel(PrefilteredEnvMap, EnvSampler, specUV, 8.0).rgb;



  // let brdf = textureSample(BRDFIntMap, EnvSampler, vec2(NdotV, roughness)).rg;

  // let specular = prefiltered * (F * brdf.r + brdf.g);

  // let color = diffuse * shadow + specular;

  // return vec4f(color, uMaterial.albedo.a);
}