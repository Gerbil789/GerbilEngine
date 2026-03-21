#include "RenderDoc.h"
#include "Engine/Utility/Path.h"
#include "Engine/Core/Log.h"
#include <renderdoc_app.h>

namespace RenderDoc
{
	RENDERDOC_API_1_6_0* rdoc_api = nullptr;

	void Initialize()
	{
		std::filesystem::path renderDocPath = GetExecutableDir() / "renderdoc.dll";

		if (!std::filesystem::exists(renderDocPath))
		{
			LOG_WARNING("RenderDoc DLL not found at {}, skipping RenderDoc integration", renderDocPath);
			return;
		}

		HMODULE module = LoadLibraryA(renderDocPath.string().c_str());
		if (!module)
		{
			LOG_ERROR("Failed to load RenderDoc DLL at {}", renderDocPath);
			return;
		}

		RENDERDOC_Version version = eRENDERDOC_API_Version_1_6_0;
		pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(module, "RENDERDOC_GetAPI");

		if (RENDERDOC_GetAPI)
		{
			int ret = RENDERDOC_GetAPI(version, (void**)&rdoc_api);
			if (ret != 1)
			{
				LOG_ERROR("Failed to initialize RenderDoc API");
			}

			LOG_TRACE("Successfully initialized RenderDoc API");
			RENDERDOC_InputButton captureKeys[1] = { RENDERDOC_InputButton::eRENDERDOC_Key_F1 };
			rdoc_api->SetCaptureKeys(captureKeys, 1);

			rdoc_api->SetCaptureFilePathTemplate("captures/frame");
		}
	}
}




