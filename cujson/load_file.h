// loadfile.h
#ifndef LOADFILE_H
#define LOADFILE_H

#include <string>
#include "cujson_types.h"

// Function to load JSON file content into a string
cuJSONInput loadJSON(const std::string& filePath);
cuJSONLinesInput loadJSONLines_chunkCount(const std::string& filePath, size_t chunkCount);
cuJSONLinesInput loadJSONLines_chunkSizeBytes(const std::string& filePath, size_t chunkSizeBytes);
cuJSONLinesInput loadJSONLines_chunkSizeMegaBytes(const std::string& filePath, size_t chunkSizeMegaBytes);

#endif // LOADFILE_H