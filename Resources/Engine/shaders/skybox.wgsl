const NUM_SHADOW_CASCADES: i32 = 4;

struct VertexOutput 
{
	@builtin(position) position: vec4f,
	@location(0) dir: vec3f,
};

struct ViewUniforms 
{
	view: mat4x4f,
	projection: mat4x4f,
	cameraPosition: vec3f,	
	_padding: f32,
};

struct ShadowUniforms
{
	lightViewProj : array<mat4x4f, NUM_SHADOW_CASCADES>,
	cascadeSplits : array<f32, NUM_SHADOW_CASCADES>,
};

//view
@group(0) @binding(0) var<uniform> uView: ViewUniforms;

//environment & shadow
@group(1) @binding(0) var EnvironmentSampler: sampler;
@group(1) @binding(1) var EnvironmentMap: texture_cube<f32>;
@group(1) @binding(2) var IrradianceMap: texture_cube<f32>;
@group(1) @binding(3) var PrefilteredEnvMap: texture_cube<f32>;
@group(1) @binding(4) var BRDFIntMap: texture_2d<f32>;
@group(1) @binding(5) var<uniform> uShadow : ShadowUniforms;
@group(1) @binding(6) var ShadowSampler : sampler_comparison;
@group(1) @binding(7) var ShadowMap : texture_depth_2d_array;

const cubeVertices: array<vec3f, 36> = array<vec3f, 36>(
    vec3f(-1,-1,-1), vec3f( 1,-1,-1), vec3f( 1, 1,-1),
    vec3f(-1,-1,-1), vec3f( 1, 1,-1), vec3f(-1, 1,-1),

    vec3f(-1,-1, 1), vec3f( 1, 1, 1), vec3f( 1,-1, 1),
    vec3f(-1,-1, 1), vec3f(-1, 1, 1), vec3f( 1, 1, 1),

    vec3f(-1, 1,-1), vec3f( 1, 1,-1), vec3f( 1, 1, 1),
    vec3f(-1, 1,-1), vec3f( 1, 1, 1), vec3f(-1, 1, 1),

    vec3f(-1,-1,-1), vec3f(-1,-1, 1), vec3f( 1,-1, 1),
    vec3f(-1,-1,-1), vec3f( 1,-1, 1), vec3f( 1,-1,-1),

    vec3f( 1,-1,-1), vec3f( 1,-1, 1), vec3f( 1, 1, 1),
    vec3f( 1,-1,-1), vec3f( 1, 1, 1), vec3f( 1, 1,-1),

		vec3f(-1,-1,-1), vec3f(-1, 1,-1), vec3f(-1, 1, 1),
		vec3f(-1,-1,-1), vec3f(-1, 1, 1), vec3f(-1,-1, 1),
);

fn tonemapACES(color: vec3f) -> vec3f 
{
  let a = 2.51;
  let b = 0.03;
  let c = 2.43;
  let d = 0.59;
  let e = 0.14;
  return clamp((color * (a * color + b)) / (color * (c * color + d) + e), vec3f(0.0), vec3f(1.0));
}

@vertex
fn vs_main(@builtin(vertex_index) vertexIndex: u32) -> VertexOutput 
{
	var out: VertexOutput; 

	let pos = cubeVertices[vertexIndex];
  out.dir = pos;

 	let viewNoTranslation = mat4x4f(
        vec4f(uView.view[0].xyz, 0.0),
        vec4f(uView.view[1].xyz, 0.0),
        vec4f(uView.view[2].xyz, 0.0),
        vec4f(0.0, 0.0, 0.0, 1.0)
    );

  let clip = uView.projection * viewNoTranslation * vec4f(pos, 1.0);

  // push to far plane (avoid clipping at z=1.0)
  out.position = vec4f(clip.xy, clip.w, clip.w);

	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f 
{
    let dir = normalize(in.dir);
    
    var color = textureSample(EnvironmentMap, EnvironmentSampler, dir).rgb;

    // Apply ACES Tonemapping
    // This squashes the infinite HDR range into 0.0 -> 1.0 beautifully
    color = tonemapACES(color);

		// Gamma correction (sRGB)
    // color = pow(color, vec3f(1.0 / 2.2));

    return vec4f(color, 1.0);
}