#include "../src/RecordLoader.h"
#include "../src/BitmapIterator.h"
#include "../src/BitmapConstructor.h"

// $.products[*].categoryPath[1:3].id
string query(BitmapIterator* iter) {
    string output = "";
    if (iter->isObject() && iter->moveToKey("products")) {
        if (iter->down() == false) return output;  /* value of "products" */
        while (iter->isArray() && iter->moveNext() == true) {
            if (iter->down() == false) continue;
            if (iter->isObject() && iter->moveToKey("categoryPath")) {
                if (iter->down() == false) continue; /* value of "categoryPath" */
                if (iter->isArray()) {
                    for (int idx = 1; idx <= 2; ++idx) {
                        // 2nd and 3rd elements inside "categoryPath" array
                        if (iter->moveToIndex(idx)) {
                            if (iter->down() == false) continue;
                            if (iter->isObject() && iter->moveToKey("id")) {
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
    return output;
}

int main() {
    char* file_path = "../dataset/bestbuy_sample_large_record.json";
    Record* rec = RecordLoader::loadSingleRecord(file_path);
    if (rec == NULL) {
        cout<<"record loading fails."<<endl;
        return -1;
    }

    /* process the input record in serial order: first build bitmap,
     * then perform the query with a bitmap iterator
     */
    Bitmap* bm = BitmapConstructor::construct(rec);
    BitmapIterator* iter = BitmapConstructor::getIterator(bm);
    string output = query(iter);
    delete iter;
    delete bm;
    delete rec;

    cout<<"matches are: "<<output<<endl;    
    return 0;
}
