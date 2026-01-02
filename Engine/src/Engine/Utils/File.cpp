#include "enginepch.h"
#include "File.h"
#include <portable-file-dialogs.h>

namespace Engine
{
	bool ReadFile(const std::filesystem::path& path, std::string& outData)
	{
		std::ifstream in(path, std::ios::in | std::ios::binary);
		if (!in)
		{
			LOG_ERROR("Could not open file '{}'", path);
			return false;
		}

		outData.assign((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		return true;
	}

	std::string OpenFile()
	{
		auto result = pfd::open_file("Open File", ".", { "All Files", "*" }).result();
		return !result.empty() ? result[0] : "";
	}

	std::string SaveFile()
	{
		auto result = pfd::save_file("Save File", ".", { "All Files", "*" }).result();
		return result;
	}

	std::string OpenDirectory()
	{
		auto result = pfd::select_folder("Select Directory", ".").result();
		return result.empty() ? "" : result;
	}

	void OpenFileExplorer(const std::filesystem::path& path)
	{
		std::string p = path.string();

		// Run in background so UI doesn't freeze
		std::thread([p]() {
#if defined(_WIN32)
			std::string winPath = p;
			std::replace(winPath.begin(), winPath.end(), '/', '\\');
			ShellExecuteA(nullptr, "open", winPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
			std::string cmd = "xdg-open \"" + p + "\"";
			system(cmd.c_str());
#endif
			}).detach();
	}
}