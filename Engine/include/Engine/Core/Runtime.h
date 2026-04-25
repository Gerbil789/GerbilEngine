#pragma once

namespace Engine
{
	class ENGINE_API Runtime
	{
	public:
		static void LoadScripts(const std::filesystem::path& dllPath);

		static void Start();
		static void Stop();
		static void Update();
	};
}