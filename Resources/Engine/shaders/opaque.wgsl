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
	@location(1) worldPos: vec3f,
	@location(2) worldNormal: vec3f,
};

struct FrameUniforms 
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

const MAX_POINT_LIGHTS = 32u;
struct Light 
{
	position: vec3f,
	range: f32,
	color: vec3f,
	intensity: f32,
};

struct LightUniforms 
{
	pointLightCount: u32,
	lights: array<Light, MAX_POINT_LIGHTS>,
};

@group(0) @binding(0) var<uniform> uFrame: FrameUniforms;
@group(1) @binding(0) var<uniform> uModel: ModelUniforms;
@group(2) @binding(0) var<uniform> uLights: LightUniforms;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput 
{
	var out: VertexOutput;

	let worldPos = uModel.model * vec4f(in.position, 1.0);
	out.position = uFrame.projection * uFrame.view * worldPos;

	out.worldPos = worldPos.xyz;
	out.worldNormal = (uModel.model * vec4f(in.normal, 0.0)).xyz;

	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f 
{
	let N = normalize(in.worldNormal);
	let V = normalize(uFrame.cameraPosition - in.worldPos);


	var lighting = vec3f(0.0);

	for (var i: u32 = 0u; i < uLights.pointLightCount; i++) {
		let light = uLights.lights[i];

		let L = light.position - in.worldPos;
		let dist = length(L);

		if (dist > light.range) {
			continue;
		}

		let Ldir = L / dist;
		let attenuation = 1.0 - (dist / light.range);

		let diff = max(dot(N, Ldir), 0.0);

		let R = reflect(-Ldir, N);
		let spec = pow(max(dot(R, V), 0.0), 32.0);

		let lightColor = light.color * light.intensity * attenuation;
		lighting += lightColor * (diff + spec);
	}

	const ambient = vec3f(0.05, 0.05, 0.05);
	let finalColor = ambient + lighting;

	return vec4f(finalColor, 1.0);
}