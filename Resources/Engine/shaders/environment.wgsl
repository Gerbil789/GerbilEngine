struct FrameUniforms {
	view: mat4x4f,
	projection: mat4x4f,
	cameraPosition: vec3f,	
	_padding: f32,
};

@group(0) @binding(0) var<uniform> uFrame: FrameUniforms;
@group(1) @binding(0) var Sampler: sampler;
@group(1) @binding(1) var Texture: texture_2d<f32>;

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) dir: vec3f,
};

const PI: f32 = 3.14159265;

fn DirectionToEquirectUV(dir: vec3f) -> vec2f
{
    let d = normalize(dir);

    let phi = atan2(d.z, d.x);
    let theta = acos(clamp(d.y, -1.0, 1.0));

    let u = phi / (2.0 * PI) + 0.5;
    let v = theta / PI;

    return vec2f(u, v);
}


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



@vertex
fn vs_main(@builtin(vertex_index) vertexIndex: u32) -> VertexOutput {
	var out: VertexOutput; 

	let pos = cubeVertices[vertexIndex];
  out.dir = pos;

 	let viewNoTranslation = mat4x4f(
        vec4f(uFrame.view[0].xyz, 0.0),
        vec4f(uFrame.view[1].xyz, 0.0),
        vec4f(uFrame.view[2].xyz, 0.0),
        vec4f(0.0, 0.0, 0.0, 1.0)
    );

  let clip = uFrame.projection * viewNoTranslation * vec4f(pos, 1.0);

  // push to far plane (avoid clipping at z=1.0)
  out.position = vec4f(clip.xy, clip.w, clip.w);

	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f 
{
	let dir = normalize(in.dir);
	let uv = DirectionToEquirectUV(dir);
  let color = textureSample(Texture, Sampler, uv);
  return vec4f(color.rgb, 1.0);
  //return textureSample(skyboxTexture, skyboxSampler, dir);
}