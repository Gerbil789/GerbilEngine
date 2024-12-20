#pragma once

namespace Engine
{
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::None:     return 0;	// 0 bytes
		case ShaderDataType::Float:    return 4;	// 4 bytes
		case ShaderDataType::Float2:   return 8;	// 2 * 4 bytes
		case ShaderDataType::Float3:   return 12;	// 3 * 4 bytes
		case ShaderDataType::Float4:   return 16;	// 4 * 4 bytes
		case ShaderDataType::Mat3:     return 36;	// 3 * 3 * 4 bytes
		case ShaderDataType::Mat4:     return 64;	// 4 * 4 * 4 bytes
		case ShaderDataType::Int:      return 4;	// 4 bytes
		case ShaderDataType::Int2:     return 8;	// 2 * 4 bytes
		case ShaderDataType::Int3:     return 12;	// 3 * 4 bytes
		case ShaderDataType::Int4:     return 16;	// 4 * 4 bytes
		case ShaderDataType::Bool:     return 1;	// 1 byte
		default: ASSERT(false, "Unknown ShaderDataType!"); return 0;
		}
	}



	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type = ShaderDataType::None;
		uint32_t Size = 0;
		uint32_t Offset = 0;

		BufferElement() = default;
		BufferElement(ShaderDataType type, const std::string& name) : Name(name), Type(type), Size(ShaderDataTypeSize(type)) {}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
			case ShaderDataType::Float:    return 1;
			case ShaderDataType::Float2:   return 2;
			case ShaderDataType::Float3:   return 3;
			case ShaderDataType::Float4:   return 4;
			case ShaderDataType::Mat3:     return 9;
			case ShaderDataType::Mat4:     return 16;
			case ShaderDataType::Int:      return 1;
			case ShaderDataType::Int2:     return 2;
			case ShaderDataType::Int3:     return 3;
			case ShaderDataType::Int4:     return 4;
			case ShaderDataType::Bool:     return 1;
			default: ASSERT(false, "Unknown ShaderDataType!"); return 0;
			}
		}
	};


	class BufferLayout
	{
	public:
		BufferLayout() {}
		BufferLayout(std::vector<BufferElement> elements) : m_Elements(elements)
		{
			CalculateOffsetAndStride();
		}

		void Push(const ShaderDataType& type, const std::string& name)
		{
			m_Elements.emplace_back(type, name);
			CalculateOffsetAndStride();
		}

		uint32_t count() const { return m_Elements.size(); }
		size_t size() const { return m_Size; }
		bool empty() const { return m_Elements.empty(); }
		inline uint32_t GetStride() const { return m_Stride; }

		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }

		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		void CalculateOffsetAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			m_Size = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
				m_Size += element.Size;
			}
		}

	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
		size_t m_Size = 0;
	};



	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		static Ref<VertexBuffer> Create(uint32_t size);
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};
}