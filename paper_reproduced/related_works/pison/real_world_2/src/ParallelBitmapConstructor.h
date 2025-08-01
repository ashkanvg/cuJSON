#ifndef PARALLELBITMAPCONSTRUCTOR_H
#define PARALLELBITMAPCONSTRUCTOR_H
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
#include "ParallelBitmap.h"
#include "Records.h"

// ParallelBitmapConstructor is a static class
class ParallelBitmapConstructor {
  private:
    static ParallelBitmap* mParallelBitmap;
    static pthread_t mThreads[MAX_THREAD]; 
    static int mThreadArgs[MAX_THREAD];
  public:
    static ParallelBitmap* construct(Record* record, int thread_num, int level_num = MAX_LEVEL);
  private:
    // builds bitmap index in non-speculative mode.
    static void* nonSpecIndexConstruction(void* arg) {
        int thread_id = (int)(*((int*)arg));
        // cout<<"thread "<<thread_id<<" starts building structral indexes."<<endl;
        // bind CPU
        cpu_set_t mask;
        cpu_set_t get;
        CPU_ZERO(&mask);
        CPU_SET(thread_id, &mask);
        if(pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
            cout<<"CPU binding failed for thread "<<thread_id<<endl;
        struct timeval begin,end;
        double duration;
        gettimeofday(&begin,NULL);
        mParallelBitmap->mBitmaps[thread_id]->nonSpecIndexConstruction();
        // cout<<thread_id<<"th thread finishes structural index construction."<<endl;
        gettimeofday(&end,NULL);
        duration=1000000*(end.tv_sec-begin.tv_sec)+end.tv_usec-begin.tv_usec;
        // printf("The duration of index construction in %dth thread is %lf\n",thread_id, duration/1000000);
        return NULL;
    }
    // builds bitmap index in speculative mode (Step 1 - 3)
    static void* buildStringMaskBitmap(void* arg) {
        int thread_id = (int)(*((int*)arg));
        // bind CPU
        cpu_set_t mask;
        cpu_set_t get;
        CPU_ZERO(&mask);
        CPU_SET(thread_id, &mask);
        if(pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask)<0)
            cout<<"CPU binding failed for thread "<<thread_id<<endl;
        //cout<<thread_id<<"th thread starts building string mask bitmap."<<endl; 
        mParallelBitmap->mBitmaps[thread_id]->buildStringMaskBitmap();
        //cout<<thread_id<<"th thread finishes building string mask bitmap."<<endl;
        return NULL;
    }
    // finish the last two steps to finish strucutral index construction
    static void* buildLeveledBitmap(void* arg) {
        int thread_id = (int)(*((int*)arg));
        // bind CPU
        cpu_set_t mask;
        cpu_set_t get;
        CPU_ZERO(&mask);
        CPU_SET(thread_id, &mask);
        if(pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask)<0)
            cout<<"CPU binding failed for thread "<<thread_id<<endl;
        mParallelBitmap->mBitmaps[thread_id]->buildLeveledBitmap();
        //cout<<thread_id<<"th thread finishes building leveled bitmap."<<endl;
        return NULL;
    }
};
#endif
