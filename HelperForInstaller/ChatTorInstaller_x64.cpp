/*
Compile:
g++ ChatTorInstaller_x64.cpp -o ChatTorInstaller_x64.exe -mconsole -lurlmon -lole32 -std=c++17 -static
*/

#include <windows.h>
#include <urlmon.h>
#include <iostream>
#include <filesystem>
#include <vector>

#pragma comment(lib, "urlmon.lib")

const std::string BASE_URL = "https://bendosh4.github.io/ChatTorWebSite/HelperForInstaller/FilesToDownload/";

const std::vector<std::string> FILES = {
    "Client.exe",
    "entry.exe",
    "exit.exe",
    "middle.exe",
    "ChatTorFrontEnd.exe"
};

const std::vector<std::string> DLL_FILES_NEEDED = {
    "libgcc_s_seh-1.dll",
    "libwinpthread-1.dll",
    "libstdc++-6.dll"
};

bool checkAndDownloadDLLs(const std::filesystem::path& installDir)
{
    std::cout << "\n==== Checking Required DLLs ====\n";
    
    bool allSuccess = true;
    std::vector<std::string> missingDLLs;

    for (const auto& dllName : DLL_FILES_NEEDED)
    {
        std::filesystem::path dllPath = installDir / dllName;
        
        if (!std::filesystem::exists(dllPath))
        {
            missingDLLs.push_back(dllName);
            std::cout << dllName << " ... MISSING\n";
        }
        else
        {
            std::cout << dllName << " ... Found\n";
        }
    }

    // Download missing DLLs
    if (!missingDLLs.empty())
    {
        std::cout << "\nDownloading missing DLLs...\n";
        
        for (const auto& dllName : missingDLLs)
        {
            std::string url = BASE_URL + dllName;
            std::filesystem::path destPath = installDir / dllName;

            std::cout << "Downloading " << dllName << " ... ";

            HRESULT hr = URLDownloadToFileA(
                nullptr,
                url.c_str(),
                destPath.string().c_str(),
                0,
                nullptr
            );

            if (SUCCEEDED(hr))
            {
                std::cout << "OK\n";
            }
            else
            {
                std::cout << "FAILED (HRESULT 0x" << std::hex << hr << std::dec << ")\n";
                allSuccess = false;
            }
        }
    }
    else
    {
        std::cout << "All required DLLs are present.\n";
    }

    return allSuccess;
}

int main()
{
    std::cout << "==== ChatTor Installer x64 ====\n\n";

    namespace fs = std::filesystem;

    fs::path installDir = "C:\\ChatTor";

    try
    {
        if (!fs::exists(installDir))
        {
            std::cout << "Creating install directory...\n";
            fs::create_directories(installDir);
        }
    }
    catch (...)
    {
        std::cout << "Failed to create install directory!\n";
        system("pause");
        return 1;
    }

    bool allSuccess = true;

    for (const auto& filename : FILES)
    {
        std::string url = BASE_URL + filename;
        fs::path destPath = installDir / filename;

        std::cout << "Downloading " << filename << " ... ";

        HRESULT hr = URLDownloadToFileA(
            nullptr,
            url.c_str(),
            destPath.string().c_str(),
            0,
            nullptr
        );

        if (SUCCEEDED(hr))
        {
            std::cout << "OK\n";
        }
        else
        {
            std::cout << "FAILED (HRESULT 0x" << std::hex << hr << std::dec << ")\n";
            allSuccess = false;
        }
    }

    if (!allSuccess)
    {
        std::cout << "\nSome files failed to download.\n";
        system("pause");
        return 1;
    }

    // Check and download required DLLs
    if (!checkAndDownloadDLLs(installDir))
    {
        std::cout << "\nSome DLLs failed to download.\n";
        system("pause");
        return 1;
    }

    std::cout << "\nInstallation complete!\n";

    std::string frontendPath = (installDir / "ChatTorFrontEnd.exe").string();

    ShellExecuteA(
        NULL,
        "open",
        frontendPath.c_str(),
        NULL,
        installDir.string().c_str(),
        SW_SHOW
    );

    return 0;
}