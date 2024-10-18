#ifndef PARALLELBITMAP_H
#define PARALLELBITMAP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <malloc.h>
#include <sys/time.h>
#include <sys/file.h>
#include <unistd.h>
#include <sched.h>
#include "Bitmap.h"
#include "LocalBitmap.h"

#define MAX_THREAD 70
#define SPECULATIVE 10
#define NONSPECULATIVE 11

class ParallelBitmap : public Bitmap {
    friend class ParallelBitmapConstructor;
    friend class ParallelBitmapIterator;
  private:
    LocalBitmap* mBitmaps[MAX_THREAD];
    int mThreadNum;
    char* mRecord;
    long mRecordLength;
    int mDepth;
    int mParallelMode;

  public:
    ParallelBitmap(char* record, int thread_num, int depth);
    ParallelBitmap(char* record, long rec_len, int thread_num, int depth);
    ~ParallelBitmap();
    void setRecordLength(long length);
    // SPECULATIVE or NONSPECULATIVE
    int parallelMode();
    // validation after step 3
    void rectifyStringMaskBitmaps();
    // validation after step 5
    void mergeBitmaps();
};
#endif
