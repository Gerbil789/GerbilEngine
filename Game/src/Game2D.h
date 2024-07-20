#include <Engine.h>


class Game2D : public Engine::Layer
{
public:
	Game2D();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(Engine::Timestep ts) override;
	void OnEvent(Engine::Event& e) override;
	void OnImGuiRender() override;
private:
	Engine::ShaderLibrary m_ShaderLibrary;
	Engine::Ref<Engine::VertexArray> m_VertexArrayObject;
	Engine::OrthographicCameraController m_CameraController;

	Engine::Ref<Engine::Texture2D> m_BackroundTexture;
	Engine::Ref<Engine::Texture2D> m_GerbilTexture;
	Engine::Ref<Engine::Texture2D> m_TileTexture;
	Engine::Ref<Engine::Texture2D> m_Spritesheet;


	std::vector<Engine::Ref<Engine::SubTexture2D>> m_TileTextures;



	glm::vec4 m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	
};