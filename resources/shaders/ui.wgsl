struct VertexOutput 
{
  @builtin(position) position: vec4f,
  @location(0) uv: vec2f,
	@location(1) tint: vec4f,
	@location(2) @interpolate(flat) mode: u32,   
  @location(3) @interpolate(flat) fontIndex: u32,
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
	mode: u32,   // 0 = UI Image, 1 = Text
  _pad0: f32,
  _pad1: f32,
  _pad2: f32,
};

@group(0) @binding(0) var<uniform> u_View: ViewUniforms;
@group(0) @binding(1) var<storage, read> s_DrawList: array<DrawItem>;

@group(0) @binding(2) var TextureAtlas: texture_2d<f32>;
@group(0) @binding(3) var TextureSampler: sampler;

@group(0) @binding(4) var FontAtlas: texture_2d<f32>;
@group(0) @binding(5) var FontSampler: sampler;

@vertex
fn vs_main(@builtin(vertex_index) vertexIdx: u32, @builtin(instance_index) instanceIdx: u32,) -> VertexOutput 
{
  var pos = array<vec2f, 6>(
    vec2f(0.0, 0.0), vec2f(1.0, 0.0), vec2f(0.0, 1.0),
    vec2f(0.0, 1.0), vec2f(1.0, 0.0), vec2f(1.0, 1.0)
  );

	let item = s_DrawList[instanceIdx];

  let finalPos = pos[vertexIdx] * item.rect.zw + item.rect.xy;
    
  var out: VertexOutput;
  out.position = u_View.ortho * vec4f(finalPos, 0.0, 1.0);
  out.uv = pos[vertexIdx] * item.uvRect.zw + item.uvRect.xy;
  out.tint = item.tint;
	out.mode = item.mode;
  return out;
}

fn median(r: f32, g: f32, b: f32) -> f32 
{
  return max(min(r, g), min(max(r, g), b));
}


@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f 
{
	let ui_color = textureSample(TextureAtlas, TextureSampler, in.uv) * in.tint;
  let msd = textureSample(FontAtlas, FontSampler, in.uv);

	let sd = median(msd.r, msd.g, msd.b);

	let atlasPxRange = 4.0;
  let atlasSize = vec2f(1024.0, 1024.0);

	let unitRange = vec2f(atlasPxRange) / atlasSize;
  let screenTexSize = vec2f(1.0) / fwidth(in.uv);
  let screenPxRange = max(0.5 * dot(unitRange, screenTexSize), 1.0);

	let screenPxDistance = screenPxRange * (sd - 0.5);
  let opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

  if (in.mode == 0u) 
	{
    return ui_color;
  } 
	else 
	{
    if (opacity < 0.01) 
		{
      discard;
    }
		return vec4f(in.tint.rgb, in.tint.a * opacity);
  }
}