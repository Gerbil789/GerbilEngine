#include "enginepch.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Renderer/UniformBuffer.h"

namespace Engine
{
	struct Vertex
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Normal = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		glm::vec2 TexCoord = { 0.0f, 0.0f };
		glm::vec3 Tangent = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Bitangent = { 0.0f, 0.0f, 0.0f };
		glm::vec2 Tiling = { 1.0f, 1.0f };
		glm::vec2 Offset = { 0.0f, 0.0f };
	};


	struct RendererData
	{
		//Ref<Shader> Shader;
		//std::array<Ref<Texture2D>, 6> TextureSlots;
		Ref<Material> defaultMaterial; 

		//Ref<Texture2D> SolidColorTexture;
		//Ref<Texture2D> MetallicValueTexture;
		//Ref<Texture2D> RoughnessValueTexture;
		//Ref<Texture2D> NormalValueTexture;

		BufferLayout Layout{
			{ ShaderDataType::Float3, "Position" },
			{ ShaderDataType::Float3, "Normal" },
			{ ShaderDataType::Float3, "Color" },
			{ ShaderDataType::Float2, "TexCoord" },
			{ ShaderDataType::Float3, "Tangent" },
			{ ShaderDataType::Float3, "Bitangent" },
			{ ShaderDataType::Float2, "Tiling" },
			{ ShaderDataType::Float2, "Offset" }
			//{ ShaderDataType::Int, "a_EntityID" }
		};

		Renderer::Statistics Stats;

		struct ObjectUniforms
		{
			int EntityID = -1;
		};

		struct GlobalUniforms
		{
			glm::mat4 ViewProjection = glm::mat4(1.0f);
		};

		ObjectUniforms ObjectUniforms;
		GlobalUniforms GlobalUniforms;

		Ref<UniformBuffer> ObjectUniformBuffer;
		Ref<UniformBuffer> GlobalUniformBuffer;
		Ref<UniformBuffer> MaterialUniformBuffer;
	};

	static RendererData s_Data;


	void Renderer::AlignOffset(size_t& currentOffset, size_t alignment)
	{
		size_t padding = currentOffset % alignment;
		if (padding != 0)
		{
			currentOffset += (alignment - padding);
		}
	}


	void Renderer::Init()
	{
		ENGINE_PROFILE_FUNCTION();

		s_Data.defaultMaterial = AssetManager::CreateAsset<Material>("resources/materials/default.mat");
		s_Data.defaultMaterial->SetShader(AssetManager::GetAsset<Shader>("resources/shaders/flatColor.shader"));
		s_Data.defaultMaterial->SetProperty("Color", glm::vec3(0.8f, 0.08f, 0.91f)); // pink

		s_Data.GlobalUniformBuffer = UniformBuffer::Create(sizeof(RendererData::GlobalUniforms), 0);
		//s_Data.MaterialUniformBuffer = UniformBuffer::Create(64, 1);
		s_Data.ObjectUniformBuffer = UniformBuffer::Create(sizeof(RendererData::ObjectUniforms), 2);


		/*Texture2DFactory factory;
		s_Data.SolidColorTexture = std::dynamic_pointer_cast<Texture2D>(factory.CreateSolidColorTexture(0xffffffff));
		s_Data.MetallicValueTexture = std::dynamic_pointer_cast<Texture2D>(factory.CreateSolidColorTexture(0xff000000));
		s_Data.RoughnessValueTexture = std::dynamic_pointer_cast<Texture2D>(factory.CreateSolidColorTexture(0xff000000));
		s_Data.NormalValueTexture = std::dynamic_pointer_cast<Texture2D>(factory.CreateSolidColorTexture(0xffff8080));*/
		//s_Data.TextureSlots.fill(s_Data.SolidColorTexture);

		//int32_t samplers[6];
		//for (uint32_t i = 0; i < 6; i++) { samplers[i] = i; }

		//s_Data.Shader = AssetManager::GetAsset<Shader>("resources/shaders/flatColor.shader");
		//s_Data.Shader->Bind();
		//s_Data.Shader->SetIntArray("u_Textures", samplers, 6);
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
		//TODO: implement
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		ENGINE_PROFILE_FUNCTION();
		ResetStats();

		s_Data.GlobalUniforms.ViewProjection = camera.GetViewProjectionMatrix();
		s_Data.GlobalUniformBuffer->SetData(&s_Data.GlobalUniforms, sizeof(RendererData::GlobalUniforms), 0);


		//s_Data.Shader->Bind();
		//s_Data.Shader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		//s_Data.Shader->SetFloat3("u_CameraPosition", camera.GetPosition());


		//Ref<Scene> scene = SceneManager::GetCurrentScene(); //TODO: use observer pattern, don't call every frame
		//std::vector<Entity> lights = scene->GetLightEntities(); //TODO: filter by enabled

		//s_Data.Shader->SetInt("u_NumLights", lights.size());
		/*for (uint32_t i = 0; i < lights.size(); i++)
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
		}*/
	}

	void Renderer::EndScene()
	{
		ENGINE_PROFILE_FUNCTION();

		/*for (uint32_t i = 0; i < 6; i++)
		{
			s_Data.TextureSlots[i]->Bind(i);
		}*/
	}

	void Renderer::DrawMesh(const glm::mat4& transform, Ref<Mesh> mesh, Ref<Material> material, int entityID)
	{
		ENGINE_PROFILE_FUNCTION();

		if (mesh == nullptr) { return; }
		if (material == nullptr) { material = s_Data.defaultMaterial; }


		auto& shader = material->GetShader();

		const auto& vertices = mesh->GetVertices();
		const auto& normals = mesh->GetNormals();
		const auto& uvs = mesh->GetUVs();
		auto indices = mesh->GetIndices();

		int vertexCount = mesh->GetVertexCount();
		int indicesCount = (int)indices.size();

		Ref<VertexArray> vertexArray = VertexArray::Create();
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertexCount * sizeof(Vertex));

		s_Data.ObjectUniforms.EntityID = entityID;
		s_Data.ObjectUniformBuffer->SetData(&s_Data.ObjectUniforms, sizeof(RendererData::ObjectUniforms), 0);


		std::vector<char> bufferData;
		size_t currentOffset = 0;

		auto properties = material->GetProperties();
		for (const auto& [name, property] : properties)
		{
			//Check the type of the property and handle each case
			if (std::holds_alternative<float>(property))
			{
				AlignOffset(currentOffset, 4);  // Align to 4 bytes for a float
				float value = std::get<float>(property);
				bufferData.resize(currentOffset + sizeof(float));
				memcpy(bufferData.data() + currentOffset, &value, sizeof(float));
				currentOffset += sizeof(float);
			}
			else if (std::holds_alternative<int>(property))
			{
				AlignOffset(currentOffset, 4);  // Align to 4 bytes for an int
				int value = std::get<int>(property);
				bufferData.resize(currentOffset + sizeof(int));
				memcpy(bufferData.data() + currentOffset, &value, sizeof(int));
				currentOffset += sizeof(int);
			}
			else if (std::holds_alternative<glm::vec2>(property))
			{
				AlignOffset(currentOffset, 8);  // Align to 8 bytes for a vec2
				glm::vec2 value = std::get<glm::vec2>(property);
				bufferData.resize(currentOffset + sizeof(glm::vec2));
				memcpy(bufferData.data() + currentOffset, &value, sizeof(glm::vec2));
				currentOffset += sizeof(glm::vec2);
			}
			else if (std::holds_alternative<glm::vec3>(property))
			{
				AlignOffset(currentOffset, 16);  // Align to 16 bytes for a vec3 (std140)
				glm::vec3 value = std::get<glm::vec3>(property);
				bufferData.resize(currentOffset + sizeof(glm::vec3));
				memcpy(bufferData.data() + currentOffset, &value, sizeof(glm::vec3));
				currentOffset += sizeof(glm::vec3);
				currentOffset += 4;  // Extra padding to align to 16 bytes (vec3 takes 12 bytes, but is padded to 16)
			}
			else if (std::holds_alternative<glm::vec4>(property))
			{
				AlignOffset(currentOffset, 16);  // Align to 16 bytes for a vec4
				glm::vec4 value = std::get<glm::vec4>(property);
				bufferData.resize(currentOffset + sizeof(glm::vec4));
				memcpy(bufferData.data() + currentOffset, &value, sizeof(glm::vec4));
				currentOffset += sizeof(glm::vec4);
			}
			// Note: Texture handling is separate, you wouldn't pack textures into the buffer but bind them separately
		}
		uint32_t size = bufferData.size();
		s_Data.MaterialUniformBuffer = UniformBuffer::Create(size, 1);
		s_Data.MaterialUniformBuffer->SetData(bufferData.data(), size, 0);

		//auto uniformBuffer = shader->GetUniformBuffer();
		//for (auto& element : uniformBuffer)
		//{
		//	auto name = element.Name;
		//	auto type = element.Type;
		//	
		//}

		vertexBuffer->SetLayout(s_Data.Layout); 
		vertexArray->AddVertexBuffer(vertexBuffer);

		Ref<IndexBuffer> IndexBuffer = IndexBuffer::Create(indices.data(), indicesCount);
		vertexArray->SetIndexBuffer(IndexBuffer);

		std::vector<Vertex> vertexBufferData(vertexCount);

		if (material != nullptr)
		{
			/*s_Data.TextureSlots[0] = material->GetColorTexture() ? material->GetColorTexture() : s_Data.SolidColorTexture;

			if (material->GetMetallicTexture() == nullptr) 
			{
				uint32_t val = (uint32_t)(material->GetMetallic() * 255.0f);
				uint32_t metallic = (0xff << 24) | (val << 16) | (val << 8) | val;
				s_Data.MetallicValueTexture->SetData(&metallic, sizeof(uint32_t));
				s_Data.TextureSlots[1] =  s_Data.MetallicValueTexture;
			}
			else 
			{
				s_Data.TextureSlots[1] = material->GetMetallicTexture();
			}


			if (material->GetRoughnessTexture() == nullptr)
			{
				uint32_t val = (uint32_t)(material->GetRoughness() * 255.0f);
				uint32_t roughness = (0xff << 24) | (val << 16) | (val << 8) | val;
				s_Data.RoughnessValueTexture->SetData(&roughness, sizeof(uint32_t));
				s_Data.TextureSlots[2] = s_Data.RoughnessValueTexture;
			}
			else
			{
				s_Data.TextureSlots[2] = material->GetRoughnessTexture();
			}


			if (material->GetNormalTexture() == nullptr) { s_Data.TextureSlots[3] = s_Data.NormalValueTexture; }
			else { s_Data.TextureSlots[3] = material->GetNormalTexture(); }

			s_Data.TextureSlots[4] = material->GetHeightTexture() ? material->GetHeightTexture() : s_Data.SolidColorTexture;
			s_Data.TextureSlots[5] = material->GetAmbientTexture() ? material->GetAmbientTexture() : s_Data.SolidColorTexture;*/
		}

		// Calculate the tangent and bitangent for each triangle
		//for (size_t i = 0; i < indices.size(); i += 3)
		//{
		//	// Get the indices of the triangle's vertices
		//	uint32_t i0 = indices[i];
		//	uint32_t i1 = indices[i + 1];
		//	uint32_t i2 = indices[i + 2];

		//	// Get the positions and texture coordinates of the vertices
		//	const glm::vec3& v0 = vertices[i0];
		//	const glm::vec3& v1 = vertices[i1];
		//	const glm::vec3& v2 = vertices[i2];

		//	const glm::vec2& uv0 = uvs[i0];
		//	const glm::vec2& uv1 = uvs[i1];
		//	const glm::vec2& uv2 = uvs[i2];

		//	// Calculate the edges of the triangle
		//	glm::vec3 deltaPos1 = v1 - v0;
		//	glm::vec3 deltaPos2 = v2 - v0;

		//	glm::vec2 deltaUV1 = uv1 - uv0;
		//	glm::vec2 deltaUV2 = uv2 - uv0;

		//	// Calculate the tangent and bitangent
		//	float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		//	glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		//	glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		//	// Accumulate the tangent and bitangent for each vertex of the triangle
		//	vertexBufferData[i0].Tangent += tangent;
		//	vertexBufferData[i1].Tangent += tangent;
		//	vertexBufferData[i2].Tangent += tangent;

		//	vertexBufferData[i0].Bitangent += bitangent;
		//	vertexBufferData[i1].Bitangent += bitangent;
		//	vertexBufferData[i2].Bitangent += bitangent;
		//}


		for (uint32_t i = 0; i < vertexCount; i++)
		{
			vertexBufferData[i].Position = transform * glm::vec4(vertices[i], 1.0f);
		}

		vertexBuffer->SetData(vertexBufferData.data(),(uint32_t)(vertexBufferData.size() * sizeof(Vertex)));
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