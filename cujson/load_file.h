// loadfile.h
#ifndef LOADFILE_H
#define LOADFILE_H

#include <string>
#include "cujson_types.h"

// Function to load JSON file content into a string
cuJSONInput loadJSON(const std::string& filePath);
cuJSONLinesInput loadJSONLines(const std::string& filePath);


#endif // LOADFILE_H