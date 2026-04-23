const PI: f32 = 3.14159265359;

@group(0) @binding(0) var envSampler: sampler;
@group(0) @binding(1) var baseCubemap: texture_cube<f32>;
@group(0) @binding(2) var outputTexture: texture_storage_2d_array<rgba16float, write>;

// Maps the 2D dispatch ID and face index to a 3D direction vector
fn cubemapDirection(face: u32, uv: vec2f) -> vec3f {
    let x = uv.x;
    let y = uv.y;

    switch(face) {
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

// Maps the 2D Hammersley point to a 3D direction on a cosine-weighted hemisphere
fn sampleHemisphere(Xi: vec2f) -> vec3f {
    let phi = 2.0 * PI * Xi.x;
    let cosTheta = sqrt(1.0 - Xi.y);
    let sinTheta = sqrt(Xi.y);

    // From spherical to cartesian
    return vec3f(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}

@compute @workgroup_size(8, 8, 1)
fn computeIrradiance(@builtin(global_invocation_id) id: vec3<u32>) {
    let size = textureDimensions(outputTexture);
    
    // Bounds check to ensure we don't write outside the texture array
    if (id.x >= size.x || id.y >= size.y || id.z >= 6u) {
        return;
    }

    // Calculate the normal direction for this specific texel on the cubemap
    let uv = (vec2f(id.xy) + 0.5) / vec2f(size.xy);
    let uvRemapped = uv * 2.0 - 1.0; 
    let N = cubemapDirection(id.z, uvRemapped);

    // Create a local tangent space around the Normal (N)
    let up = select(vec3f(1.0, 0.0, 0.0), vec3f(0.0, 0.0, 1.0), abs(N.z) < 0.999);
    let tangent = normalize(cross(up, N));
    let bitangent = cross(N, tangent);

    var irradiance = vec3f(0.0);
    
    // 512 to 1024 is usually plenty for diffuse irradiance since it is very low-frequency
    let SAMPLE_COUNT = 2048u; 

    for(var i = 0u; i < SAMPLE_COUNT; i++) {
        let Xi = hammersley(i, SAMPLE_COUNT);
        
        // Get a local direction in the hemisphere
        let H = sampleHemisphere(Xi);
        
        // Transform the local direction (H) to world space (L) using the tangent basis
        let L = normalize(tangent * H.x + bitangent * H.y + N * H.z);

        // Sample the environment map. 
        // We use textureSampleLevel with level 0.0, but if your baseCubemap has mips, 
        // sampling a lower resolution mip (e.g., level 2.0 or 3.0) can help reduce noise.
  
				var sampleColor = textureSampleLevel(baseCubemap, envSampler, L, 5.0).rgb;
				sampleColor = min(sampleColor, vec3f(25.0));
				irradiance += sampleColor;
    }

    // Average out the accumulated samples
    // Note: Because we use cosine-weighted importance sampling, we don't need to multiply 
    // by cos(theta) or divide by a PDF inside the loop. The math cancels out beautifully!
    irradiance = irradiance / f32(SAMPLE_COUNT);

    // Store the calculated pixel into our single-mip texture array
    textureStore(outputTexture, id.xy, id.z, vec4f(irradiance, 1.0));
}