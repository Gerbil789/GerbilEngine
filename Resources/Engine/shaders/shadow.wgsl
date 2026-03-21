struct VertexInput 
{
	@location(0) position: vec3f,
};

struct VertexOutput 
{
	@builtin(position) position: vec4f,
};

struct ModelUniforms 
{
	model: mat4x4f,
};

struct ShadowUniforms
{
  lightViewProj : mat4x4<f32>,
};

@group(0) @binding(0) var<uniform> uShadow : ShadowUniforms;
@group(1) @binding(0) var<uniform> uModel: ModelUniforms;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput 
{
	var out: VertexOutput;

	let worldPos = uModel.model * vec4<f32>(in.position, 1.0);
  out.position = uShadow.lightViewProj * worldPos;

	return out;
}