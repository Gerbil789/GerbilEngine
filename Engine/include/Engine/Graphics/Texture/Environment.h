#pragma once

#include "Engine/Graphics/Texture/Texture2D.h"
#include "Engine/Graphics/Texture/TextureCube.h"

namespace Engine
{
  struct Environment
  {
    Texture2D TextureHDR;
    TextureCube BaseCubemap;
    TextureCube IrradianceMap;
    TextureCube PrefilteredMap;
  };

  class ENGINE_API EnvironmentBaker
  {
  public:
    static Environment BakeEnvironment(Texture2D& equirectangularHDR);

  private:
    static TextureCube EquirectangularToCubemap(Texture2D& source);
    static TextureCube CalculateIrradiance(TextureCube& sourceCubemap);
    static TextureCube CalculatePrefiltered(TextureCube& sourceCubemap);
  };
}