#include <iostream>
#include <filesystem>
#include <windows.h>
#include <chrono>
#include <thread>

namespace fs = std::filesystem;

// Function to calculate the total size of the Temp folder
uintmax_t calculateFolderSize(const fs::path& folderPath) {
    uintmax_t totalSize = 0;
    try {
        for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
            if (fs::is_regular_file(entry.path())) {
                totalSize += fs::file_size(entry.path());
            }
        }
    }
    catch (const std::exception& e) {
        std::wcerr << L"Error calculating size: " << folderPath << L" - " << e.what() << std::endl;
    }
    return totalSize;
}

std::wstring getTempDirectory() {
    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    return std::wstring(tempPath);
}

void cleanTempFolder() {
    std::wstring tempDir = getTempDirectory();

    try {
        for (const auto& entry : fs::directory_iterator(tempDir)) {
            try {
                if (fs::is_directory(entry.path())) {
                    fs::remove_all(entry.path());
                    std::wcout << L"Deleted folder: " << entry.path() << std::endl;
                }
                else {
                    fs::remove(entry.path());
                    std::wcout << L"Deleted file: " << entry.path() << std::endl;
                }
            }
            catch (const std::exception& e) {
                std::wcerr << L"Could not delete: " << entry.path() << L" - " << e.what() << std::endl;
            }
        }
    }
    catch (const std::exception& e) {
        std::wcerr << L"Error reading directory: " << tempDir << L" - " << e.what() << std::endl;
    }
}

int main() {
    system("title autotempcleaner");

    std::wstring tempDir = getTempDirectory();

    // Calculate the total size of the Temp folder
    uintmax_t totalSizeBytes = calculateFolderSize(tempDir);
    double totalSizeMB = static_cast<double>(totalSizeBytes) / (1024 * 1024); // Convert to MB

    // Output of the size
    std::wcout << L"Total size of Temp folder: " << totalSizeMB << L" MB" << std::endl;

    // Wait 3 seconds
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Clean up the Temp folder
    cleanTempFolder();

    // Calculate new total size of the Temp folder and print it
    totalSizeBytes = calculateFolderSize(tempDir);
    totalSizeMB = static_cast<double>(totalSizeBytes) / (1024 * 1024); // Convert to MB
    std::wcout << L"New total size of Temp folder: " << totalSizeMB << L" MB" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(3));

    system("exit");
}
