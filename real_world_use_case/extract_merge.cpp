#include <iostream>
#include <fstream>
#include <filesystem>
#include <zlib.h>

namespace fs = std::filesystem;

bool decompressGzipToFile(const fs::path& gzPath, std::ofstream& outMerged) {
    gzFile inputFile = gzopen(gzPath.c_str(), "rb");
    if (!inputFile) {
        std::cerr << "Failed to open " << gzPath << "\n";
        return false;
    }

    constexpr size_t bufferSize = 8192;
    char buffer[bufferSize];

    int bytesRead = 0;
    while ((bytesRead = gzread(inputFile, buffer, bufferSize)) > 0) {
        outMerged.write(buffer, bytesRead);
    }

    gzclose(inputFile);
    // Add newline to separate files if not already
    // outMerged << '\n';
    return true;
}

int main() {
    std::string folder = "/home/csgrads/aveda002/Desktop/CUDA-Test/JSONPARSING/Test-Files/use-case/1_1_2025_to_1_2_2025"; // Change this to your folder path
    std::string outputPath = "github_archive_small_records.json";
    std::ofstream outMerged(outputPath, std::ios::binary);
    if (!outMerged) {
        std::cerr << "Failed to open " << outputPath << " for writing.\n";
        return 1;
    }

    for (const auto& entry : fs::directory_iterator(folder)) {
        if (entry.path().extension() == ".gz") {
            std::cout << "Processing: " << entry.path().filename() << std::endl;
            if (!decompressGzipToFile(entry.path(), outMerged)) {
                std::cerr << "Failed to decompress: " << entry.path() << "\n";
            }
        }
    }

    outMerged.close();
    std::cout << "All files merged into: " << outputPath << std::endl;
    return 0;
}
