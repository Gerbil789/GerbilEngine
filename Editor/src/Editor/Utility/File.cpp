#include "File.h"

#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <shobjidl.h> // For IFileDialog
#include <shellapi.h>
#include <wrl/client.h> // For Microsoft::WRL::ComPtr
#include <thread>
#include <algorithm>

namespace Editor::FileDialog
{
	// Internal helper to translate clean C++ structs into the Win32 double-null string
	static std::string BuildWin32FilterString(std::initializer_list<DialogFilter> filters)
	{
		if (filters.size() == 0)
		{
			return "All Files (*.*)\0*.*\0";
		}

		std::string result;
		for (const auto& filter : filters)
		{
			result += filter.name + " (" + filter.spec + ")";
			result += '\0';
			result += filter.spec;
			result += '\0';
		}
		result += '\0'; // Final terminator required by Windows

		return result;
	}

	std::string SelectFile(std::initializer_list<DialogFilter> filters)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		std::string win32Filter = BuildWin32FilterString(filters);

		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = win32Filter.c_str();
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			return std::string(szFile);
		}

		return "";
	}

	std::string SelectPath(std::initializer_list<DialogFilter> filters, const char* defaultExt)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		std::string win32Filter = BuildWin32FilterString(filters);

		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = win32Filter.c_str();
		ofn.nFilterIndex = 1;
		ofn.lpstrDefExt = defaultExt;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			return std::string(szFile);
		}

		return "";
	}

	std::filesystem::path SelectDirectory()
	{
		std::filesystem::path resultPath;

		// Initialize COM for the thread
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		bool needToUninitialize = SUCCEEDED(hr);

		// Using ComPtr automatically handles ->Release() when going out of scope
		Microsoft::WRL::ComPtr<IFileOpenDialog> pFolderDialog;

		if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFolderDialog))))
		{
			DWORD dwOptions;
			if (SUCCEEDED(pFolderDialog->GetOptions(&dwOptions)))
			{
				pFolderDialog->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
			}

			if (SUCCEEDED(pFolderDialog->Show(NULL)))
			{
				Microsoft::WRL::ComPtr<IShellItem> pItem;
				if (SUCCEEDED(pFolderDialog->GetResult(&pItem)))
				{
					PWSTR pszFolderPath = NULL;
					if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath)))
					{
						resultPath = pszFolderPath;
						CoTaskMemFree(pszFolderPath);
					}
				}
			}
		}

		if (needToUninitialize)
		{
			CoUninitialize();
		}

		return resultPath;
	}

	void OpenFileExplorer(const std::filesystem::path& path)
	{
		std::string p = path.string();

		// Run in background so UI doesn't freeze
		std::thread([p]() {
			std::string winPath = p;
			std::replace(winPath.begin(), winPath.end(), '/', '\\');
			ShellExecuteA(nullptr, "open", winPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
			}).detach();
	}
}