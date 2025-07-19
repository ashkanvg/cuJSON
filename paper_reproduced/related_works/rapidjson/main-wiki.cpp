// rapidjson/example/simpledom/simpledom.cpp`
#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/filereadstream.h"
#include "include/rapidjson/error/en.h"

#include <fstream>
#include <iostream>
 #include <malloc.h>
#include <unistd.h>

using namespace rapidjson;
using namespace std;
 

void printMemoryUsage(const std::string& message) {
    std::ifstream file("/proc/self/statm");
    long rss;
    file >> rss;

    // Convert pages to MB
    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
    long resident_set = rss * page_size_kb;
    double resident_set_mb = resident_set / 1024.0;
    std::cout << message << " - Memory Usage: " << resident_set_mb << " MB\n";
}


int main() {
    // 1. Parse a JSON string into DOM.
    // const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    // "../Test-Files/Pison Large Datasets/bestbuy_small_records.json"

    // Open the file
    // FILE* fp = fopen("../Test-Files/Pison Large Datasets/bestbuy_large_record.json", "r");
    // FILE* fp = fopen("../Test-Files/Pison Large Datasets/google_map_large_record.json", "r");
    // FILE* fp = fopen("../Test-Files/Pison Large Datasets/nspl_large_record.json", "r");
    // FILE* fp = fopen("../Test-Files/Pison Large Datasets/walmart_large_record.json", "r");
    // FILE* fp = fopen("../Test-Files/Pison Large Datasets/twitter_large_record.json", "r");
    FILE* fp = fopen("../../../dataset/wiki_large_record.json", "r");
  
    // Check if the file was opened successfully
    if (!fp) {
        std::cerr << "Error: unable to open file"<< std::endl;
        return 1;
    }
    // printMemoryUsage("1"); // Print memory usage after deallocation

    

    
    time_t start, end;
    start = clock();
    char readBuffer[65536];

    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rapidjson::Document doc;
    doc.ParseStream(is);

    // Check if the document is valid
    if (doc.HasParseError()) {
        std::cerr << "Error: failed to parse JSON document"<< std::endl;
        cerr << "Error parsing JSON: "<< doc.GetParseError() << endl;

        fclose(fp);
        return 1;
    }
    // printMemoryUsage("2"); // Print memory usage after deallocation

    end = clock();
    std::cout << ((double)(end-start)/CLOCKS_PER_SEC)*1000 << std::endl;
    
    // Close the file
    fclose(fp);
  
    


    // const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    // Document d;
    // d.Parse(json);
 
    // // 2. Modify it by DOM.
    // Value& s = d["studio"];

    // Get the "name" member
    // if (doc.HasMember("studio") && doc["studio"].IsString()) {
    //     std::string name = doc["studio"].GetString();
    //     std::cout << "studio: " << name << std::endl;
    // }
    // s.SetInt(s.GetInt() + 1);
 
    // // 3. Stringify the DOM
    // StringBuffer buffer;
    // Writer<StringBuffer> writer(buffer);
    // d.Accept(writer);
 
    // // Output {"project":"rapidjson","stars":11}
    // std::cout << buffer.GetString() << std::endl;
    return 0;


    //  // Open the file
    // ifstream file("../Test-Files/bb-aa.json");
  
    // // Read the entire file into a string
    // string json((istreambuf_iterator<char>(file)),istreambuf_iterator<char>());
  
    // // Create a Document object 
    //   // to hold the JSON data
    // Document doc;
  
    // // Parse the JSON data
    // doc.Parse(json.c_str());
  
    // // Check for parse errors
    // if (doc.HasParseError()) {
    //     cerr << "Error parsing JSON: "<< doc.GetParseError() << endl;
    //     return 1;
    // }
  
    // // Now you can use the Document object to access the
    // // JSON data
    // return 0;
}