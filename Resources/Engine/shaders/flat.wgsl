struct VertexInput 
{
	@location(0) position: vec3f,
	@location(1) normal: vec3f,
	@location(2) uv: vec2f,
};

struct VertexOutput 
{
	@builtin(position) position: vec4f,
	@location(0) uv: vec2f,
};

struct ViewUniforms 
{
	view: mat4x4f,
	projection: mat4x4f,
	cameraPosition: vec3f,	
	_padding: f32,
};

struct ModelUniforms 
{
	model: mat4x4f,
};

struct MaterialUniforms 
{
	color: vec4f,
};

@group(0) @binding(0) var<uniform> uView: ViewUniforms;
//skip environment bindings
@group(2) @binding(0) var<uniform> uMaterial: MaterialUniforms;
@group(3) @binding(0) var<uniform> uModel: ModelUniforms;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput 
{
	var out: VertexOutput; 
	out.position = uView.projection * uView.view * uModel.model * vec4f(in.position, 1.0);
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f 
{
	return uMaterial.color;
}