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

const PI: f32 = 3.14159265;

@group(0) @binding(0) var<uniform> uFrame: FrameUniforms;
@group(0) @binding(1) var EnvSampler: sampler;
@group(0) @binding(2) var IrradianceMap: texture_2d<f32>;
@group(0) @binding(3) var BRDFIntMap: texture_2d<f32>;
@group(0) @binding(4) var PrefilteredEnvMap: texture_2d<f32>;
@group(1) @binding(0) var<uniform> uModel: ModelUniforms;
@group(2) @binding(0) var<uniform> uMaterial: MaterialUniforms;
@group(2) @binding(1) var Sampler: sampler;
@group(2) @binding(2) var AlbedoTexture: texture_2d<f32>;

fn DirectionToEquirectUV(dir: vec3f) -> vec2f
{
    let d = normalize(dir);

    let phi = atan2(d.z, d.x);
    let theta = acos(clamp(d.y, -1.0, 1.0));

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
	out.position = uFrame.projection * uFrame.view * uModel.model * vec4f(in.position, 1.0);
	out.normal = normalize((uModel.model * vec4f(in.normal, 0.0)).xyz);
	out.uv = in.uv;
	out.worldPos = (uModel.model * vec4f(in.position,1)).xyz;
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f 
{
    let uv = in.uv * uMaterial.tiling;

    let albedoTex = textureSample(AlbedoTexture, Sampler, uv).rgb;
    let albedo = albedoTex * uMaterial.albedo.rgb;

    let N = normalize(in.normal);
    let V = normalize(uFrame.cameraPosition - in.worldPos);
    let NdotV = max(dot(N, V), 0.0);

    // base reflectivity
    let F0 = mix(vec3f(0.04), albedo, uMaterial.metallic);

    // Fresnel
    let F = FresnelSchlick(NdotV, F0);

    // diffuse energy conservation
    let Fd = (vec3f(1.0) - F) * (1.0 - uMaterial.metallic);

    // ---------- DIFFUSE IBL ----------
    let irradianceUV = DirectionToEquirectUV(N);
    let irradiance = textureSample(IrradianceMap, EnvSampler, irradianceUV).rgb;

    let diffuse = Fd * (albedo / PI) * irradiance;

    // ---------- SPECULAR IBL ----------
    let R = reflect(-V, N);
    let specUV = DirectionToEquirectUV(R);

    // prefiltered environment lookup
    let prefiltered = textureSample(PrefilteredEnvMap, EnvSampler, specUV).rgb;

    // BRDF lookup
    let brdf = textureSample(
        BRDFIntMap,
        EnvSampler,
        vec2(NdotV, uMaterial.roughness)
    ).rg;

    let specular = prefiltered * (F * brdf.x + brdf.y);

    let color = diffuse + specular;

    return vec4f(color, uMaterial.albedo.a);
}