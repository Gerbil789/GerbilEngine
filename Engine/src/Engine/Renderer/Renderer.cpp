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
	struct RendererData
	{
		Ref<Material> defaultMaterial;
		Renderer::Statistics Stats;

		struct GlobalUniforms
		{
			glm::mat4 ViewProjection = glm::mat4(1.0f);
		};

		GlobalUniforms GlobalUniforms;
		Ref<UniformBuffer> GlobalUniformBuffer;
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		ENGINE_PROFILE_FUNCTION();

		//s_Data.defaultMaterial = AssetManager::GetAsset<Material>("resources/materials/default.material");
		s_Data.defaultMaterial = AssetManager::GetAsset<Material>("assets/materials/test.material");
		s_Data.GlobalUniformBuffer = UniformBuffer::Create(sizeof(RendererData::GlobalUniforms), 0);
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

		//GLOBAL DATA
		s_Data.GlobalUniforms.ViewProjection = camera.GetViewProjectionMatrix();
		s_Data.GlobalUniformBuffer->SetData(&s_Data.GlobalUniforms, sizeof(RendererData::GlobalUniforms), 0);
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

		const Ref<Shader> shader = material->GetShader();
		DynamicVertex vertex = shader->GetVertex();

		const std::vector<glm::vec3>& vertices = mesh->GetVertices();
		const auto& normals = mesh->GetNormals();
		const auto& uvs = mesh->GetUVs();
		auto indices = mesh->GetIndices();

		int vertexCount = mesh->GetVertexCount();
		uint32_t indicesCount = indices.size();

		BufferLayout shaderInputLayout = shader->GetInputBuffer(); //per vertex data

		//PER VERTEX DATA
		std::vector<uint8_t> vertexBufferData = std::vector<uint8_t>(vertexCount * shaderInputLayout.size());
		size_t currentOffset = 0;
		for (uint32_t i = 0; i < vertexCount; i++) 
		{
			for (auto& element : shaderInputLayout) 
			{
				auto name = element.Name;
				auto size = element.Size;

				if (name == "EntityID") 
				{
					memcpy(vertexBufferData.data() + currentOffset, &entityID, size);
				}
				else if (name == "Position") 
				{
					glm::vec3 position = transform * glm::vec4(vertices[i], 1.0f);
					memcpy(vertexBufferData.data() + currentOffset, &position, size);
				}
				currentOffset += size; 
			}
		}

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertexCount * shaderInputLayout.size());
		vertexBuffer->SetLayout(shaderInputLayout);		
		vertexBuffer->SetData(vertexBufferData.data(), (uint32_t)vertexBufferData.size());

		Ref<IndexBuffer> IndexBuffer = IndexBuffer::Create(indices.data(), indicesCount);

		Ref<VertexArray> vertexArray = VertexArray::Create();
		vertexArray->AddVertexBuffer(vertexBuffer);
		vertexArray->SetIndexBuffer(IndexBuffer);



		//MATERIAL DATA
		BufferLayout shaderMaterialBuffer = shader->GetMaterialBuffer();
		if (!shaderMaterialBuffer.empty()) 
		{
			std::vector<uint8_t> bufferData(shaderMaterialBuffer.size());
			for (const auto& var : shaderMaterialBuffer)
			{
				std::string name = var.Name;
				ShaderDataType type = var.Type;
				uint32_t size = var.Size;
				uint32_t offset = var.Offset;

				switch (type)
				{
				case ShaderDataType::Float:
				{
					float value = material->GetProperty<float>(name);
					memcpy(bufferData.data() + offset, &value, size);
					break;
				}
				case ShaderDataType::Float2:
				{
					glm::vec2 value = material->GetProperty<glm::vec2>(name);
					memcpy(bufferData.data() + offset, &value, size);
					break;
				}
				case ShaderDataType::Float3:
				{
					glm::vec3 value = material->GetProperty<glm::vec3>(name);
					memcpy(bufferData.data() + offset, &value, size);
					break;
				}

				default:
					break;
				}
			}

			Ref<UniformBuffer> MaterialUniformBuffer = UniformBuffer::Create(bufferData.size(), 1);
			MaterialUniformBuffer->SetData(bufferData.data(), bufferData.size(), 0);
		}

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

	void Renderer::AlignOffset(size_t& currentOffset, size_t alignment)
	{
		size_t padding = currentOffset % alignment;
		if (padding != 0)
		{
			currentOffset += (alignment - padding);
		}
	}
}



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


//if (material != nullptr)
//{
//	/*s_Data.TextureSlots[0] = material->GetColorTexture() ? material->GetColorTexture() : s_Data.SolidColorTexture;
//
//	if (material->GetMetallicTexture() == nullptr)
//	{
//		uint32_t val = (uint32_t)(material->GetMetallic() * 255.0f);
//		uint32_t metallic = (0xff << 24) | (val << 16) | (val << 8) | val;
//		s_Data.MetallicValueTexture->SetData(&metallic, sizeof(uint32_t));
//		s_Data.TextureSlots[1] =  s_Data.MetallicValueTexture;
//	}
//	else
//	{
//		s_Data.TextureSlots[1] = material->GetMetallicTexture();
//	}
//
//
//	if (material->GetRoughnessTexture() == nullptr)
//	{
//		uint32_t val = (uint32_t)(material->GetRoughness() * 255.0f);
//		uint32_t roughness = (0xff << 24) | (val << 16) | (val << 8) | val;
//		s_Data.RoughnessValueTexture->SetData(&roughness, sizeof(uint32_t));
//		s_Data.TextureSlots[2] = s_Data.RoughnessValueTexture;
//	}
//	else
//	{
//		s_Data.TextureSlots[2] = material->GetRoughnessTexture();
//	}
//
//
//	if (material->GetNormalTexture() == nullptr) { s_Data.TextureSlots[3] = s_Data.NormalValueTexture; }
//	else { s_Data.TextureSlots[3] = material->GetNormalTexture(); }
//
//	s_Data.TextureSlots[4] = material->GetHeightTexture() ? material->GetHeightTexture() : s_Data.SolidColorTexture;
//	s_Data.TextureSlots[5] = material->GetAmbientTexture() ? material->GetAmbientTexture() : s_Data.SolidColorTexture;*/
//}

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