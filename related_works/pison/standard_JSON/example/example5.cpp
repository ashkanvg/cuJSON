#include <bits/stdc++.h>
#include "../src/RecordLoader.h"
#include "../src/BitmapIterator.h"
#include "../src/BitmapConstructor.h"

// $.aliases[*].ilo[0:1].value
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


// $.claims.P150[*].mainsnak.property --> paper
string query(BitmapIterator* iter) {
    string output = "";
    if (iter->isObject() && iter->moveToKey("claims")) {
        if (iter->down() == false) return output;
        if(iter->isObject() && iter->moveToKey("P150")){
            if (iter->down() == false) return output;
            
            while (iter->isArray() && iter->moveNext() == true) {
                if (iter->down() == false) continue;
                if (iter->isObject() && iter->moveToKey("mainsnak")) {
                    if (iter->down() == false) continue; /* value of "ilo" */
                    if (iter->isArray()) {
                        for (int idx = 0; idx <= 1; ++idx) {
                            // first and second elements inside "ilo" array
                            if (iter->moveToIndex(idx)) {
                                if (iter->down() == false) continue;
                                if (iter->isObject() && iter->moveToKey("property")) {
                                    // value of "id"
                                    char* value = iter->getValue();
                                    output.append(value).append(";");
                                    if (value) free(value);
                                }
                                iter->up();
                            }
                        }
                    }
                    iter->up();
                }
                iter->up();
            }
            iter->up();
        }
        iter->up();
          /* value of "products" */
    }
    return output;
}

int main() {
    char* file_path = "../../../Test-Files/Pison Large Datasets/wiki_large_record.json";
    
    auto start2 = chrono::high_resolution_clock::now();
    Record* rec = RecordLoader::loadSingleRecord(file_path);    
    if (rec == NULL) {
        cout<<"record loading fails."<<endl;
        return -1;
    }
    auto end2 = chrono::high_resolution_clock::now();
    double time_taken_2 = chrono::duration_cast<chrono::nanoseconds>(end2 - start2).count();
    time_taken_2 *= 1e-9;
    cout << "Time taken by program is (loader): " << fixed << time_taken_2 << setprecision(9);
    cout << " sec" << endl;

    auto start = chrono::high_resolution_clock::now();

    /* process the input record in serial order: first build bitmap,
     * then perform the query with a bitmap iterator
     */
    int thread_num = 4;
    int level_num = 5;
    Bitmap* bm = BitmapConstructor::construct(rec, thread_num,level_num);
    BitmapIterator* iter = BitmapConstructor::getIterator(bm);
    string output = query(iter);
    

    auto end = chrono::high_resolution_clock::now();

    double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-9;
 
    cout << "Time taken by program is : " << fixed << time_taken << setprecision(9);
    cout << " sec" << endl;

    delete iter;
    delete bm;
    delete rec;

    //cout<<"matches are: "<<output<<endl;    
    return 0;
}
