#include "../cujson/cujsonlines.h"

int main(int argc, char **argv) {
    std::string filePath = "../dataset/merged_output.json";
    // Check command-line arguments
    if (argc >= 2) {
        filePath = argv[1];  // XML file path
        cout << "\033[1;36m[INFORM]\033[0m Using custom JSON file from command line: " << filePath << "\n";
    } else {
        std::cout << "\033[1;36m[INFORM]\033[0m Using default JSON file path.\n";
    }



    // Load File
    size_t maxChunkSizeMB = 512; // 256MB
    cuJSONLinesInput input = loadJSONLines_chunkSizeMegaBytes(filePath, maxChunkSizeMB);
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
    
    // report time with chrono in milliseconds:
    auto start = std::chrono::high_resolution_clock::now();
    cuJSONResult parsed_tree = parse_json_lines(input);
    if(parsed_tree.structural == nullptr) {
        std::cout << "\033[1;31m[ERR]\033[0m JSON parsing failed.\n";
        cudaFreeHost(input.data);
        return EXIT_FAILURE;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);





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

    // report time with chrono in milliseconds:

    cuJSONLinesIterator itr = cuJSONLinesIterator(&parsed_tree, filePath.c_str());

    auto start_itr = std::chrono::high_resolution_clock::now();
    // //TT1
    int index0;
    index0 = itr.gotoArrayIndex(0);
    int count = 0;
    for(int i = 0; i < 211259; i++) {
    // for(int i = 0; i < 5; i++) {
        bool condition = itr.checkKeyValue("type", "PushEvent");
        if(condition == true){
            index0 = itr.gotoKey("id");
            count++;
            // std::cout << "\033[1;32m[RESULT]\033[0m Query Value of $[i].id: " << itr.getValue() << "\n";
        }
        // index0 = itr.gotoKey("id");
        // index0 = itr.gotoKey("repo");
        // index0 = itr.gotoKey("name");
        // itr.reset();
        index0 = itr.gotoNextSibling(1);

    } 
    // int fileLines = 2111259;

    // cuJSONLinesIterator itr = cuJSONLinesIterator(&parsed_tree, filePath.c_str());
    // int index0 = itr.gotoArrayIndex(0);
    // int count = 0;
    // for(int i = 0; i < fileLines; i++) {
    //     bool condition = itr.checkKeyValue("type", "PushEvent");
    //     if(condition == true){
    //         index0 = itr.gotoKey("repo");
    //         index0 = itr.gotoKey("name");            
    //         count++;
            
    //     }
    //     index0 = itr.gotoNextSibling(1);
    // } 

    auto end_itr = std::chrono::high_resolution_clock::now();
    auto duration_itr = std::chrono::duration_cast<std::chrono::nanoseconds>(end_itr - start_itr);

    cout << "\033[1;32m[RESULT]\033[0m Total PushEvent count: " << count << "\n";

    // std::cout << "\033[1;32m[RESULT]\033[0m Query Value of $[0].repo.name: " << itr.getValue() << "\n";


    // report time of parse and query and total time:
    std::cout << "\033[1;32m[RESULT]\033[0m Parsing time: " << duration.count() / 1e6 << " ms\n";
    std::cout << "\033[1;32m[RESULT]\033[0m Query time: " << duration_itr.count() / 1e6 << " ms\n";
    std::cout << "\033[1;32m[RESULT]\033[0m Total time: " << (duration.count() + duration_itr.count()) / 1e6 << " ms\n";      

    itr.freeJson();


    cudaDeviceReset();
    return 0;
}