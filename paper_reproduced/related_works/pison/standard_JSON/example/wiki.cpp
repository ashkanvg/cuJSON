#include <bits/stdc++.h>
#include "../src/RecordLoader.h"
#include "../src/BitmapIterator.h"
#include "../src/BitmapConstructor.h"

// $.aliases[0].ilo[0:1].value
// string query(BitmapIterator* iter) {
//     string output = "";
//     if (iter->isObject() && iter->moveToKey("aliases")) {
//         if (iter->down() == false) return output;  /* value of "products" */
//         while (iter->isArray() && iter->moveNext() == true) {
//             if (iter->down() == false) continue;
//             if (iter->isObject() && iter->moveToKey("ilo")) {
//                 if (iter->down() == false) continue; /* value of "ilo" */
//                 if (iter->isArray()) {
//                     for (int idx = 0; idx <= 1; ++idx) {
//                         // first and second elements inside "ilo" array
//                         if (iter->moveToIndex(idx)) {
//                             if (iter->down() == false) continue;
//                             if (iter->isObject() && iter->moveToKey("value")) {
//                                 // value of "id"
//                                 char* value = iter->getValue();
//                                 output.append(value).append(";");
//                                 if (value) free(value);
//                             }
//                             iter->up();
//                         }
//                     }
//                 }
//                 iter->up();
//             }
//             iter->up();
//         }
//         iter->up();
//     }
//     return output;
// }


// $.claims.P150[0].mainsnak.property --> paper
string query(BitmapIterator* iter) {
    string output = "";
    while (iter->isArray() && iter->moveNext() == true) {
        auto start_query = std::chrono::high_resolution_clock::now();    
        if (iter->down() == false) continue;
        if (iter->isObject() && iter->moveToKey("claims")) {
            if (iter->down() == false) return output;
            if(iter->isObject() && iter->moveToKey("P150")){
                if (iter->down() == false) return output;
                while (iter->isArray() && iter->moveNext() == true) {
                    if (iter->down() == false) continue;
                    if (iter->isObject() && iter->moveToKey("mainsnak")) {
                        if (iter->down() == false) continue; 
                        if (iter->isObject() && iter->moveToKey("property")) {

                            char* value = iter->getValue();
                            output.append(value).append(";");
                            if (value) free(value);
                            auto end_query = std::chrono::high_resolution_clock::now();
                            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_query - start_query);
                            std::cout << "query: " << duration.count() << " nanoseconds" << std::endl;

                            // cout << "final";
                            return output;
                        }
                        iter->up();
                    }
                    iter->up();
                }  
                iter->up(); 
            }
            iter->up();
        }
        iter->up();
    }

    // if (iter->isObject() && iter->moveToKey("claims")) {
    //     if (iter->down() == false) return output;
    //     if(iter->isObject() && iter->moveToKey("P150")){
    //         if (iter->down() == false) return output;
            
    //         while (iter->isArray() && iter->moveNext() == true) {
    //             if (iter->down() == false) continue;
    //             if (iter->isObject() && iter->moveToKey("mainsnak")) {
    //                 if (iter->down() == false) continue; 
    //                 if (iter->isArray()) {
    //                     for (int idx = 0; idx <= 1; ++idx) {
    //                         // first and second elements inside "ilo" array
    //                         if (iter->moveToIndex(idx)) {
    //                             if (iter->down() == false) continue;
    //                             if (iter->isObject() && iter->moveToKey("property")) {
    //                                 // value of "id"
    //                                 char* value = iter->getValue();
    //                                 output.append(value).append(";");
    //                                 if (value) free(value);
    //                             }
    //                             iter->up();
    //                         }
    //                     }
    //                 }
    //                 iter->up();
    //             }
    //             iter->up();
    //         }
    //         iter->up();
    //     }
    //     iter->up();
    // }
    return output;
}

int main() {
    char* file_path = "../../../../../dataset/wiki_large_record.json";
    
    // auto start2 = chrono::high_resolution_clock::now();
    Record* rec = RecordLoader::loadSingleRecord(file_path);    
    if (rec == NULL) {
        cout<<"record loading fails."<<endl;
        return -1;
    }
    // auto end2 = chrono::high_resolution_clock::now();
    // double time_taken_2 = chrono::duration_cast<chrono::nanoseconds>(end2 - start2).count();
    // time_taken_2 *= 1e-9;
    // cout << "Time taken by program is (loader): " << fixed << time_taken_2 << setprecision(9);
    // cout << " sec" << endl;

    auto start = chrono::high_resolution_clock::now();

    /* process the input record in serial order: first build bitmap,
     * then perform the query with a bitmap iterator
     */
    int thread_num = 4;
    int level_num = 5;
    Bitmap* bm = BitmapConstructor::construct(rec, thread_num,level_num);
    BitmapIterator* iter = BitmapConstructor::getIterator(bm);

    // auto start_query = std::chrono::high_resolution_clock::now();    
    // string output = query(iter);
    // auto end_query = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_query - start_query);
    delete iter;
    delete bm;
    delete rec;

    // std::cout << "query: " << duration.count() << " nanoseconds" << std::endl;
                                    

    auto end = chrono::high_resolution_clock::now();


    double time_taken_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << time_taken_ms << setprecision(3) << endl;

    // delete iter;
    // delete bm;
    // delete rec;

    //cout<<"matches are: "<<output<<endl;    
    return 0;
}
