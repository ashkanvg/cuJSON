// rapidjson/example/simpledom/simpledom.cpp`
#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/filereadstream.h"
#include "include/rapidjson/error/en.h"

#include <fstream>
#include <iostream>
#include <chrono>
#include <string>

#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

 
using namespace rapidjson;
using namespace std;
using namespace std::chrono;

 
void calcFunction(const char* fileName, int which);

int main() {

    // Open the file
    // const char* fileName3 = "../Test-Files/nspl_large_record.json";
    // calcFunction(fileName3, 0);

    const char* fileName4 = "../Test-Files/twitter_large_record.json";
    calcFunction(fileName4, 1);
    // calcFunction(fileName4, 2);
    // calcFunction(fileName4, 3);
    // calcFunction(fileName4, 4);

    const char* fileName6 = "../Test-Files/walmart_large_record.json";
    calcFunction(fileName6, 5);

    // const char* fileName5 = "../Test-Files/wiki_large_record.json";
    // calcFunction(fileName5, 6);
    // calcFunction(fileName5, 7);

    // const char* fileName2 = "../Test-Files/google_map_large_record.json";
    // calcFunction(fileName2, 8);
    // calcFunction(fileName2, 9);

    // const char* fileName = "../Test-Files/bestbuy_large_record.json";
    // calcFunction(fileName, 10);
    // calcFunction(fileName, 11);



    return 0;


}


void calcFunction(const char* fileName, int which){
    cout << "FILE NAME:" << fileName << endl;
    // Open the file
    FILE* fp = fopen(fileName, "r");
  
    // Check if the file was opened successfully
    if (!fp) {
        std::cerr << "Error: unable to open file"<< std::endl;
        return;
    }

    time_t start, end;
    // start = clock();
    char readBuffer[65536];

    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    start = clock();

    rapidjson::Document doc;
    doc.ParseStream(is);
    // Check if the document is valid
    if (doc.HasParseError()) {
        std::cerr << "Error: failed to parse JSON document"<< std::endl;
        cerr << "Error parsing JSON: "<< doc.GetParseError() << endl;

        fclose(fp);
        return;
    }

    
    end = clock();
    std::cout << "parse: " << ((double)(end-start)/CLOCKS_PER_SEC)*1000 << " miliseconds" << std::endl;
    
    high_resolution_clock::time_point startQ, stopQ;
    startQ = high_resolution_clock::now();

    // view.columns[0].name
    switch(which){
        // case 0:
        //     {Value& s0 = doc["meta"]["view"]["columns"][0]["name"];
        //     // cout << "VALUE:" << s.GetString() << endl;
        //     break;}
        case 1:{
            // tt1 /created_at
            Value& s1 = doc[0]["user"]["lang"];
            Value& s11 = doc[0]["lang"];
            // cout << "VALUE:" << s.GetString() << endl;
            // cout << "VALUE:" << s2.GetString() << endl;
            break;
        }
        case 2:{
            // tt2:
            Value& s2 = doc[0]["user"]["id"];
            Value& s22 = doc[0]["user"]["lang"];

            // cout << "VALUE:" << s.GetString() << endl;
            // cout << "VALUE:" << s2.GetString() << endl;
            break;}
        case 3:
            {// t{t3
            Value& s3 = doc[0]["user"]["id"];

            // cout << "VALUE:" << s.GetString() << endl;
            break;}
        case 4:
            {// tt4
            Value& s4 = doc[0]["entities"]["urls"][0]["indices"][0];
            // cout << "VALUE:" << s.GetString() << endl;

            break;}
        case 5:
            {// walmart
            // Value& s5 = doc["items"][0]["bestMarketplacePrice"]["price"];
            Value& s5 = doc["items"][0];
            // Value& s55 = doc["items"][0]["name"];
            // cout << "VALUE:" << s.GetString() << endl;
            // cout << "VALUE:" << s2.GetString() << endl;

            break;}
        // case 6:
        //     {// wiki:
        //     Value& s6 = doc[0]["descriptions"];
        //     // cout << "VALUE:" << s.GetString() << endl;
        //     break;}
        // case 7:
        //     {// wiki2
        //     Value& s7 = doc[0]["claims"]["P1245"][0]["mainsnak"]["property"];
        //     // cout << "VALUE:" << s.GetString() << endl;
        //     break;}
        // case 8:
        //     {// gg
        //     Value& s8 = doc[0]["routes"];
        //     // cout << "VALUE:" << s.GetString() << endl;
        //     break;}
        // case 9:
        //     {// gg
        //     Value& s9 = doc[0]["routes"][0]["legs"][0]["steps"][0]["distance"]["text"];
        //     // cout << "VALUE:" << s.GetString() << endl;
        //     break;}
        // case 10:
        //     {// bb1
        //     Value& s10 = doc["products"][0]["regularPrice"];
        //     // cout << "VALUE:" << s.GetString() << endl;
        //     break;}
        // case 11:
        //     {// bb2
        //     Value& s111 = doc["products"][0]["categoryPath"][1]["id"];
        //     Value& s112 = doc["products"][0]["categoryPath"][1]["id"];
        //     Value& s113 = doc["products"][0]["categoryPath"][1]["id"];  

        //     // cout << "VALUE:" << s.GetString() << endl;
        //     // cout << "VALUE:" << s2.GetString() << endl;
        //     // cout << "VALUE:" << s3.GetString() << endl;

        //     break;}
    }
    // Value& s = doc["meta"]["view"]["columns"][0]["name"];
    // cout << "VALUE:" << s.GetString();
    
    stopQ = high_resolution_clock::now();
    auto elapsed = duration_cast<nanoseconds>(stopQ - startQ);
    cout << "Total query time: " << elapsed.count() << " nanoseconds." << endl;

    // Close the file
    fclose(fp);
  


}
