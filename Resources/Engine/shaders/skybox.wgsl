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

struct EnvironmentUniforms
{
	lightViewProj : array<mat4x4f, NUM_SHADOW_CASCADES>,
	cascadeSplits : array<f32, NUM_SHADOW_CASCADES>,
};

//view
@group(0) @binding(0) var<uniform> uView: ViewUniforms;
//environment
@group(1) @binding(0) var<uniform> uEnvironment : EnvironmentUniforms;
@group(1) @binding(1) var EnvSampler: sampler;
@group(1) @binding(2) var BRDFIntMap: texture_2d<f32>;
@group(1) @binding(3) var IrradianceMap: texture_cube<f32>;
@group(1) @binding(4) var PrefilteredEnvMap: texture_cube<f32>;
@group(1) @binding(5) var shadowSampler : sampler_comparison;
@group(1) @binding(6) var shadowMap : texture_depth_2d_array;




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


fn tonemapACES(color: vec3f) -> vec3f {
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
    
    var color = textureSample(PrefilteredEnvMap, EnvSampler, dir).rgb;

    // 2. Apply Exposure (Adjust this value! Lower = darker, Higher = brighter)
    // You can eventually pass this in via your FrameUniforms so you can tweak it in C++
    let exposure = 1.0; 
    color = color * exposure;

    // 3. Apply ACES Tonemapping
    // This squashes the infinite HDR range into 0.0 -> 1.0 beautifully
    color = tonemapACES(color);

    // 4. Gamma Correction
    // IMPORTANT: If your WebGPU swapchain format is standard BGRA8Unorm (NOT BGRA8UnormSrgb),
    // you must manually apply gamma correction here so it doesn't look overly dark.
    // Uncomment the line below if you aren't using an -srgb swapchain format.
    // color = pow(color, vec3f(1.0 / 2.2));

    return vec4f(color, 1.0);
}