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
		glm::vec3 TexIndex = { 0.0f, 0.0f, 0.0f };
		glm::vec2 TilingFactor = { 1.0f, 1.0f };
		int EntityID = -1;
	};


	struct RendererData
	{
		static const uint32_t MaxTextureSlots = 32;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		BufferLayout Layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float3, "a_TexIndex" },
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

		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data.TextureShader = AssetManager::GetAsset<Shader>("resources/shaders/texture.glsl", ShaderSettings::LIGHTING);
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;
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
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjection());
		s_Data.TextureShader->SetFloat3("u_CameraPosition", camera.GetPosition());


		Ref<Scene> scene = SceneManager::GetCurrentScene(); //TODO: use observer pattern, don't call every frame
		std::vector<Entity> lights = scene->GetLightEntities(); //TODO: filter by enabled

		s_Data.TextureShader->SetInt("u_NumLights", lights.size());

		for (uint32_t i = 0; i < lights.size(); i++)
		{
			auto& lightComponent = lights[i].GetComponent<LightComponent>();
			auto& transfromComponent = lights[i].GetComponent<TransformComponent>();

			std::string lightName = "u_Lights[" + std::to_string(i) + "].";
			s_Data.TextureShader->SetInt(lightName + "type", (int)lightComponent.Type);
			s_Data.TextureShader->SetFloat3(lightName + "position", transfromComponent.Position);
			s_Data.TextureShader->SetFloat3(lightName + "color", lightComponent.Color);
			s_Data.TextureShader->SetFloat(lightName + "intensity", lightComponent.Intensity);

			switch (lightComponent.Type)
			{
			case LightType::Point:
				s_Data.TextureShader->SetFloat(lightName + "range", lightComponent.Range);
				s_Data.TextureShader->SetFloat3(lightName + "attenuation", lightComponent.Attenuation);
				break;

			case LightType::Directional:
				s_Data.TextureShader->SetFloat3(lightName + "direction", transfromComponent.Rotation);
				break;

			case LightType::Spot:
				s_Data.TextureShader->SetFloat3(lightName + "direction", transfromComponent.Rotation);
				s_Data.TextureShader->SetFloat(lightName + "innerAngle", lightComponent.InnerAngle);
				s_Data.TextureShader->SetFloat(lightName + "outerAngle", lightComponent.OuterAngle);
				break;
			}
		}
		s_Data.TextureSlotIndex = 1;
	}

	void Renderer::EndScene()
	{
		ENGINE_PROFILE_FUNCTION();

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
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


		glm::vec3 textureIndex = { 0.0f, 0.0f, 0.0f };


		if (material != nullptr) 
		{
			auto colorTexture = material->colorTexture ? material->colorTexture : s_Data.WhiteTexture;

			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				if (*s_Data.TextureSlots[i].get() == *colorTexture.get())
				{
					textureIndex[0] = (float)i;
					break;
				}
			}

			if (textureIndex[0] == 0.0f)
			{
				textureIndex[0] = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = colorTexture;
				s_Data.TextureSlotIndex++;
			}

			auto normalTexture = material->normalTexture ? material->normalTexture : s_Data.WhiteTexture;

			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				if (*s_Data.TextureSlots[i].get() == *normalTexture.get())
				{
					textureIndex[1] = (float)i;
					break;
				}
			}

			if (textureIndex[1] == 0.0f)
			{
				textureIndex[1] = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = normalTexture;
				s_Data.TextureSlotIndex++;
			}


			auto roughnessTexture = material->roughnessTexture ? material->roughnessTexture : s_Data.WhiteTexture;

			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				if (*s_Data.TextureSlots[i].get() == *roughnessTexture.get())
				{
					textureIndex[2] = (float)i;
					break;
				}
			}

			if (textureIndex[2] == 0.0f)
			{
				textureIndex[2] = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = roughnessTexture;
				s_Data.TextureSlotIndex++;
			}
		}


		for (uint32_t i = 0; i < vertexCount; i++)
		{
			vertexBufferData[i].Position = transform * glm::vec4(vertices[i], 1.0f);
			vertexBufferData[i].Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			vertexBufferData[i].Normal = glm::normalize(glm::mat3(glm::transpose(glm::inverse(transform))) * normals[i]);
			vertexBufferData[i].TexCoord = uvs.size() > 0 ? uvs[i] : glm::vec2(0.0f, 0.0f);
			vertexBufferData[i].TexIndex = textureIndex;
			vertexBufferData[i].TilingFactor = material ? material->tiling : glm::vec2(1.0f, 1.0f);
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