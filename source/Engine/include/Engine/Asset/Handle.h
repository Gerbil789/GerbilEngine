#pragma once

#include "Engine/Core/UUID.h"

namespace engine
{
  template <typename T>
  struct Handle
  {
    Uuid id = 0;

    constexpr Handle() = default;
    constexpr explicit Handle(Uuid uuid) : id(uuid) {}

    constexpr bool operator==(const Handle& other) const { return id == other.id; }
    constexpr bool operator!=(const Handle& other) const { return id != other.id; }

  	T& Get() const;
  };

  struct AssetHasher
  {
    template <typename T>
    std::size_t operator()(const Handle<T>& handle) const
    {
      return std::hash<Uuid>()(handle.id);
    }
  };
  
  using TextureHandle = Handle<struct Texture2D>;
  using SceneHandle = Handle<struct Scene>;
  using MeshHandle = Handle<struct Mesh>;
  using ShaderHandle = Handle<struct Shader>;
  using AudioClipHandle = Handle<struct AudioClip>;
  using MaterialHandle = Handle<struct Material>;
  using MaterialInstanceHandle = Handle<struct MaterialInstance>;
  
  namespace texture
  {
    inline constexpr TextureHandle White{ 0 };
    inline constexpr TextureHandle Normal{ 1 };
    inline constexpr TextureHandle HDR{ 2 };
    inline constexpr TextureHandle EditorIcons{ 3 };
    inline constexpr TextureHandle DefaultFontAtlas{ 4 };
    inline constexpr TextureHandle EditorThumbnailAtlas{ 5 };
  }

	namespace mesh
	{
		inline constexpr MeshHandle Empty{ 0 };
		inline constexpr MeshHandle Cube{ 1 };
		inline constexpr MeshHandle Sphere{ 2 };
	}

	namespace shader
	{
		inline constexpr ShaderHandle Pink{ 0 };
		inline constexpr ShaderHandle Flat{ 1 };
		inline constexpr ShaderHandle UI{ 2 };
		inline constexpr ShaderHandle Skybox{ 3 };
		inline constexpr ShaderHandle Normal{ 4 };
	}

	namespace material
	{
		inline constexpr MaterialHandle Pink{ 0 };
		inline constexpr MaterialHandle White{ 1 };
	}

	namespace material_instance
	{
		inline constexpr MaterialInstanceHandle Pink{ 0 };
		inline constexpr MaterialInstanceHandle White{ 1 };
	}

  namespace audio
  {
    inline constexpr AudioClipHandle Empty{ 0 };
  }

  namespace scene
	{
		inline constexpr SceneHandle Empty{ 0 };
		inline constexpr SceneHandle Default{ 1 };
	}
}