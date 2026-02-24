struct VertexInput {
	@location(0) position: vec3f,
	@location(1) normal: vec3f,
	@location(2) uv: vec2f,
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) uv: vec2f,
	@location(1) normal: vec3f,
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
	color: vec4f,
	_pad: vec3f,
	brightness: f32,

};

@group(0) @binding(0) var<uniform> uFrame: FrameUniforms;
@group(1) @binding(0) var<uniform> uModel: ModelUniforms;
@group(2) @binding(0) var<uniform> uMaterial: MaterialUniforms;
@group(2) @binding(1) var Sampler: sampler;
@group(2) @binding(2) var Albedo: texture_2d<f32>;
@group(2) @binding(3) var Ambient: texture_2d<f32>;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput 
{
	var out: VertexOutput; 
	out.position = uFrame.projection * uFrame.view * uModel.model * vec4f(in.position, 1.0);
	out.normal = normalize((uModel.model * vec4f(in.normal, 0.0)).xyz);
	out.uv = in.uv;
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f 
{
	let amb = textureSample(Ambient, Sampler, in.uv).rgb * uMaterial.brightness;
  let color = textureSample(Albedo, Sampler, in.uv).rgb * uMaterial.color.rgb + amb;
  return vec4f(color, uMaterial.color.a);
}

