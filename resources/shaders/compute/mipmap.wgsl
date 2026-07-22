@group(0) @binding(0) var previousMipLevel: texture_2d_array<f32>;
@group(0) @binding(1) var nextMipLevel: texture_storage_2d_array<rgba16float,write>;


@compute @workgroup_size(8, 8, 1)
fn computeMipMap(@builtin(global_invocation_id) id: vec3<u32>) 
{
    let size = textureDimensions(nextMipLevel);

    if (id.x >= size.x || id.y >= size.y || id.z >= 6u) 
		{
        return;
    }

    let layer = id.z;
    let base = 2u * id.xy;

	let c0 = textureLoad(previousMipLevel, base + vec2<u32>(0,0), layer, 0);
  let c1 = textureLoad(previousMipLevel, base + vec2<u32>(1,0), layer, 0);
  let c2 = textureLoad(previousMipLevel, base + vec2<u32>(0,1), layer, 0);
  let c3 = textureLoad(previousMipLevel, base + vec2<u32>(1,1), layer, 0);

    let color = (c0 + c1 + c2 + c3) * 0.25;

    textureStore(nextMipLevel, id.xy, layer, color);
}
