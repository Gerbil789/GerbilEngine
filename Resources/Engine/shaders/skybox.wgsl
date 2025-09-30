struct FrameUniforms {
	view: mat4x4f,
	projection: mat4x4f,
	cameraPosition: vec3f,	
	_padding: f32,
};

@group(0) @binding(0) var<uniform> uFrameUniforms: FrameUniforms;
@group(1) @binding(0) var skyboxSampler: sampler;
@group(1) @binding(1) var skyboxTexture: texture_cube<f32>;

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) dir: vec3f,
};

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
        vec4f(uFrameUniforms.view[0].xyz, 0.0),
        vec4f(uFrameUniforms.view[1].xyz, 0.0),
        vec4f(uFrameUniforms.view[2].xyz, 0.0),
        vec4f(0.0, 0.0, 0.0, 1.0)
    );

  let clip = uFrameUniforms.projection * viewNoTranslation * vec4f(pos, 1.0);

  // push to far plane (avoid clipping at z=1.0)
  out.position = vec4f(clip.xy, clip.w, clip.w);

	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
	let dir = normalize(in.dir);
  return textureSample(skyboxTexture, skyboxSampler, dir);
}