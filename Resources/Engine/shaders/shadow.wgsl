struct VertexInput 
{
	@location(0) position: vec3f,
	@location(1) normal: vec3f,
	@location(2) uv: vec2f,
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

  let worldPos = (uModel.model * vec4f(in.position, 1.0)).xyz;
  let worldNormal = normalize((uModel.model * vec4f(in.normal, 0.0)).xyz);

  let biasAmount = 0.0000; 
  let biasedWorldPos = worldPos - (worldNormal * biasAmount);

  out.position = uShadow.lightViewProj * vec4f(biasedWorldPos, 1.0);
  return out;
}