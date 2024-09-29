#include "enginepch.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Core/AssetManager.h"

namespace Engine
{
	struct Vertex
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Normal = { 0.0f, 0.0f, 0.0f };
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec2 TexCoord = { 0.0f, 0.0f };
		glm::vec2 TilingFactor = { 1.0f, 1.0f };
		int EntityID = -1;
	};


	struct RendererData
	{
		Ref<Shader> Shader;
		Ref<Texture2D> WhiteTexture;

		std::array<Ref<Texture2D>, 6> TextureSlots;

		BufferLayout Layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			//{ ShaderDataType::, "a_TexIndex" },
			{ ShaderDataType::Float2, "a_TilingFactor" },
			{ ShaderDataType::Int, "a_EntityID" }
		};

		Renderer::Statistics Stats;
	};

	static RendererData s_Data;


	void Renderer::Init()
	{
		ENGINE_PROFILE_FUNCTION();

		Texture2DFactory factory;
		s_Data.WhiteTexture = std::dynamic_pointer_cast<Texture2D>(factory.CreateTexture(1, 1, 0xffffffff));
		s_Data.TextureSlots.fill(s_Data.WhiteTexture);

		int32_t samplers[6];
		for (uint32_t i = 0; i < 6; i++)
			samplers[i] = i;

		s_Data.Shader = AssetManager::GetAsset<Shader>("resources/shaders/standard.glsl", ShaderSettings::LIGHTING);
		s_Data.Shader->Bind();
		s_Data.Shader->SetIntArray("u_Textures", samplers, 6);
	}

	void Renderer::Shutdown()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		ENGINE_PROFILE_FUNCTION();
		RenderCommand::SetViewport(0, 0, width, height);
	}


	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		ENGINE_PROFILE_FUNCTION();
		ResetStats();
		s_Data.Shader->Bind();
		s_Data.Shader->SetMat4("u_ViewProjection", camera.GetViewProjection());
		s_Data.Shader->SetFloat3("u_CameraPosition", camera.GetPosition());


		Ref<Scene> scene = SceneManager::GetCurrentScene(); //TODO: use observer pattern, don't call every frame
		std::vector<Entity> lights = scene->GetLightEntities(); //TODO: filter by enabled

		s_Data.Shader->SetInt("u_NumLights", lights.size());

		for (uint32_t i = 0; i < lights.size(); i++)
		{
			auto& lightComponent = lights[i].GetComponent<LightComponent>();
			auto& transfromComponent = lights[i].GetComponent<TransformComponent>();

			std::string lightName = "u_Lights[" + std::to_string(i) + "].";
			s_Data.Shader->SetInt(lightName + "type", (int)lightComponent.Type);
			s_Data.Shader->SetFloat3(lightName + "position", transfromComponent.Position);
			s_Data.Shader->SetFloat3(lightName + "color", lightComponent.Color);
			s_Data.Shader->SetFloat(lightName + "intensity", lightComponent.Intensity);

			switch (lightComponent.Type)
			{
			case LightType::Point:
				s_Data.Shader->SetFloat(lightName + "range", lightComponent.Range);
				s_Data.Shader->SetFloat3(lightName + "attenuation", lightComponent.Attenuation);
				break;

			case LightType::Directional:
				s_Data.Shader->SetFloat3(lightName + "direction", transfromComponent.Rotation);
				break;

			case LightType::Spot:
				s_Data.Shader->SetFloat3(lightName + "direction", transfromComponent.Rotation);
				s_Data.Shader->SetFloat(lightName + "innerAngle", lightComponent.InnerAngle);
				s_Data.Shader->SetFloat(lightName + "outerAngle", lightComponent.OuterAngle);
				break;
			}
		}
	}

	void Renderer::EndScene()
	{
		ENGINE_PROFILE_FUNCTION();

		for (uint32_t i = 0; i < 6; i++)
		{
			s_Data.TextureSlots[i]->Bind(i);
		}

	}

	void Renderer::Flush()
	{
		ENGINE_PROFILE_FUNCTION();
		//TODO: for batch rendering
	}


	void Renderer::DrawMesh(const glm::mat4& transform, Ref<Mesh> mesh, Ref<Material> material, int entityID)
	{
		ENGINE_PROFILE_FUNCTION();

		if (mesh == nullptr) { return; }

		const auto& vertices = mesh->GetVertices();
		const auto& normals = mesh->GetNormals();
		const auto& uvs = mesh->GetUVs();
		auto indices = mesh->GetIndices();

		int vertexCount = mesh->GetVertexCount();
		int indicesCount = indices.size();

		Ref<VertexArray> vertexArray = VertexArray::Create();

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertexCount * sizeof(Vertex));
		vertexBuffer->SetLayout(s_Data.Layout);
		vertexArray->AddVertexBuffer(vertexBuffer);

		Ref<IndexBuffer> IndexBuffer = IndexBuffer::Create(indices.data(), indicesCount);
		vertexArray->SetIndexBuffer(IndexBuffer);


		std::vector<Vertex> vertexBufferData(vertexCount);

		if (material != nullptr) 
		{
			s_Data.TextureSlots[0] = material->GetColorTexture() ? material->GetColorTexture() : s_Data.WhiteTexture;
			s_Data.TextureSlots[1] = material->GetMetallicTexture() ? material->GetMetallicTexture() : s_Data.WhiteTexture;
			s_Data.TextureSlots[2] = material->GetRoughnessTexture() ? material->GetRoughnessTexture() : s_Data.WhiteTexture;
			s_Data.TextureSlots[3] = material->GetNormalTexture() ? material->GetNormalTexture() : s_Data.WhiteTexture;
			s_Data.TextureSlots[4] = material->GetHeightTexture() ? material->GetHeightTexture() : s_Data.WhiteTexture;
			s_Data.TextureSlots[5] = material->GetAmbientTexture() ? material->GetAmbientTexture() : s_Data.WhiteTexture;
		}


		for (uint32_t i = 0; i < vertexCount; i++)
		{
			vertexBufferData[i].Position = transform * glm::vec4(vertices[i], 1.0f);
			vertexBufferData[i].Color = material ? material->GetColor() : glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			vertexBufferData[i].Normal = normals[i];// glm::normalize(glm::mat3(glm::transpose(glm::inverse(transform))) * normals[i]);
			vertexBufferData[i].TexCoord = uvs.size() > 0 ? uvs[i] : glm::vec2(0.0f, 0.0f);
			vertexBufferData[i].TilingFactor = material ? material->GetTiling() : glm::vec2(1.0f, 1.0f);
			vertexBufferData[i].EntityID = entityID;
		}

		vertexBuffer->SetData(vertexBufferData.data(), vertexBufferData.size() * sizeof(Vertex));
		RenderCommand::DrawIndexed(indicesCount);

		s_Data.Stats.DrawCalls++;
		s_Data.Stats.VertexCount += vertexCount;
		s_Data.Stats.IndicesCount += indicesCount;
	}


	Renderer::Statistics Renderer::GetStats()
	{
		return s_Data.Stats;
	}

	void Renderer::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}
}