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
	Engine::Ref<Engine::Texture2D> m_Texture;
	Engine::OrthographicCameraController m_CameraController;

	glm::vec4 m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };

	struct ProfileResult
	{
		const char* Name;
		float Time;
	};

	std::vector<ProfileResult> m_ProfileResults;
	
};