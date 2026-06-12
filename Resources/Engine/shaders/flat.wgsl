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
};

struct ViewUniforms 
{
	view: mat4x4f,
	projection: mat4x4f,
	cameraPosition: vec3f,	
	_padding: f32,
};

struct ModelBuffer
{
  models: array<mat4x4f>,
};

struct MaterialUniforms 
{
	albedo: vec4f,
};

@group(0) @binding(0) var<uniform> uView: ViewUniforms;
@group(2) @binding(0) var<uniform> uMaterial: MaterialUniforms;
@group(3) @binding(0) var<storage, read> uModelData: ModelBuffer;

@vertex
fn vs_main(in: VertexInput, @builtin(instance_index) instanceIdx: u32) -> VertexOutput 
{
	var out: VertexOutput; 
	let modelMatrix = uModelData.models[instanceIdx];
	out.position = uView.projection * uView.view * modelMatrix * vec4f(in.position, 1.0);
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f 
{
	return uMaterial.albedo;
}