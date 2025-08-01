#ifndef BITMAPITERATOR_H
#define BITMAPITERATOR_H

#include "Bitmap.h"
#include <unordered_set>
using namespace std;

class Bitmap;

#define OBJECT 1
#define ARRAY 2
#define PRIMITIVE 3

#define ERR -1
#define MAX_FIELD_SIZE 1000

// Metadata for parsing and querying, saves context information at each level when iterating leveled bitmap.
struct IterCtxInfo {
    // current thread id for parsing and querying. Only used during leveled bitmap iteration.
    int thread_id;
    // OBJECT or ARRAY
    int type;
    // position array for colon and comma
    long* positions;
    // start index of the record position array at the current level
    long start_idx;
    // end index of the record position array at the current level
    long end_idx;
    // current index of the record position array at the current level
    long cur_idx;
    // the current level
    int level;
};

struct KeyPos {
    long start;
    long end;
};

class BitmapIterator {
    friend class BitmapConstructor;
    friend class BitmapConstructor1;
  private:
    int type;
  public:
    int mVisitedFields;
  public:
    // Creates a copy of iterator. Often used for parallel querying.
    virtual BitmapIterator* getCopy() = 0;
    // Moves back to the object or array which contains the current nested record. 
    // Often used when the current nested record has been processed. 
    // Valid except for the first level of the record. 
    virtual bool up() = 0; 
    // Moves to the start of the nested object or array. 
    // Gets all colon or comma positions from leveled bitmap indexes for current nested record. 
    // Valid if we are at { or [.
    virtual bool down() = 0;
    // Whether the iterator points to an object.
    virtual bool isObject() = 0;
    // Whether the iterator points to an array.
    virtual bool isArray() = 0;
    // Moves iterator to the next array item.
    virtual bool moveNext() = 0; 
    // Moves to the corresponding key field inside the current object.
    virtual bool moveToKey(char* key) = 0;
    // Moves to the corresponding key fields inside the current object, returns the current key name.
    // After this operation, the current key field will be removed from key_set.
    virtual char* moveToKey(unordered_set<char*>& key_set) = 0;
    // Returns the number of elements inside current array.
    virtual int numArrayElements() = 0;
    // If the current record is an array, moves to an item based on index.
    // Returns false if the index is out of the boundary.
    virtual bool moveToIndex(int index) = 0;
    // Gets the content of the current value inside an object or array. 
    virtual char* getValue() = 0;
    virtual ~BitmapIterator() {

    }
};
#endif
