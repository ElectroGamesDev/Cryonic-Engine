#include "Utilities.h"
#include "ConsoleLogger.h"
#include <ShlObj.h>
#include <fileapi.h>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <vector>
#include <chrono>
#include <iomanip>
#include <json.hpp>
#include <algorithm>

void Utilities::HideFile(std::filesystem::path path) // Todo: Figure out why the file isn't being hidden
{
    SetFileAttributes(path.wstring().c_str(), FILE_ATTRIBUTE_HIDDEN);
}

void Utilities::OpenPathInExplorer(std::filesystem::path path)
{
	ShellExecute(NULL, L"open", path.wstring().c_str(), NULL, NULL, SW_SHOWDEFAULT);
}

int Utilities::GetNumberOfCores()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    return sysInfo.dwNumberOfProcessors;
}

std::filesystem::path Utilities::CreateUniqueFile(std::filesystem::path directory, std::string name, std::string extension)
{
    std::filesystem::path filePath(directory);
    filePath /= name + "." + extension;

    int count = 0;
    while (std::filesystem::exists(filePath))
    {
        ++count;
        filePath = std::filesystem::path(directory) / (name + " (" + std::to_string(count) + ")." + extension);
    }

    std::ofstream file(filePath);
    if (file.is_open())
    {
        file.close();
        return filePath;
    }
    return "";
}

std::string dataTypes[5] = { "int", "float", "bool", "char", "string" };
std::vector<std::pair<std::string, std::vector<std::string>>> enums; // Consider changing this to a unordered_set as it has random access
nlohmann::json Utilities::GetExposedVariables(std::filesystem::path path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        ConsoleLogger::WarningLog("Failed to open script " + path.string() + " to get exposed variables.");
        return nullptr; // Empty
    }

    nlohmann::json json = nlohmann::json::array();

    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(std::filesystem::last_write_time(path) - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
    std::time_t tt = std::chrono::system_clock::to_time_t(sctp);

    json.push_back(static_cast<int>(tt));


    nlohmann::json variablesJson = nlohmann::json::array();
    int accessModifier = 0; // 0 = private, 1 = public, 2 = protected
    std::string line;
    std::pair<std::string, std::vector<std::string>> definingEnum;
    while (std::getline(file, line))
    {
        //line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
        if (line.find("class ") != std::string::npos) // Todo: This is needed incase there are multiple classes in the header, but if a variable/function has "class" in the name, it will switch acess modifier to private ALTHOUGH having a space at the end may may this not an issue
            accessModifier = 0;
        if (line.find("public") != std::string::npos) // Not using else-if incase if this is same line as class
            accessModifier = 1;
        else if (line.find("private") != std::string::npos)
            accessModifier = 0;
        else if (line.find("public") != std::string::npos)
            accessModifier = 2;

        size_t enumPos = line.find("enum");
        if (enumPos != std::string::npos)
        {
            std::string tempLine = line;
            tempLine.erase(std::remove_if(tempLine.begin(), tempLine.end(), ::isspace), tempLine.end()); // Remove spaces
            tempLine = tempLine.substr(enumPos + 3); // Remove everything before enum name
            size_t bracePos = tempLine.find("{"); // Check if the line has a "{"
            if (bracePos != std::string::npos && tempLine != "{") // Checks if it has a "{" and it has values in it
            {
                definingEnum.first = tempLine.substr(0, bracePos); // Get the enum name
                tempLine = tempLine.substr(bracePos + 1); // Remove everything before the "{" so now its just the enum values and "}" and ";"

                size_t endBracePos = tempLine.find("}");
                if (bracePos != std::string::npos) // Has end brace on this line
                {
                    tempLine = tempLine.substr(0, endBracePos); // Get rid of everything after "}" so its only the enum values in the string
                    std::stringstream ss(tempLine);
                    while (ss.good())
                    {
                        std::string value;
                        std::getline(ss, value, ',');
                        definingEnum.second.push_back(value);
                    }
                    enums.push_back(definingEnum);
                    definingEnum.first = "";
                    definingEnum.second.clear();
                    continue;
                }
                else // There is no "}", so it will assume the rest of the charaters are enum values
                {
                    std::stringstream ss(tempLine);
                    while (ss.good())
                    {
                        std::string value;
                        std::getline(ss, value, ',');
                        if (value == "")
                            continue;
                        definingEnum.second.push_back(value);
                    }
                    continue;
                }
            }
            else if (tempLine != "")
            {
                definingEnum.first = tempLine; // If there is no "{", its safe to assume only the enum name is remaining on this line
                continue;
            }
        }
        else if (definingEnum.first != "") // Todo: Check if the code in the if above is even running as it seems even with single-lined enums it will still run this code when it shouldn't.
        {
            std::string tempLine = line;
            tempLine.erase(std::remove_if(tempLine.begin(), tempLine.end(), ::isspace), tempLine.end());
            if (tempLine == "" || tempLine == "{")
                continue;

            size_t endBracePos = tempLine.find("}");
            if (endBracePos != std::string::npos)
            {
                tempLine = tempLine.substr(0, endBracePos);
                enums.push_back(definingEnum);
                definingEnum.first = "";
                definingEnum.second.clear();
				continue;
            }
            std::stringstream ss(tempLine);
            while (ss.good())
            {
                std::string value;
                std::getline(ss, value, ',');
                if (value == "")
                    continue;
                definingEnum.second.push_back(value);
            }
            continue;
        }

        // Todo: this won't work if variables are declared among multiple tnes. Instead go through each line until ";" is found
        else if (line.find("(") == std::string::npos && line.find(")") == std::string::npos) // Todo: need to check if the line is commented. Support both single line comment, and multi-line comment. If single-lien commented, make sure to check if the "//" is before the declaration
        {
            if (line.find("Hide") != std::string::npos)
                continue;
            if (line.find(" ") != std::string::npos)
            {
                int type = 0;
                std::string selectedDataType;

                for (const std::string& dataType : dataTypes)
                    if (line.find(dataType) != std::string::npos)
                    {
                        type = 1;
                        selectedDataType = dataType;
                        break;
                    }
                if (type == 0)
                    for (const auto& dataType : enums)
                        if (line.find(dataType.first) != std::string::npos)
                        {
                            type = 2;
                            selectedDataType = dataType.first;
                            break;
                        }
                if (type != 0)
                {
                    if (accessModifier == 1 || line.find("Expose") != std::string::npos)
                    {
                        size_t startPos = line.find(selectedDataType);
                        size_t semicolonPos = line.find(";", startPos);
                        if (semicolonPos != std::string::npos)
                        {
                            std::string variableDeclaration = line.substr(startPos, semicolonPos - startPos);
                            std::stringstream ss(variableDeclaration);
                            std::string word;
                            nlohmann::json jsonArray = nlohmann::json::array();

                            // Adds the variable data type to the json
                            while (ss >> word)
                            {
                                if (word == "=")
                                    continue;
                                jsonArray.push_back(word);
                            }

                            //ConsoleLogger::InfoLog(jsonArray.dump());

                            if (jsonArray.size() <= 2) // Check if variable declared a value
                            {
                                if (jsonArray[0] == "int")
                                    jsonArray.push_back(0);
                                else if (jsonArray[0] == "float")
                                    jsonArray.push_back(0.0f);
                                else if (jsonArray[0] == "bool")
                                    jsonArray.push_back(false);
                                else if (jsonArray[0] == "string" || jsonArray[0] == "char")
                                    jsonArray.push_back("");
                                else if (type == 2) // Check if its an enum
                                {
                                    for (const auto& pair : enums)
                                        if (pair.first == selectedDataType)
                                        {
                                            if (pair.second.size() > 0)
                                                jsonArray.push_back(pair.second[0]);
                                            break;
                                        }
                                }
                            }
                            else
                            {
                                if (jsonArray[0] == "float")
                                {
                                    std::string value = jsonArray[2].get<std::string>();
                                    value.erase(value.size() - 1);
                                    jsonArray[2] = std::stof(value);
                                }
                                else if (jsonArray[0] == "string" || jsonArray[0] == "char")
                                {
                                    std::string value = jsonArray[2].get<std::string>();
                                    value.erase(0, 1);
                                    value.erase(value.size() - 1);
                                    jsonArray[2] = value;
                                }
                                else if (jsonArray[0] == "bool")
                                    jsonArray[2] = (jsonArray[2] == "true") ? true : false;
                            }

                            // Adding another variable name, and making the first letter capitalized
                            jsonArray.push_back(jsonArray[1]);
                            std::string newName = jsonArray.back();
                            if (!newName.empty()) {
                                newName[0] = std::toupper(newName[0]);

                                // Adding spaces to new variable name
                                bool firstLetter = true;
                                std::string name = newName;
                                newName = "";
                                for (char c : name)
                                {
                                    if (c == '_')
                                        newName += ' ';
                                    else if (std::isupper(c))
                                    {
                                        if (!firstLetter)
                                            newName += ' ';
                                        newName += c;
                                    }
                                    else
                                        newName += c;
                                    firstLetter = false;
                                }

                                jsonArray.back() = newName;
                            }

                            if (type == 2)
                                for (const auto& pair : enums)
                                    if (pair.first == selectedDataType)
                                    {
                                        nlohmann::json enumValuesJson = nlohmann::json::array();
                                        for (const std::string& enumValues : pair.second)
                                            enumValuesJson.push_back(enumValues);
                                        jsonArray.push_back(enumValuesJson);
                                        break;
                                    }

                            // Adding another variable value, and making the first letter capitalized
                            //jsonArray.push_back(jsonArray[2]);
                            //if (jsonArray.back().is_string())
                            //{
                            //    std::string newValue = jsonArray.back();
                            //    if (!newValue.empty()) {
                            //        newValue[0] = std::toupper(newValue[0]);
                            //        jsonArray.back() = newValue;
                            //    }
                            //}

                            variablesJson.push_back(jsonArray);
                        }
                    }
                }
            }
        }
    }
    json.push_back(variablesJson);

    file.close();

    return json;
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