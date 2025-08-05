#include "cujson/cujson.h"


int main(int argc, char **argv) {

    std::string filePath = "./dataset/twitter_sample_large_record.json";
    // Check command-line arguments
    if (argc >= 2) {
        filePath = argv[1];  // XML file path
        cout << "\033[1;36m[INFORM]\033[0m Using custom JSON file from command line: " << filePath << "\n";
    } else {
        std::cout << "\033[1;36m[INFORM]\033[0m Using default JSON file path.\n";
    }


    // Load File
    cuJSONInput input = loadJSON(filePath);
    if (!input.data) {
        std::cout << "\033[1;31m[ERR]\033[0m File loading failed. Please check the file path.\n";
        return EXIT_FAILURE;
    }
    

    // Parse JSON with cuJSON
    auto start_time = std::chrono::high_resolution_clock::now();
    cuJSONResult parsed_tree = parse_standard_json(input);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end_time - start_time;
    std::cout << "parse_standard_json took " << elapsed.count() << " ms" << std::endl;


    if(parsed_tree.structural == nullptr) {
        std::cout << "\033[1;31m[ERR]\033[0m JSON parsing failed.\n";
        cudaFreeHost(input.data);
        return EXIT_FAILURE;
    }


    // Process the parsed tree as needed
    // For example, you can print the parsed tree size or perform further operations
    std::cout << "\033[1;32m[RESULT]\033[0m Parsed tree size: " << parsed_tree.totalResultSize << " elements\n";


        
    // Or you can traverse it for the query purpose:
    cuJSONIterator itr = cuJSONIterator(&parsed_tree, filePath.c_str());

    //TT1
    // int index0;
    // index0 = itr.gotoArrayIndex(0);
    // index0 = itr.gotoArrayIndex(0);
    // index0 = itr.gotoKey("user");
    // index0 = itr.gotoKey("lang");
    // itr.reset();
    // index0 = itr.gotoArrayIndex(0);
    // index0 = itr.gotoArrayIndex(0);
    // index0 = itr.gotoKey("lang");

    // std::cout << "\033[1;32m[RESULT]\033[0m Query Value of $[0].lang: " << itr.getValue() << "\n";
    itr.freeJson();


    cudaDeviceReset();
    return 0;
}