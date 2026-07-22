struct VertexOutput 
{
    @builtin(position) position: vec4f,
    @location(0) uv: vec2f,
};

struct UIUniforms 
{
    ortho: mat4x4f,
};

struct UIInstance 
{
    rect: vec4f,
    color: vec4f,
		uvRect: vec4f,
};

@group(0) @binding(0) var<uniform> uOrtho: UIUniforms;
@group(0) @binding(1) var<storage, read> uModelData: array<UIInstance>;

// material
@group(1) @binding(0) var uSampler: sampler;
@group(1) @binding(1) var uTexture: texture_2d<f32>;

@vertex
fn vs_main(
    @builtin(vertex_index) vertexIdx: u32,
    @builtin(instance_index) instanceIdx: u32,
) -> VertexOutput 
{
    var pos = array<vec2f, 6>(
        vec2f(0.0, 0.0), vec2f(1.0, 0.0), vec2f(0.0, 1.0),
        vec2f(0.0, 1.0), vec2f(1.0, 0.0), vec2f(1.0, 1.0)
    );

    let rect = uModelData[instanceIdx].rect;
    let color = uModelData[instanceIdx].color;
    let uvRect = uModelData[instanceIdx].uvRect;

    let finalPos = pos[vertexIdx] * rect.zw + rect.xy;
    
    var out: VertexOutput;
    out.position = uOrtho.ortho * vec4f(finalPos, 0.0, 1.0);
    out.uv = pos[vertexIdx] * uvRect.zw + uvRect.xy;
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f 
{
  return textureSample(uTexture, uSampler, in.uv);
}