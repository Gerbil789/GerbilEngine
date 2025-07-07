#include "enginepch.h"
#include "Renderer.h"
//#include "Engine/Renderer/RenderCommand.h"
//#include "Engine/Renderer/VertexArray.h"
//#include "Engine/Renderer/Shader.h"
#include "Engine/Scene/SceneManager.h"
//#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/Application.h"
//#include "Engine/Renderer/UniformBuffer.h"
#include "Engine/Utils/File.h"

namespace Engine
{
	Renderer::Renderer()
	{
		ENGINE_PROFILE_FUNCTION();
		SceneManager::RegisterObserver(this);
		m_Device = Application::Get().GetGraphicsContext()->GetDevice();
		m_Queue = Application::Get().GetGraphicsContext()->GetQueue();
		Resize(m_Width, m_Height);

		//initialize pipeline
		wgpu::ShaderModule shaderModule = LoadShader("resources/shaders/testshader.wgsl");

		// Create the render pipeline
		wgpu::RenderPipelineDescriptor pipelineDesc;


		wgpu::VertexBufferLayout vertexBufferLayout;
		std::vector<wgpu::VertexAttribute> vertexAttribs(2);

		// Describe the position attribute
		vertexAttribs[0].shaderLocation = 0; // @location(0)
		vertexAttribs[0].format = wgpu::VertexFormat::Float32x2;
		vertexAttribs[0].offset = 0;

		// Describe the color attribute
		vertexAttribs[1].shaderLocation = 1; // @location(1)
		vertexAttribs[1].format = wgpu::VertexFormat::Float32x3;
		vertexAttribs[1].offset = 2 * sizeof(float); // non null offset!

		vertexBufferLayout.attributeCount = static_cast<uint32_t>(vertexAttribs.size());
		vertexBufferLayout.attributes = vertexAttribs.data();

		vertexBufferLayout.arrayStride = 5 * sizeof(float);
		vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;


		pipelineDesc.vertex.bufferCount = 1;
		pipelineDesc.vertex.buffers = &vertexBufferLayout;

		// NB: We define the 'shaderModule' in the second part of this chapter.
		// Here we tell that the programmable vertex shader stage is described
		// by the function called 'vs_main' in that module.
		pipelineDesc.vertex.module = shaderModule;
		pipelineDesc.vertex.entryPoint = { "vs_main", strlen("vs_main") };
		pipelineDesc.vertex.constantCount = 0;
		pipelineDesc.vertex.constants = nullptr;

		// Each sequence of 3 vertices is considered as a triangle
		pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;

		// We'll see later how to specify the order in which vertices should be
		// connected. When not specified, vertices are considered sequentially.
		pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;

		// The face orientation is defined by assuming that when looking
		// from the front of the face, its corner vertices are enumerated
		// in the counter-clockwise (CCW) order.
		pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;

		// But the face orientation does not matter much because we do not
		// cull (i.e. "hide") the faces pointing away from us (which is often
		// used for optimization).
		pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

		// We tell that the programmable fragment shader stage is described
		// by the function called 'fs_main' in the shader module.
		wgpu::FragmentState fragmentState;
		fragmentState.module = shaderModule;
		fragmentState.entryPoint = { "fs_main", strlen("fs_main") };
		fragmentState.constantCount = 0;
		fragmentState.constants = nullptr;

		wgpu::BlendState blendState;
		blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
		blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
		blendState.color.operation = wgpu::BlendOperation::Add;
		blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
		blendState.alpha.dstFactor = wgpu::BlendFactor::One;
		blendState.alpha.operation = wgpu::BlendOperation::Add;

		wgpu::ColorTargetState colorTarget;
		colorTarget.format = WGPUTextureFormat_RGBA8Unorm;
		colorTarget.blend = &blendState;
		colorTarget.writeMask = wgpu::ColorWriteMask::All; // We could write to only some of the color channels.

		// We have only one target because our render pass has only one output color
		// attachment.
		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTarget;
		pipelineDesc.fragment = &fragmentState;

		// We do not use stencil/depth testing for now
		pipelineDesc.depthStencil = nullptr;

		// Samples per pixel
		pipelineDesc.multisample.count = 1;

		// Default value for the mask, meaning "all bits on"
		pipelineDesc.multisample.mask = ~0u;

		// Default value as well (irrelevant for count = 1 anyways)
		pipelineDesc.multisample.alphaToCoverageEnabled = false;
		pipelineDesc.layout = nullptr;

		m_Pipeline = m_Device.createRenderPipeline(pipelineDesc);

		// We no longer need to access the shader module
		shaderModule.release();



		//initialize buffers
		std::vector<float> pointData = {
			// x,   y,     r,   g,   b
			-0.5, -0.5,   1.0, 0.0, 0.0, // Point #0
			+0.5, -0.5,   0.0, 1.0, 0.0, // Point #1
			+0.5, +0.5,   0.0, 0.0, 1.0, // Point #2
			-0.5, +0.5,   1.0, 1.0, 0.0  // Point #3
		};

		std::vector<uint16_t> indexData = {
			0, 1, 2, // Triangle #0 connects points #0, #1 and #2
			0, 2, 3  // Triangle #1 connects points #0, #2 and #3
		};


		m_IndexCount = static_cast<uint32_t>(indexData.size());

		// Create vertex buffer
		wgpu::BufferDescriptor bufferDesc;
		bufferDesc.size = pointData.size() * sizeof(float);
		bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex; // Vertex usage here!
		bufferDesc.mappedAtCreation = false;
		m_PointBuffer = m_Device.createBuffer(bufferDesc);

		// Upload geometry data to the buffer
		m_Queue.writeBuffer(m_PointBuffer, 0, pointData.data(), bufferDesc.size);

		// Create index buffer
		// (we reuse the bufferDesc initialized for the pointBuffer)
		bufferDesc.size = indexData.size() * sizeof(uint16_t);
		bufferDesc.size = (bufferDesc.size + 3) & ~3; // round up to the next multiple of 4
		bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index;
		m_IndexBuffer = m_Device.createBuffer(bufferDesc);

		m_Queue.writeBuffer(m_IndexBuffer, 0, indexData.data(), bufferDesc.size);
	}

	Renderer::~Renderer()
	{
		ENGINE_PROFILE_FUNCTION();
		SceneManager::UnregisterObserver(this);
	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		ENGINE_PROFILE_FUNCTION();

		m_Width = width;
		m_Height = height;

		wgpu::TextureDescriptor desc{};
		desc.label = { "RendererTexture", WGPU_STRLEN };
		desc.dimension = WGPUTextureDimension_2D;
		desc.format = WGPUTextureFormat_RGBA8Unorm;
		desc.size = { width, height, 1 };
		desc.mipLevelCount = 1;
		desc.sampleCount = 1;
		desc.usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding;
		m_OutputTexture = m_Device.createTexture(desc);

		wgpu::TextureViewDescriptor viewDesc{};
		viewDesc.label = { "RendererTextureView", WGPU_STRLEN };
		viewDesc.dimension = WGPUTextureViewDimension_2D;
		viewDesc.format = desc.format;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;
		m_OutputView = m_OutputTexture.createView(viewDesc);
	}

	void Renderer::BeginScene(const Camera& camera)
	{
		ENGINE_PROFILE_FUNCTION();

		//TODO: setup camera stuff here

		//TODO: set uniform buffers here
	}

	void Renderer::RenderScene()
	{
		ENGINE_PROFILE_FUNCTION();

		std::vector<Engine::Entity> entities = m_Scene->GetEntities<TransformComponent, MeshComponent>();
		for (auto entity : entities)
		{
			auto transform = entity.GetComponent<TransformComponent>();
			auto mesh = entity.GetComponent<MeshComponent>();

			//TODO: draw mesh
			//Renderer::DrawMesh(transform.GetTransform(), mesh, (int)entity);

			
		}



		wgpu::RenderPassColorAttachment colorAttachment{};
		colorAttachment.view = m_OutputView;
		colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		colorAttachment.loadOp = WGPULoadOp_Clear;
		colorAttachment.storeOp = WGPUStoreOp_Store;
		colorAttachment.clearValue = m_ClearColor;

		wgpu::RenderPassDescriptor renderPassDesc{};
		renderPassDesc.label = { "RenderPassDescriptor", WGPU_STRLEN };
		renderPassDesc.colorAttachmentCount = 1;
		renderPassDesc.colorAttachments = &colorAttachment;

		wgpu::CommandEncoderDescriptor encoderDesc = {};
		encoderDesc.label = { "RendererCommandEncoderDescriptor", WGPU_STRLEN };
		m_CommandEncoder = m_Device.createCommandEncoder(encoderDesc);

		m_RenderPass = m_CommandEncoder.beginRenderPass(renderPassDesc);
		m_RenderPass.setPipeline(m_Pipeline);
		m_RenderPass.setVertexBuffer(0, m_PointBuffer, 0, m_PointBuffer.getSize());
		m_RenderPass.setIndexBuffer(m_IndexBuffer, wgpu::IndexFormat::Uint16, 0, m_IndexBuffer.getSize());
		m_RenderPass.drawIndexed(m_IndexCount, 1, 0, 0, 0);
		m_RenderPass.end();
		m_RenderPass.release();

		wgpu::CommandBuffer commandBuffer = m_CommandEncoder.finish();
		Application::Get().GetGraphicsContext()->GetQueue().submit(1, &commandBuffer);
	}


	void Renderer::EndScene()
	{
		ENGINE_PROFILE_FUNCTION();

	}

	wgpu::ShaderModule Renderer::LoadShader(const std::string& path)
	{
		auto content = Engine::ReadFile(path);
		if (!content)
		{
			LOG_ERROR("Failed to load test shader");
			return nullptr;
		}

		const char* shaderSource = content.value().c_str();

		wgpu::ShaderModuleDescriptor shaderDesc;
		wgpu::ShaderModuleWGSLDescriptor shaderCodeDesc;
		shaderCodeDesc.chain.next = nullptr;
		shaderCodeDesc.chain.sType = wgpu::SType::ShaderSourceWGSL;
		shaderDesc.nextInChain = &shaderCodeDesc.chain;
		shaderCodeDesc.code = { shaderSource, strlen(shaderSource) };

		return m_Device.createShaderModule(shaderDesc);
	}

	//void Renderer::DrawMesh(const glm::mat4& transform, Ref<Mesh> mesh, Ref<Material> material, int entityID)
	//{
	//	ENGINE_PROFILE_FUNCTION();

	//	if (mesh == nullptr) { return; }
	//	if (material == nullptr) { material = AssetManager::GetAsset<Material>("assets/materials/pink.material"); } 

	//	const Ref<Shader> shader = material->GetShader();
	//	shader->Bind();
	//	DynamicVertex vertex = shader->GetVertex();

	//	const std::vector<glm::vec3>& vertices = mesh->GetVertices();
	//	const auto& normals = mesh->GetNormals();
	//	const auto& uvs = mesh->GetUVs();
	//	auto indices = mesh->GetIndices();

	//	int vertexCount = mesh->GetVertexCount();
	//	uint32_t indicesCount = indices.size();

	//	BufferLayout shaderInputLayout = shader->GetInputBufferLayout();

	//	//PER VERTEX DATA
	//	std::vector<uint8_t> vertexBufferData = std::vector<uint8_t>(vertexCount * shaderInputLayout.size());
	//	size_t currentOffset = 0;
	//	for (uint32_t i = 0; i < vertexCount; i++) 
	//	{
	//		for (auto& element : shaderInputLayout) 
	//		{
	//			auto name = element.Name;
	//			auto size = element.Size;

	//			if (name == "EntityID") 
	//			{
	//				memcpy(vertexBufferData.data() + currentOffset, &entityID, size);
	//			}
	//			else if (name == "Position") 
	//			{
	//				glm::vec3 position = transform * glm::vec4(vertices[i], 1.0f);
	//				memcpy(vertexBufferData.data() + currentOffset, &position, size);
	//			}
	//			else if (name == "Normal")
	//			{
	//				glm::vec3 normal = glm::normalize(glm::vec3(transform * glm::vec4(normals[i], 0.0f)));
	//				memcpy(vertexBufferData.data() + currentOffset, &normal, size);
	//			}
	//			else if (name == "Tangent")
	//			{
	//				glm::vec3 tangent = glm::vec3(1.0f, 0.0f, 0.0f); // Default tangent, can be calculated if needed
	//				memcpy(vertexBufferData.data() + currentOffset, &tangent, size);
	//			}
	//			else if (name == "Bitangent")
	//			{
	//				glm::vec3 bitangent = glm::vec3(0.0f, 1.0f, 0.0f); // Default bitangent, can be calculated if needed
	//				memcpy(vertexBufferData.data() + currentOffset, &bitangent, size);
	//			}
	//			else if (name == "UV")
	//			{
	//				glm::vec2 uv = uvs.empty() ? glm::vec2(0.0f) : uvs[i];
	//				memcpy(vertexBufferData.data() + currentOffset, &uv, size);
	//			}
	//			else if (name == "Color")
	//			{
	//				glm::vec4 color = material->GetProperty<glm::vec4>("Color");
	//				memcpy(vertexBufferData.data() + currentOffset, &color, size);
	//			}
	//			else if (name == "Metallic")
	//			{
	//				float metallic = material->GetProperty<float>("Metallic");
	//				memcpy(vertexBufferData.data() + currentOffset, &metallic, size);
	//			}
	//			else if (name == "Roughness")
	//			{
	//				float roughness = material->GetProperty<float>("Roughness");
	//				memcpy(vertexBufferData.data() + currentOffset, &roughness, size);
	//			}
	//			else if (name == "Height")
	//			{
	//				float height = material->GetProperty<float>("Height");
	//				memcpy(vertexBufferData.data() + currentOffset, &height, size);
	//			}
	//			else if (name == "AmbientOcclusion")
	//			{
	//				float ambientOcclusion = material->GetProperty<float>("AmbientOcclusion");
	//				memcpy(vertexBufferData.data() + currentOffset, &ambientOcclusion, size);
	//			}


	//			currentOffset += size; 
	//		}
	//	}

	//	//Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertexCount * shaderInputLayout.size());
	//	Ref<VertexBuffer> vertexBuffer = CreateRef<VertexBuffer>(vertexCount * shaderInputLayout.size());
	//	vertexBuffer->SetLayout(shaderInputLayout);		
	//	vertexBuffer->SetData(vertexBufferData.data(), (uint32_t)vertexBufferData.size());

	//	//Ref<IndexBuffer> IndexBuffer = IndexBuffer::Create(indices.data(), indicesCount);
	//	//Ref<IndexBuffer> IndexBuffer = CreateRef<IndexBuffer>(indices.data(), indicesCount);
	//	Ref<VertexArray> vertexArray = CreateRef<VertexArray>();
	//	vertexArray->AddVertexBuffer(vertexBuffer);
	//	//vertexArray->SetIndexBuffer(IndexBuffer);



	//	//MATERIAL DATA
	//	BufferLayout shaderMaterialBufferLayout = shader->GetMaterialBufferLayout();
	//	if (!shaderMaterialBufferLayout.empty()) 
	//	{
	//		std::vector<uint8_t> bufferData(shaderMaterialBufferLayout.size());
	//		for (const auto& var : shaderMaterialBufferLayout)
	//		{
	//			std::string name = var.Name;
	//			ShaderDataType type = var.Type;
	//			uint32_t size = var.Size;
	//			uint32_t offset = var.Offset;

	//			switch (type)
	//			{
	//			case ShaderDataType::Float:
	//			{
	//				float value = material->GetProperty<float>(name);
	//				memcpy(bufferData.data() + offset, &value, size);
	//				break;
	//			}
	//			case ShaderDataType::Float2:
	//			{
	//				glm::vec2 value = material->GetProperty<glm::vec2>(name);
	//				memcpy(bufferData.data() + offset, &value, size);
	//				break;
	//			}
	//			case ShaderDataType::Float3:
	//			{
	//				glm::vec3 value = material->GetProperty<glm::vec3>(name);
	//				memcpy(bufferData.data() + offset, &value, size);
	//				break;
	//			}

	//			default:
	//				break;
	//			}
	//		}

	//		const Ref<UniformBuffer> materialUniformBuffer = material->GetUniformBuffer();
	//		materialUniformBuffer->SetData(bufferData.data(), bufferData.size(), 0);
	//		materialUniformBuffer->Bind(1);

	//	}


	//	RenderCommand::DrawIndexed(indicesCount);

	//	s_Stats.DrawCalls++;
	//	s_Stats.VertexCount += vertexCount;
	//	s_Stats.IndicesCount += indicesCount;
	//}


	//Renderer::RendererStatistics Renderer::GetStats()
	//{
	//	return s_Stats;
	//}

	//void Renderer::ResetStats()
	//{
	//	memset(&s_Stats, 0, sizeof(RendererStatistics));
	//}

	//void Renderer::AlignOffset(size_t& currentOffset, size_t alignment)
	//{
	//	size_t padding = currentOffset % alignment;
	//	if (padding != 0)
	//	{
	//		currentOffset += (alignment - padding);
	//	}
	//}
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