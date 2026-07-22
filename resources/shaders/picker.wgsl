struct VertexInput 
{
    @location(0) position: vec3f,
    @location(1) normal: vec3f,
    @location(2) uv: vec2f,
};

struct VertexOutput 
{
    @builtin(position) position: vec4f,
		// Integer attributes passed between stages must be decorated with @interpolate(flat)
    @location(0) @interpolate(flat) instanceIdx: u32,
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

struct IdBuffer
{
    ids: array<vec2<u32>>,
};

@group(0) @binding(0) var<uniform> uView: ViewUniforms;
@group(1) @binding(0) var<storage, read> uModelData: ModelBuffer;
@group(2) @binding(0) var<storage, read> uIdData: IdBuffer;

@vertex
fn vs_main(in: VertexInput, @builtin(instance_index) instanceIdx: u32) -> VertexOutput 
{
    var out: VertexOutput; 
    
    let modelMatrix = uModelData.models[instanceIdx];
    out.position = uView.projection * uView.view * modelMatrix * vec4f(in.position, 1.0);
    
    out.instanceIdx = instanceIdx;
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec2<u32> 
{
    return uIdData.ids[in.instanceIdx];
}