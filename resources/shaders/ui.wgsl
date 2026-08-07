struct VertexOutput 
{
  @builtin(position) position: vec4f,
  @location(0) uv: vec2f,
	@location(1) tint: vec4f,
};

struct ViewUniforms 
{
  ortho: mat4x4f,
};

struct DrawItem 
{
  rect: vec4f,
  tint: vec4f,
	uvRect: vec4f,
};

@group(0) @binding(0) var<uniform> u_View: ViewUniforms;
@group(0) @binding(1) var<storage, read> s_DrawList: array<DrawItem>;
@group(0) @binding(2) var Sampler: sampler;
@group(0) @binding(3) var Atlas: texture_2d<f32>;

@vertex
fn vs_main(@builtin(vertex_index) vertexIdx: u32, @builtin(instance_index) instanceIdx: u32,) -> VertexOutput 
{
  var pos = array<vec2f, 6>(
    vec2f(0.0, 0.0), vec2f(1.0, 0.0), vec2f(0.0, 1.0),
    vec2f(0.0, 1.0), vec2f(1.0, 0.0), vec2f(1.0, 1.0)
  );

  let rect = s_DrawList[instanceIdx].rect;
  let tint = s_DrawList[instanceIdx].tint;
  let uvRect = s_DrawList[instanceIdx].uvRect;

  let finalPos = pos[vertexIdx] * rect.zw + rect.xy;
    
  var out: VertexOutput;
  out.position = u_View.ortho * vec4f(finalPos, 0.0, 1.0);
  out.uv = pos[vertexIdx] * uvRect.zw + uvRect.xy;
  out.tint = tint;
  return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f 
{
  return textureSample(Atlas, Sampler, in.uv) * in.tint;
}