// cujson_types.h
#ifndef CUJSON_TYPES_H
#define CUJSON_TYPES_H

#include <cstdint>
#include <cstddef>
#include <vector>

struct cuJSONInput {
    uint8_t* data;                          // pointer to the data buffer
    size_t size;                            // size of the input data
};

struct cuJSONLinesInput {
    uint8_t* data;                          // pointer to the data buffer
    size_t chunkCount;                      // number of chunks in the parser
    size_t size;                            // size of the input data
    std::vector<uint8_t*> chunks;           // vector of pointers to each chunk
    std::vector<size_t> chunksSize;         // vector of size to each chunk
};

struct cuJSONResult{
    uint8_t* inputJSON;                     // JSON file
    int chunkCount;                         // number of chunk in parser
    int bufferSize;                         // size of buffer in parser
    std::vector<int> resultSizes;           // array of size of each chunk
    std::vector<int> resultSizesPrefix;     // prefix sums over sizes of each chunk
    int32_t* structural;                    // real json position of each structural array
    int32_t* pair_pos;                      // ending idx of each opening in structural will store in that corresponding idx
    int depth;                              // max depth of JSON file
    int totalResultSize;                    // total size of our array | tree size
    int fileSize;                           // JSON file size
};

enum tokens_type_enum { OBJECT,ARRAY,KEYVALUE,VALUE,CLOSING }; 
typedef tokens_type_enum token_type;

enum primitive_type_enum { NUMBER,TRUE,FALSE,NULL_TYPE,STRING }; 
typedef primitive_type_enum primitive_type;

#endif  // CUJSON_TYPES_H
