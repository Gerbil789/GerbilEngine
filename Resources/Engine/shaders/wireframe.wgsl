struct VertexInput {
	@location(0) position: vec3f,
	@location(1) normal: vec3f,
	@location(2) uv: vec2f,
};

struct VertexOutput {
	@builtin(position) position: vec4f,
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

struct WireframeUniforms {
    color: vec4f,
};


@group(0) @binding(0) var<uniform> uFrameUniforms: FrameUniforms;
@group(1) @binding(0) var<uniform> uModelUniforms: ModelUniforms;
@group(2) @binding(0) var<uniform> uWireframe: WireframeUniforms;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput; 
	out.position = uFrameUniforms.projection * uFrameUniforms.view * uModelUniforms.model * vec4f(in.position, 1.0);
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
	return uWireframe.color;
}