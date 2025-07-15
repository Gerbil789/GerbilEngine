struct VertexInput {
	@location(0) position: vec3f,
	@location(1) normal: vec3f,
	@location(2) uv: vec2f,
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) color: vec3f,
};

struct ModelUniforms {
	model: mat4x4f,
};

struct FrameUniforms {
	projection: mat4x4f,
	view: mat4x4f,
	cameraPosition: vec3f,	
	_padding: f32,
};

struct MaterialUniforms {
	color: vec4f,
};

@group(0) @binding(0) var<uniform> uModelUniforms: ModelUniforms;
@group(1) @binding(0) var<uniform> uFrameUniforms: FrameUniforms;
@group(2) @binding(0) var<uniform> uMaterialUniforms: MaterialUniforms;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput; 
	out.position = uFrameUniforms.projection * uFrameUniforms.view * uModelUniforms.model * vec4f(in.position, 1.0);
	out.color = vec3f(in.uv.x, in.uv.y, 0);
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
	return vec4f(in.color, 1.0);
}