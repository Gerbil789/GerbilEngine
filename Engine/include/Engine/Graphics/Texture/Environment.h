#pragma once


namespace Engine
{
  class TextureCube;
	class Texture2D;

  struct Environment
  {
		Texture2D* TextureHDR = nullptr;
    TextureCube* BaseCubemap = nullptr;
    TextureCube* IrradianceMap = nullptr;
    TextureCube* PrefilteredMap = nullptr;
  };

  class ENGINE_API EnvironmentBaker
  {
  public:
    static Environment BakeEnvironment(Texture2D* equirectangularHDR);

  private:
    static TextureCube* EquirectangularToCubemap(Texture2D* source);
    static TextureCube* CalculateIrradiance(TextureCube* sourceCubemap);
    static TextureCube* CalculatePrefiltered(TextureCube* sourceCubemap);
  };
}