#pragma once

#include "Engine/Graphics/Texture/TextureCube.h"

namespace Engine
{
  struct Environment
  {
    TextureCube BaseCubemap;
    TextureCube IrradianceMap;
    TextureCube PrefilteredMap;
  };

  class ENGINE_API EnvironmentBaker
  {
  public:
    static Environment BakeEnvironment(Uuid equirectangularHDR);

  private:
    static TextureCube EquirectangularToCubemap(Uuid source); //TODO: move to utils
    static TextureCube CalculateIrradiance(TextureCube& sourceCubemap);
    static TextureCube CalculatePrefiltered(TextureCube& sourceCubemap);
  };
}