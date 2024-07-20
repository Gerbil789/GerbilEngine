#include <Engine.h>


namespace Engine 
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(Timestep ts) override;
		void OnEvent(Event& e) override;
		void OnImGuiRender() override;
	private:
		ShaderLibrary m_ShaderLibrary;
		Ref<VertexArray> m_VertexArrayObject;
		OrthographicCameraController m_CameraController;

		Ref<Texture2D> m_BackroundTexture;
		Ref<Texture2D> m_GerbilTexture;
		Ref<Texture2D> m_TileTexture;
		Ref<Texture2D> m_Spritesheet;

		std::vector<Ref<SubTexture2D>> m_TileTextures;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		Ref<FrameBuffer> m_FrameBuffer;
	};
}