#include "cujson/cujsonlines.h"

int main(int argc, char **argv) {
    std::string filePath = "./dataset/twitter_sample_small_records.json";
    // Check command-line arguments
    if (argc >= 2) {
        filePath = argv[1];  // XML file path
        cout << "\033[1;36m[INFORM]\033[0m Using custom JSON file from command line: " << filePath << "\n";
    } else {
        std::cout << "\033[1;36m[INFORM]\033[0m Using default JSON file path.\n";
    }



    // Load File
    cuJSONLinesInput input = loadJSONLines_chunkCount(filePath, 4);
    if (!input.data) {
        std::cout << "\033[1;31m[ERR]\033[0m File loading failed. Please check the file path.\n";
        return EXIT_FAILURE;
    }
    
    cout << "\033[1;32m[INFORM]\033[0m File loaded successfully. Size: " << input.size << " bytes\n";
    cout << "\033[1;32m[INFORM]\033[0m Number of chunks: " << input.chunkCount << "\n";
    cout << "\033[1;32m[INFORM]\033[0m Chunk sizes: ";
    for (size_t i = 0; i < input.chunksSize.size(); ++i) {
        std::cout << input.chunksSize[i] << " ";
    }
    std::cout << "\n";  


    // Parse JSON with cuJSON
    cuJSONResult parsed_tree = parse_json_lines(input);
    if(parsed_tree.structural == nullptr) {
        std::cout << "\033[1;31m[ERR]\033[0m JSON parsing failed.\n";
        cudaFreeHost(input.data);
        return EXIT_FAILURE;
    }



    // Process the parsed tree as needed
    // For example, you can print the parsed tree size or perform further operations
    std::cout << "\033[1;32m[RESULT]\033[0m Parsed tree size: " << parsed_tree.totalResultSize << " elements\n";


    // print values of parsed_tree.structural
    // std::cout << "\033[1;32m[RESULT]\033[0m Parsed tree structural values:\n";
    // for (int i = 0; i < parsed_tree.totalResultSize && i < 150; ++i) {
    //     std::cout << parsed_tree.structural[i] << " ";
    // }
    // std::cout << "\n";
    
        
    // // Or you can traverse it for the query purpose:
    cuJSONLinesIterator itr = cuJSONLinesIterator(&parsed_tree, filePath.c_str());

    // //TT1
    int index0;
    index0 = itr.gotoArrayIndex(0);
    index0 = itr.gotoKey("lang");
    std::cout << "\033[1;32m[RESULT]\033[0m Query Value of $[0].user.lang: " << itr.getValue() << "\n";

    itr.freeJson();


    cudaDeviceReset();
    return 0;
}