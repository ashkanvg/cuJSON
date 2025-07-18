#ifndef BITMAP_H
#define BITMAP_H

#include <iostream>
using namespace std;

#define MAX_LEVEL 22
#define SEQUENTIAL 1
#define PARALLEL 2

class Bitmap {
    friend class BitmapConstructor;
  private:
    int type;
  public:
    Bitmap() {
        type = SEQUENTIAL;
    }
    virtual void setRecordLength(long length) {}
    virtual void indexConstruction() {}
    virtual void setStreamFlag(bool flag) {}
    virtual ~Bitmap() {}
};
#endif
