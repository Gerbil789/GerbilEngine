const PI: f32 = 3.14159265359;

@group(0) @binding(0) var envSampler: sampler;
@group(0) @binding(1) var baseCubemap: texture_cube<f32>;
@group(0) @binding(2) var outputMipLevel: texture_storage_2d_array<rgba16float,write>;

// (Keep your cubemapDirection function here)
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

// Generates a random sequence based on the Van der Corput sequence
fn radicalInverse_VdC(bits: u32) -> f32 {
    var b = (bits << 16u) | (bits >> 16u);
    b = ((b & 0x55555555u) << 1u) | ((b & 0xAAAAAAAAu) >> 1u);
    b = ((b & 0x33333333u) << 2u) | ((b & 0xCCCCCCCCu) >> 2u);
    b = ((b & 0x0F0F0F0Fu) << 4u) | ((b & 0xF0F0F0F0u) >> 4u);
    b = ((b & 0x00FF00FFu) << 8u) | ((b & 0xFF00FF00u) >> 8u);
    return f32(b) * 2.3283064365386963e-10; 
}

// Generates a 2D point uniformly distributed on a plane
fn hammersley(i: u32, N: u32) -> vec2f {
    return vec2f(f32(i) / f32(N), radicalInverse_VdC(i));
}

// Transforms the 2D point into a 3D direction biased by roughness (GGX)
fn importanceSampleGGX(Xi: vec2f, N_dir: vec3f, roughness: f32) -> vec3f {
    let a = roughness * roughness;
    let phi = 2.0 * PI * Xi.x;
    let cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    let sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    // from spherical to cartesian
    let H = vec3f(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    // tangent space to world space
    let up = select(vec3f(1.0, 0.0, 0.0), vec3f(0.0, 0.0, 1.0), abs(N_dir.z) < 0.999);
    let tangent = normalize(cross(up, N_dir));
    let bitangent = cross(N_dir, tangent);

    return normalize(tangent * H.x + bitangent * H.y + N_dir * H.z);
}

@compute @workgroup_size(8, 8, 1)
fn computeMipMap(@builtin(global_invocation_id) id: vec3<u32>) 
{
    let size = textureDimensions(outputMipLevel);
    if (id.x >= size.x || id.y >= size.y || id.z >= 6u) {
        return;
    }

    // Clever trick to calculate current roughness without needing a uniform buffer!
    // We figure out which mip we are on by comparing the output size to the base size.
    let baseSize = textureDimensions(baseCubemap); 

    // // Map mip level to a roughness value [0.0, 1.0]
    // let roughness = currentMip / max(numMips - 1.0, 1.0);

// Calculate total mips mathematically (equivalent to C++ std::bit_width)
    let maxDim = max(f32(baseSize.x), f32(baseSize.y));
    let numMips = floor(log2(maxDim)) + 1.0;
    
    // Calculate which mip we are currently writing to
    let currentMip = log2(f32(baseSize.x) / f32(size.x));
    
    // Map mip level to a roughness value [0.0, 1.0]
    let roughness = currentMip / max(numMips - 1.0, 1.0);




    let uv = (vec2f(id.xy) + 0.5) / vec2f(size.xy);
    let uvRemapped = uv * 2.0 - 1.0; 
    let N_dir = cubemapDirection(id.z, uvRemapped);

    // For the prefilter approximation, we assume view direction (V) == reflection (R) == normal (N)
    let R = N_dir;
    let V = R;

    let SAMPLE_COUNT = 256u; // Adjust between 512-4096 depending on how much time you have during generation
    var totalWeight = 0.0;
    var prefilteredColor = vec3f(0.0);

    for(var i = 0u; i < SAMPLE_COUNT; i++) {
        let Xi = hammersley(i, SAMPLE_COUNT);
        let H = importanceSampleGGX(Xi, N_dir, roughness);
        let L = normalize(2.0 * dot(V, H) * H - V);

        let NdotL = max(dot(N_dir, L), 0.0);
        if(NdotL > 0.0) {
            // Sample the environment map using the 3D direction vector
            prefilteredColor += textureSampleLevel(baseCubemap, envSampler, L, 0.0).rgb * NdotL;
            totalWeight += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;

    textureStore(outputMipLevel, id.xy, id.z, vec4f(prefilteredColor, 1.0));
}