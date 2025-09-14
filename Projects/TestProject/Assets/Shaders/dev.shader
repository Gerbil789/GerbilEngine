vertexAttributes:
  - { location: 0, label: "position", format: "Float32x3" }
  - { location: 1, label: "normal",   format: "Float32x3" }
  - { location: 2, label: "uv",       format: "Float32x2" }

bindGroups:
  - group: 0
    label: "frame"
    bindings:
      - { binding: 0, label: "frameUniforms",   type: "uniform-buffer", stages: "vertex|fragment" }

  - group: 1
    label: "model"
    bindings:
      - { binding: 0, label: "modelUniforms",   type: "uniform-buffer", stages: "vertex" }

  - group: 2
    label: "material"
    bindings:
      - { binding: 0, label: "materialUniforms", type: "uniform-buffer", stages: "fragment" }

vsEntry: "vs_main"
fsEntry: "fs_main"

#SHADER

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

struct MaterialUniforms {
	color: vec4f,
};

@group(0) @binding(0) var<uniform> uFrameUniforms: FrameUniforms;
@group(1) @binding(0) var<uniform> uModelUniforms: ModelUniforms;
@group(2) @binding(0) var<uniform> uMaterialUniforms: MaterialUniforms;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput; 
	out.position = uFrameUniforms.projection * uFrameUniforms.view * uModelUniforms.model * vec4f(in.position, 1.0);
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
	return uMaterialUniforms.color;
}