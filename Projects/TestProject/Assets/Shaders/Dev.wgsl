struct VertexInput {
	@location(0) position: vec3f,
	@location(1) normal: vec3f,
	@location(2) uv: vec2f,
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) uv: vec2f,
	@location(1) normal: vec3f,
	@location(2) worldPos: vec3f,
	@location(3) lightPos: vec4f,
};

struct FrameUniforms {
	view: mat4x4f,
	projection: mat4x4f,
	cameraPosition: vec3f,	
	_padding: f32,
};

struct ModelUniforms {
	model: mat4x4f,
};

struct MaterialUniforms {
	albedo: vec4f,
	roughness: f32,
	metallic: f32,
	tiling: vec2f,
};

struct Light {
	lightViewProj : mat4x4f,
};

const PI: f32 = 3.14159265;

@group(0) @binding(0) var<uniform> uFrame: FrameUniforms;
@group(0) @binding(1) var EnvSampler: sampler;
@group(0) @binding(2) var IrradianceMap: texture_2d<f32>;
@group(0) @binding(3) var BRDFIntMap: texture_2d<f32>;
// @group(0) @binding(4) var PrefilteredEnvMap: array<texture_2d<f32>, 9>;
@group(0) @binding(4) var PrefilteredEnvMap0: texture_2d<f32>;
@group(0) @binding(5) var PrefilteredEnvMap1: texture_2d<f32>;
@group(0) @binding(6) var PrefilteredEnvMap2: texture_2d<f32>;
@group(0) @binding(7) var PrefilteredEnvMap3: texture_2d<f32>;
@group(0) @binding(8) var PrefilteredEnvMap4: texture_2d<f32>;
@group(0) @binding(9) var PrefilteredEnvMap5: texture_2d<f32>;
@group(0) @binding(10) var PrefilteredEnvMap6: texture_2d<f32>;
@group(0) @binding(11) var PrefilteredEnvMap7: texture_2d<f32>;
@group(0) @binding(12) var PrefilteredEnvMap8: texture_2d<f32>;

@group(1) @binding(0) var<uniform> uModel: ModelUniforms;

@group(2) @binding(0) var<uniform> uMaterial: MaterialUniforms;
@group(2) @binding(1) var Sampler: sampler;
@group(2) @binding(2) var AlbedoTexture: texture_2d<f32>;


@group(0) @binding(13) var shadowMap : texture_depth_2d;
@group(0) @binding(14) var shadowSampler : sampler_comparison;
@group(0) @binding(15) var<uniform> uShadow : Light;



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

fn SamplePrefilter(level: i32, uv: vec2<f32>) -> vec3<f32> {
    switch(level) {
        case 0: { return textureSample(PrefilteredEnvMap0, EnvSampler, uv).rgb; }
        case 1: { return textureSample(PrefilteredEnvMap1, EnvSampler, uv).rgb; }
        case 2: { return textureSample(PrefilteredEnvMap2, EnvSampler, uv).rgb; }
        case 3: { return textureSample(PrefilteredEnvMap3, EnvSampler, uv).rgb; }
        case 4: { return textureSample(PrefilteredEnvMap4, EnvSampler, uv).rgb; }
        case 5: { return textureSample(PrefilteredEnvMap5, EnvSampler, uv).rgb; }
        case 6: { return textureSample(PrefilteredEnvMap6, EnvSampler, uv).rgb; }
        case 7: { return textureSample(PrefilteredEnvMap7, EnvSampler, uv).rgb; }
        default: { return textureSample(PrefilteredEnvMap8, EnvSampler, uv).rgb; }
    }
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


  out.lightPos = uShadow.lightViewProj * worldPos;

	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f
{



// return vec4f(vec3f(in.position.z), 1.0);
// return vec4f(in.uv, 0.0, 1.0);
// let proj = in.lightPos.xyz / in.lightPos.w;
// let uv = proj.xy * 0.5 + 0.5;
// let depth = textureSampleLevel(shadowMap, shadowSampler, uv, 0.0);
// return vec4f(vec3f(depth), 1.0);

	let texSize = textureDimensions(shadowMap);
	let coord = vec2<i32>(in.uv * vec2<f32>(texSize));
	let depth = textureLoad(shadowMap, coord, 0);

	return vec4f(vec3f(depth), 1.0);

	 //let depthSample = textureSample(shadowMap, shadowSampler, in.uv);
	 //return vec4f(vec3f(depthSample), 1.0);


  // // --- 1. perspective divide ---
  //   let proj = in.lightPos.xyz / in.lightPos.w;

  //   // --- 2. convert to [0,1] ---
  //   let light_uv = proj.xy * 0.5 + 0.5;
  //   let depth = proj.z * 0.5 + 0.5;

  //   // --- 4. bias (important) ---
  //   let bias = 0.003;

  //   // --- 5. depth comparison ---
  //   let visibility = textureSampleCompare(
  //       shadowMap,
  //       shadowSampler,
  //       light_uv,
  //       depth - bias
  //   );

	// 	// compute mask instead of branching
	// 	let inBounds =
  //   	light_uv.x >= 0.0 && light_uv.x <= 1.0 &&
  //   	light_uv.y >= 0.0 && light_uv.y <= 1.0;

	// 	// apply mask
	// 	let shadow = select(1.0, visibility, inBounds);


  //   let uv = in.uv * uMaterial.tiling;

  //   let albedoTex = textureSample(AlbedoTexture, Sampler, uv).rgb;
  //   let albedo = albedoTex * uMaterial.albedo.rgb;

  //   let N = normalize(in.normal);
  //   let V = normalize(uFrame.cameraPosition - in.worldPos);
  //   let NdotV = max(dot(N, V), 0.0);

  //   let F0 = mix(vec3f(0.04), albedo, uMaterial.metallic);
  //   let F = FresnelSchlick(NdotV, F0);
  //   let Fd = (vec3f(1.0) - F) * (1.0 - uMaterial.metallic);

  //   let hdr_uv = DirectionToEquirectUV(N);
  //   let irradiance = textureSample(IrradianceMap, EnvSampler, hdr_uv).rgb;
  //   let diffuse = Fd * (albedo / PI) * irradiance;


  //   let R = reflect(-V, N);
  //   let specUV = DirectionToEquirectUV(R);
  //   //let prefiltered = textureSample(PrefilteredEnvMap[0], EnvSampler, specUV).rgb;


	// let levelCount: f32 = 9.0;
	// let index = i32(clamp(floor(pow(uMaterial.roughness, 2) * levelCount), 0.0, levelCount - 1.0));

	// let prefiltered = SamplePrefilter(index, specUV);

  //   let brdf = textureSample(BRDFIntMap, EnvSampler, vec2(NdotV, uMaterial.roughness)).rg;

  //   let specular = prefiltered * (F * brdf.r + brdf.g);

  //   let color = (diffuse + specular) * shadow;

  //   return vec4f(color, uMaterial.albedo.a);
}