#pragma once

namespace Engine
{
	class Runtime
	{
	public:
		static void LoadScripts();

		static void Start();
		static void Stop();
		static void Update();
	};
}