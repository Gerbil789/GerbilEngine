#include "enginepch.h"
#include "YAMLSerializer.h"

namespace Engine
{
	void YamlSerializer::BeginMap(const std::string& key)
	{
		m_Stack.push_back({ ContextType::Map, key, {} });
	}

	void YamlSerializer::EndMap()
	{
		FlushCurrentContext();
	}

	void YamlSerializer::BeginSequence(const std::string& key)
	{
		m_Stack.push_back({ ContextType::Sequence, key, {} });
	}

	void YamlSerializer::EndSequence()
	{
		FlushCurrentContext();
	}

	void YamlSerializer::SaveToFile(const std::filesystem::path& filepath) const
	{
		std::ofstream fout(filepath);
		for (const auto& line : m_FinalLines)
		{
			fout << line << "\n";
		}
		fout.close();
	}

	void Engine::YamlSerializer::FlushCurrentContext()
	{
		Context ctx = m_Stack.back();
		m_Stack.pop_back();

		std::vector<std::string> output;

		bool isSequenceItem =
			(ctx.type == ContextType::Map && ctx.key.empty() &&
				!m_Stack.empty() && m_Stack.back().type == ContextType::Sequence);

		if (isSequenceItem)
		{
			// Anonymous map inside sequence
			if (!ctx.lines.empty())
			{
				output.push_back("- " + ctx.lines[0]);
				for (size_t i = 1; i < ctx.lines.size(); ++i)
					output.push_back("  " + ctx.lines[i]);
			}
			else
			{
				output.push_back("-"); // Empty item
			}
		}
		else
		{
			if (!ctx.key.empty())
				output.push_back(ctx.key + ":");

			for (const auto& line : ctx.lines)
				output.push_back("  " + line);
		}

		// Determine whether to indent or not
		if (!m_Stack.empty())
		{
			// Inside a parent map or sequence: write as-is
			m_Stack.back().lines.insert(m_Stack.back().lines.end(), output.begin(), output.end());
		}
		else
		{
			// Top-level: write without indentation
			for (const std::string& line : output)
			{
				if (line.starts_with("  "))
					m_FinalLines.push_back(line.substr(2)); // Remove 2 spaces
				else
					m_FinalLines.push_back(line);
			}
		}
	}


}