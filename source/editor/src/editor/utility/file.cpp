#include "file.h"

#ifdef ENGINE_PLATFORM_WINDOWS
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <shobjidl.h> // For IFileDialog
#include <shellapi.h>
#include <wrl/client.h> // For Microsoft::WRL::ComPtr
#include <thread>
#include <algorithm>
#include <shlobj.h> // Required for Shell API
#endif
namespace editor::FileDialog
{
	void OpenFileExplorer(const std::filesystem::path& path)
	{

#ifdef ENGINE_PLATFORM_WINDOWS
		// 1. Ensure the path is absolute and uses native windows backslashes
		std::filesystem::path absPath = std::filesystem::absolute(path);
		absPath.make_preferred();
		std::wstring wPath = absPath.wstring();

		std::error_code ec;
		bool isDirectory = std::filesystem::is_directory(absPath, ec);

		// Run in background so ui doesn't freeze
		std::thread([wPath, isDirectory]() {
			// 2. Initialize COM on this background thread
			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			if (SUCCEEDED(hr))
			{
				// 3. Convert the string path into a windows Item ID List (PIDL)
				LPITEMIDLIST pidl = ILCreateFromPathW(wPath.c_str());
				if (pidl)
				{
					if (isDirectory)
					{
						// Open the directory directly
						SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
					}
					else
					{
						// To reliably select a file, windows needs the PIDL of the parent folder 
						// and a relative child PIDL for the file itself.
						LPITEMIDLIST pidlParent = ILClone(pidl);
						ILRemoveLastID(pidlParent);
						LPCITEMIDLIST pidlChild = ILFindLastID(pidl);

						// This native call guarantees the file will be selected, even on the first open
						SHOpenFolderAndSelectItems(pidlParent, 1, &pidlChild, 0);

						ILFree(pidlParent);
					}
					ILFree(pidl); // Clean up memory
				}
				CoUninitialize();
			}
			}).detach();

#endif
	}
}