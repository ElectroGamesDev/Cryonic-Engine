#include "Utilities.h"
#include <ShlObj.h>

void Utilities::OpenPathInExplorer(std::filesystem::path path)
{
	ShellExecute(NULL, L"open", path.wstring().c_str(), NULL, NULL, SW_SHOWDEFAULT);
}

std::string Utilities::SelectFolderDialog(const std::filesystem::path& projectPath)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    std::string path;

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* pFileOpen;

        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            DWORD dwOptions;
            hr = pFileOpen->GetOptions(&dwOptions);
            if (SUCCEEDED(hr))
            {
                pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);  // Use FOS_PICKFOLDERS for selecting folders

                IShellItem* pDefaultFolder;
                hr = SHCreateItemFromParsingName(projectPath.c_str(), NULL, IID_IShellItem, reinterpret_cast<void**>(&pDefaultFolder));

                if (SUCCEEDED(hr))
                {
                    pFileOpen->SetFolder(pDefaultFolder);
                    pDefaultFolder->Release();
                }
            }

            hr = pFileOpen->Show(NULL);

            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    if (SUCCEEDED(hr))
                    {
                        int len = WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, NULL, 0, NULL, NULL);
                        std::string str(len, '\0');
                        WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, &str[0], len, NULL, NULL);
                        path = str;

                        CoTaskMemFree(pszFilePath);
                    }
                    else
                    {
                        path = "";
                    }
                    pItem->Release();
                }
                else
                {
                    path = "";
                }
            }
            else
            {
                path = "";
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }

    path.erase(std::remove_if(path.begin(), path.end(), [](char c) {
        return !std::isprint(static_cast<unsigned char>(c));
        }), path.end());
    return path;
}