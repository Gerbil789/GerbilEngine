#include "Editor/Utility/File.h"

#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <shobjidl.h> // For IFileDialog
#include <shellapi.h>
#include <thread>
#include <algorithm>

namespace Editor
{
	std::string OpenFile()
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL; // Can be set to a parent window handle if you have one
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "All Files\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		// OFN_NOCHANGEDIR prevents the dialog from changing the app's working directory
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			return std::string(szFile);
		}

		return "";
	}

	std::string SaveFile()
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "All Files\0*.*\0";
		ofn.nFilterIndex = 1;
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

	std::filesystem::path OpenDirectory()
	{
		std::filesystem::path resultPath;

		// Initialize COM for the thread. 
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		bool needToUninitialize = SUCCEEDED(hr);

		IFileOpenDialog* pFolderDialog = NULL;

		// Create the FileOpenDialog object
		if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFolderDialog))))
		{
			DWORD dwOptions;
			if (SUCCEEDED(pFolderDialog->GetOptions(&dwOptions)))
			{
				// Set options to pick folders only and force file system paths
				pFolderDialog->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
			}

			// Show the dialog
			if (SUCCEEDED(pFolderDialog->Show(NULL)))
			{
				IShellItem* pItem;
				if (SUCCEEDED(pFolderDialog->GetResult(&pItem)))
				{
					PWSTR pszFolderPath = NULL;

					// Get the path string
					if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath)))
					{
						resultPath = pszFolderPath;
						CoTaskMemFree(pszFolderPath);
					}
					pItem->Release();
				}
			}
			pFolderDialog->Release();
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