#include "AssetManager.h"
#include "IconsFontAwesome6.h"
#include <string>
#include "ConsoleLogger.h"
#include "curl/curl.h"
#include <json.hpp>
#include <vector>
#include "RaylibWrapper.h"
#include "Utilities.h"
#include <filesystem>
#include "ProjectManager.h"
#include "ImGuiNotify.hpp"

namespace AssetManager
{
    bool open = false;
    ImGuiWindowClass* windowClass;
    
    struct AssetData
    {
        std::string name;
        std::string description;
        std::string author;
        std::string category;
        std::string url;
        RaylibWrapper::Texture2D icon;
    };
    std::vector<AssetData> cachedAssets;
    bool assetsCached = false;
    bool connectedToInternet = true;

    size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* data)
    {
        data->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    size_t WriteImageDataToMemory(void* contents, size_t size, size_t nmemb, std::vector<unsigned char>* buffer) {
        size_t totalSize = size * nmemb;
        buffer->insert(buffer->end(), (unsigned char*)contents, (unsigned char*)contents + totalSize);
        return totalSize;
    }

    void RenderWindow()
    {
        if (!open)
        {
            if (assetsCached)
                Cleanup();

            return;
        }

        // Todo: Add pages

        if (!assetsCached && connectedToInternet) // Using connectedToInternet since it prevents the program from being stuck constantly checking if it has an internet connection
        {
            if (Utilities::HasInternetConnection())
                FetchAssets();
            else
                connectedToInternet = false;
        }

        ImGui::SetNextWindowClass(windowClass);
        if (ImGui::Begin((ICON_FA_BOX_ARCHIVE + std::string(" Asset Manager")).c_str(), &open, ImGuiWindowFlags_NoCollapse))
        {
            ImVec2 cursorPos = { 20, 40 };
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            for (AssetData& assetData : cachedAssets)
            {
                ImGui::SetCursorPos(cursorPos);
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.8f, 0.8f, 0.8f));
                // Todo: This is probably not best, but i'm not sure what else I could use
                ImGui::BeginChild(("##" + assetData.url).c_str(), { 150, 150 }, ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollbar);

                ImGui::SetCursorPos({ 32, 5 });
                RaylibWrapper::rlImGuiImageSizeV(&assetData.icon, { 85, 85 });

                ImGui::SetCursorPos({ (ImGui::GetWindowWidth() - ImGui::CalcTextSize(assetData.name.c_str()).x) * 0.5f, 95});
                ImGui::Text(assetData.name.c_str());

                ImGui::SetCursorPos({ 37, 120 });
                if (ImGui::Button(("Install##" + assetData.url).c_str(), { 75, 25 }))
                    InstallAsset(assetData.url);

                ImGui::EndChild();
                ImGui::PopStyleColor(2);
                ImGui::PopStyleVar(2);

                cursorPos = { ImGui::GetCursorPosX() + 180, cursorPos.y };
                if (cursorPos.x > ImGui::GetWindowWidth() - 180) // Todo: Will this cause an infinite loop if the window is really small?
                    cursorPos = { 5, ImGui::GetCursorPosY() + 180 };
            }
        }
        ImGui::End();
    }

    void CheckForUpdates()
    {
    }

    bool InstallAsset(std::string url)
    {
        // TODO: Support dependancies

        // Check if Git is installed
        if (!Utilities::IsProgramInstalled("git --version"))
        {
            ConsoleLogger::ErrorLog("Package Manager - Failed to download asset due to Git not being installed. Try again after installing it.");
            ImGuiWindow* window = ImGui::FindWindowByName((ICON_FA_CODE + std::string(" Console")).c_str());
            if (window != NULL && window->DockNode != NULL && window->DockNode->TabBar != NULL)
                window->DockNode->TabBar->NextSelectedTabId = window->TabId;

            ImGuiToast toast(ImGuiToastType::Error, 2500, true);
            toast.setTitle("Package Manager", "");
            toast.setContent("Failed to download asset, please install Git first.");
            ImGui::InsertNotification(toast);

            return false;
        }

        if (!std::filesystem::exists(ProjectManager::projectData.path / "Assets" / "Third Party"))
            std::filesystem::create_directory(ProjectManager::projectData.path / "Assets" / "Third Party");

        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        std::string command = "cmd.exe /C \"cd /d \"" + std::filesystem::path(ProjectManager::projectData.path / "Assets" / "Third Party").string() + "\" && git clone " + url + "\"";

        if (!CreateProcessA(NULL, const_cast<LPSTR>(command.c_str()), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi))
        {
            ConsoleLogger::ErrorLog("Package Manager - Failed to install asset.");

            ImGuiToast toast(ImGuiToastType::Error, 2500, true);
            toast.setTitle("Package Manager", "");
            toast.setContent("Failed to install asset.");
            ImGui::InsertNotification(toast);
            return false;
        }

        while (true)
        {
            DWORD waitResult = WaitForSingleObject(pi.hProcess, 100);
            if (waitResult == WAIT_OBJECT_0)
                break;
            else if (waitResult != WAIT_TIMEOUT)
            {
                ConsoleLogger::ErrorLog("Package Manager - Failed to install asset.");

                ImGuiToast toast(ImGuiToastType::Error, 2500, true);
                toast.setTitle("Package Manager", "");
                toast.setContent("Failed to install asset.");
                ImGui::InsertNotification(toast);
                return false;
            }
        }

        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        if (exitCode != 0)
        {
            ConsoleLogger::ErrorLog("Package Manager - Failed to install asset.");

            ImGuiToast toast(ImGuiToastType::Error, 2500, true);
            toast.setTitle("Package Manager", "");
            toast.setContent("Failed to install asset.");
            ImGui::InsertNotification(toast);
            return false;
        }

        return true;
    }

    bool UpdateAsset(std::string url)
    {
        return true;
    }

    void Init(ImGuiWindowClass* winClass)
    {
        windowClass = winClass;
        curl_global_init(CURL_GLOBAL_ALL); // This would be called in Editor.cpp, but including Curl causes some issues
    }

    void Cleanup()
    {
        for (AssetData& assetData : cachedAssets)
            RaylibWrapper::UnloadTexture(assetData.icon);

        cachedAssets.clear();
        assetsCached = false;
    }

    std::vector<unsigned char> DownloadImageToMemory(const std::string& url)
    {
        CURL* curl = curl_easy_init();
        std::vector<unsigned char> imageData;

        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteImageDataToMemory);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &imageData);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            // Todo: This disables the SSL Verification. For some reason curl_easy_perform fails when it is enabled
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                ConsoleLogger::ErrorLog("Image download failed : " + std::string(curl_easy_strerror(res)));
                imageData.clear();
            }
            curl_easy_cleanup(curl);
        }
        return imageData;
    }

    AssetData FetchAsset(std::string url)
    {
        std::string readBuffer;
        CURL* curl = curl_easy_init();
        if (curl)
        {
            //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            // Todo: This disables the SSL Verification. For some reason curl_easy_perform fails when it is enabled
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                curl_easy_cleanup(curl);
                return AssetData();
            }

            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (response_code != 200)
            {
                ConsoleLogger::ErrorLog("HTTP request failed. Error: " + std::string(curl_easy_strerror(res)), true);
                return AssetData();
            }

            curl_easy_cleanup(curl);
        }
        else
            return AssetData();

        try
        {
            nlohmann::json assets = nlohmann::json::parse(readBuffer);

            for (const auto& asset : assets)
            {
                // Add this
            }
        }
        catch (nlohmann::json::parse_error& e)
        {
            ConsoleLogger::ErrorLog("JSON parsing error: " + std::string(e.what()), true);
            return AssetData();
        }
    }

    bool FetchAssets()
    {
        std::string readBuffer;
        CURL* curl = curl_easy_init();
        if (curl)
        {
            //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl, CURLOPT_URL, "https://assetregistry.cryonicengine.com/assets.json");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            // Todo: This disables the SSL Verification. For some reason curl_easy_perform fails when it is enabled
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                curl_easy_cleanup(curl);
                return false;
            }

            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (response_code != 200)
            {
                ConsoleLogger::ErrorLog("HTTP request failed. Response code: " + response_code, true);
                return false;
            }

            curl_easy_cleanup(curl);
        }
        else
            return false;

        try
        {
            nlohmann::json assets = nlohmann::json::parse(readBuffer);

            for (const auto& asset : assets)
            {
                if (asset.contains("name") && asset.contains("description") && asset.contains("author") && asset.contains("url") && asset.contains("category"))
                {
                    // Todo: Check if the icon page exists

                    std::string imageUrl = asset["url"].get<std::string>();
                    size_t pos = imageUrl.find("github.com");
                    if (pos != std::string::npos)
                    {
                        imageUrl.replace(pos, 10, "raw.githubusercontent.com");
                        imageUrl += "/refs/heads/main/icon.png";
                    }

                    AssetData assetData;
                    assetData.name = asset["name"].get<std::string>();
                    assetData.description = asset["description"].get<std::string>();
                    assetData.author = asset["author"].get<std::string>();
                    assetData.category = asset["category"].get<std::string>();
                    assetData.url = asset["url"].get<std::string>();

                    // Github only supports HTTPS, this curl lib only works with HTTP
                    // Once HTTPS is supported, change the website in HasInternetConnection() in Utilities.
                    
                    std::vector<unsigned char> imageData = DownloadImageToMemory(imageUrl);
                    RaylibWrapper::Image image = RaylibWrapper::LoadImageFromMemory(".png", imageData.data(), imageData.size());
                    assetData.icon = RaylibWrapper::LoadTextureFromImage(image);
                    RaylibWrapper::UnloadImage(image);

                    cachedAssets.push_back(assetData);
                }
            }
        }
        catch (nlohmann::json::parse_error& e)
        {
            ConsoleLogger::ErrorLog("JSON parsing error: " + std::string(e.what()), true);
            return false;
        }

        assetsCached = true;
        return true;
    }
}