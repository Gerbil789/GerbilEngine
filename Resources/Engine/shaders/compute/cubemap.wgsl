@group(0) @binding(0) var inputSampler: sampler;
@group(0) @binding(1) var inputTexture: texture_2d<f32>;
@group(0) @binding(2) var outputTexture: texture_storage_2d_array<rgba32float,write>;


fn cubemapDirection(face: u32, uv: vec2f) -> vec3f 
{
  let x = uv.x;
  let y = uv.y;

  switch(face) 
  {
    case 0u: { return normalize(vec3f( 1.0, -y, -x)); } // +X
    case 1u: { return normalize(vec3f(-1.0, -y,  x)); } // -X
    case 2u: { return normalize(vec3f( x,  1.0,  y)); } // +Y
    case 3u: { return normalize(vec3f( x, -1.0, -y)); } // -Y
    case 4u: { return normalize(vec3f( x, -y,  1.0)); } // +Z
    default: { return normalize(vec3f(-x, -y, -1.0)); } // -Z
  }
}

fn dirToEquirectUV(dir: vec3f) -> vec2f 
{
    let phi = atan2(dir.z, dir.x);        // [-pi, pi]
    let theta = asin(dir.y);              // [-pi/2, pi/2]

    let u = (phi + 3.14159265359) / (2.0 * 3.14159265359);
    
    // Invert V because texture Y=0 is the Top, but 3D Y=1 is Up
    let v = 1.0 - ((theta + 1.57079632679) / 3.14159265359); 

    return vec2f(u, v);
}


@compute @workgroup_size(4, 4, 1)
fn equirectToCubemap(@builtin(global_invocation_id) id: vec3<u32>) 
{
  let size = textureDimensions(outputTexture);
  if (id.x >= size.x || id.y >= size.y || id.z >= 6u) 
	{
    return;
  }

  let uv = (vec2f(id.xy) + 0.5) / vec2f(size.xy);
  let uvRemapped = uv * 2.0 - 1.0; // [-1, 1]

  let dir = cubemapDirection(id.z, uvRemapped);
  let eqUV = dirToEquirectUV(dir);

  let color = textureSampleLevel(inputTexture, inputSampler, eqUV, 0.0);
  textureStore(outputTexture, id.xy, id.z, color);
}