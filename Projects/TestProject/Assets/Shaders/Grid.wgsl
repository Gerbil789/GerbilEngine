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
};

struct ViewUniforms
{
	view: mat4x4f,
	projection: mat4x4f,
	cameraPosition: vec3f,
	_padding: f32,
};

struct MaterialUniforms 
{
	albedo: vec4f,
	tiling: vec2f,
	offset: vec2f,
};

// storage buffer
struct ModelBuffer
{
  models: array<mat4x4f>,
};

//view
@group(0) @binding(0) var<uniform> uView: ViewUniforms;

//material
@group(2) @binding(0) var<uniform> uMaterial: MaterialUniforms;
@group(2) @binding(1) var MaterialSampler: sampler;
@group(2) @binding(2) var AlbedoTexture: texture_2d<f32>;

//model
@group(3) @binding(0) var<storage, read> uModelData: ModelBuffer;


@vertex
fn vs_main(in: VertexInput, @builtin(instance_index) instanceIdx: u32) -> VertexOutput 
{
	var out: VertexOutput; 

	let modelMatrix = uModelData.models[instanceIdx];
  let worldPos = modelMatrix * vec4f(in.position, 1.0);

	out.position = uView.projection * uView.view * worldPos;
	out.normal = normalize((modelMatrix * vec4f(in.normal, 0.0)).xyz);
	out.uv = in.uv;

	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f
{
	let uv = in.uv * uMaterial.tiling + uMaterial.offset;
	let albedo = textureSample(AlbedoTexture, MaterialSampler, uv).rgb * uMaterial.albedo.rgb;

	let color = uMaterial.albedo.rgb * albedo;

	let gammaCorrected = pow(color, vec3f(1.0 / 2.2));
	return vec4f(gammaCorrected, uMaterial.albedo.a);
}