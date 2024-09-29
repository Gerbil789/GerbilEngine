#include "enginepch.h"
#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Core/AssetManager.h"

namespace Engine 
{
	struct QuadVertex
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Normal = { 0.0f, 0.0f, 0.0f };
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec2 TexCoord = { 0.0f, 0.0f };
		float TexIndex = 0.0f;
		glm::vec2 TilingFactor = { 1.0f, 1.0f };
		int EntityID = -1;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 1000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		glm::vec4 QuadVertexPositions[4] = {
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f }
		};

		Renderer2D::Statistics Stats;
	};

	static Renderer2DData s_Data;




	void Renderer2D::Init()
	{
		ENGINE_PROFILE_FUNCTION();

		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float2, "a_TilingFactor" },
			{ ShaderDataType::Int, "a_EntityID" }
		};

		s_Data.QuadVertexBuffer->SetLayout(layout);

		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6) 
		{
			quadIndices[i] = offset;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;
				
			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> QuadIndexBuffer = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(QuadIndexBuffer);
		delete[] quadIndices;

		Texture2DFactory factory;
		s_Data.WhiteTexture = std::dynamic_pointer_cast<Texture2D>(factory.CreateTexture(1, 1, 0xffffffff));

		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data.TextureShader = AssetManager::GetAsset<Shader>("resources/shaders/standard2D.glsl", ShaderSettings::LIGHTING);
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;
			
		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::Shutdown()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		ENGINE_PROFILE_FUNCTION();
		glm::mat4 viewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", viewProjection);

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		ENGINE_PROFILE_FUNCTION();
		Renderer2D::ResetStats();
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjection());
		s_Data.TextureShader->SetFloat3("u_CameraPosition", camera.GetPosition());


		Ref<Scene> scene = SceneManager::GetCurrentScene(); //TODO: use observer pattern
		std::vector<Entity> lights = scene->GetLightEntities(); //TODO: filter by enabled

		s_Data.TextureShader->SetInt("u_NumLights", lights.size());

		for(uint32_t i = 0; i < lights.size(); i++)
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

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		ENGINE_PROFILE_FUNCTION();

		uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		Flush();
	}

	void Renderer2D::Flush()
	{
		ENGINE_PROFILE_FUNCTION();

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++) 
		{
			s_Data.TextureSlots[i]->Bind(i);
		}

		if (s_Data.QuadIndexCount != 0) 
		{
			RenderCommand::DrawIndexed(s_Data.QuadIndexCount);
			s_Data.Stats.DrawCalls++;
		}

	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	{
		ENGINE_PROFILE_FUNCTION();
		//tmp
		if (src.Material != nullptr) 
		{
			if(src.Material->GetColorTexture() == nullptr)
			{
				DrawQuad(transform, s_Data.WhiteTexture, src.Material->GetTiling(), src.Material->GetColor(), entityID);
				return;
			}


			DrawQuad(transform, src.Material->GetColorTexture(), src.Material->GetTiling(), src.Material->GetColor(), entityID);
			return;
		}


		if (src.Texture) 
		{
			DrawQuad(transform, src.Texture, src.TilingFactor, src.Color, entityID);
		}
		else 
		{
			DrawQuad(transform, s_Data.WhiteTexture, src.TilingFactor, src.Color, entityID);
		}
	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;
	}


	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		ENGINE_PROFILE_FUNCTION();
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		ENGINE_PROFILE_FUNCTION();
		constexpr glm::vec2 textureCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		if(s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for(uint32_t i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, glm::vec2 tilingFactor, const glm::vec4& color)
	{
		ENGINE_PROFILE_FUNCTION();
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, glm::vec2 tilingFactor, const glm::vec4& color)
	{
		ENGINE_PROFILE_FUNCTION();

		constexpr glm::vec2 textureCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		float textureIndex = 0.0f;

		for(uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if(*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}


		if(textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });


		for(size_t i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}



	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, glm::vec2 tilingFactor, const glm::vec4& color)
	{
		ENGINE_PROFILE_FUNCTION();
		DrawQuad({ position.x, position.y, 0.0f }, size, subtexture, tilingFactor, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, glm::vec2 tilingFactor, const glm::vec4& color)
	{
		ENGINE_PROFILE_FUNCTION();

		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture();

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}


		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });


		for (size_t i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityId)
	{
		ENGINE_PROFILE_FUNCTION();
		constexpr glm::vec2 textureCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		for (uint32_t i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->EntityID = entityId;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, glm::vec2 tilingFactor, const glm::vec4& color, int entityId)
	{
		ENGINE_PROFILE_FUNCTION();
		constexpr glm::vec2 textureCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		float textureIndex = 0.0f;


		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if(textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		//calculate normal from transform
		glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(transform[1]), glm::vec3(transform[0])));

		for (uint32_t i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->Normal = normal;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityId;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}





	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		ENGINE_PROFILE_FUNCTION();
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		ENGINE_PROFILE_FUNCTION();
		constexpr glm::vec2 textureCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for(uint32_t i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];

			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}





	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, glm::vec2 tilingFactor, const glm::vec4& color)
	{
		ENGINE_PROFILE_FUNCTION();
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, glm::vec2 tilingFactor, const glm::vec4& color)
	{
		ENGINE_PROFILE_FUNCTION();

		constexpr glm::vec2 textureCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}


		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for(size_t i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}







	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, glm::vec2 tilingFactor, const glm::vec4& color)
	{
		ENGINE_PROFILE_FUNCTION();
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, subtexture, tilingFactor, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, glm::vec2 tilingFactor, const glm::vec4& color)
	{
		ENGINE_PROFILE_FUNCTION();

		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture();

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}


		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}






	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}


	Renderer2D::Statistics Renderer2D::GetStats()
	{
		s_Data.Stats.TriangleCount = s_Data.Stats.QuadCount * 2;
		return s_Data.Stats;
	}
}