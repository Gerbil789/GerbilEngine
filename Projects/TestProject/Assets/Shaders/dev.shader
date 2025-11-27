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
	metalic: f32,
	roughness: u32,
	_padding: vec2f,
};

@group(0) @binding(0) var<uniform> uFrameUniforms: FrameUniforms;
@group(1) @binding(0) var<uniform> uModelUniforms: ModelUniforms;
@group(2) @binding(0) var<uniform> uMaterialUniforms: MaterialUniforms;
@group(2) @binding(1) var Sampler: sampler;
@group(2) @binding(2) var albedoTexture: texture_2d<f32>;
@group(2) @binding(3) var ambientTexture: texture_2d<f32>;

struct VertexInput {
	@location(0) position: vec3f,
	@location(1) normal: vec3f,
	@location(2) uv: vec2f,
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) uv: vec2f,
};

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput; 
	out.position = uFrameUniforms.projection * uFrameUniforms.view * uModelUniforms.model * vec4f(in.position, 1.0);
	out.uv = in.uv;
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
	let texelCoords = vec2i(in.uv * vec2f(textureDimensions(albedoTexture)));
  let color = textureLoad(albedoTexture, texelCoords, 0).rgb * uMaterialUniforms.color.rgb;
	let corrected_color = pow(color, vec3f(2.2));
	return vec4f(corrected_color, 1.0);
}