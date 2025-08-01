#include "ParallelBitmapConstructor.h"

ParallelBitmap* ParallelBitmapConstructor::mParallelBitmap = NULL;
pthread_t ParallelBitmapConstructor::mThreads[MAX_THREAD]; 
int ParallelBitmapConstructor::mThreadArgs[MAX_THREAD];

ParallelBitmap* ParallelBitmapConstructor::construct(Record* record, int thread_num, int level_num) {
    char* record_text = NULL;
    long length = 0;
    if (record->rec_start_pos > 0) record_text = record->text + record->rec_start_pos;
    else record_text = record->text;
    if (record->rec_length > 0) length = record->rec_length;
    else length = strlen(record->text);
    mParallelBitmap = new ParallelBitmap(record_text, length, thread_num, level_num);
    int mode = mParallelBitmap->parallelMode();
    if (mode == NONSPECULATIVE) {
        for (int i = 0; i < thread_num; ++i) {
            mThreadArgs[i] = i;
            int rc=pthread_create(&mThreads[i], NULL, &ParallelBitmapConstructor::nonSpecIndexConstruction, &mThreadArgs[i]);
            if (rc)
            {
                cout<<"Thread Error; return code is "<<rc<<endl;
                return NULL;
            }
        }
        for(int i = 0; i < thread_num; ++i)
        {
            int rc = pthread_join(mThreads[i], NULL);
            if (rc)
            {
                cout<<"Thread Error; return code is "<<rc<<endl;
                return NULL;
            }
        }
        mParallelBitmap->mergeBitmaps();
    } else {
        for (int i = 0; i < thread_num; ++i) {
            mThreadArgs[i] = i;
            int rc=pthread_create(&mThreads[i], NULL, &ParallelBitmapConstructor::buildStringMaskBitmap, &mThreadArgs[i]);
            if (rc)
            {
                cout<<"Thread Error; return code is "<<rc<<endl;
                return NULL;
            }
        }
        for(int i = 0; i < thread_num; ++i)
        {
            int rc = pthread_join(mThreads[i], NULL);
            if (rc)
            {
                cout<<"Thread Error; return code is "<<rc<<endl;
                return NULL;
            }
        }
        mParallelBitmap->rectifyStringMaskBitmaps();
        for (int i = 0; i < thread_num; ++i) {
            mThreadArgs[i] = i;
            int rc=pthread_create(&mThreads[i], NULL, &ParallelBitmapConstructor::buildLeveledBitmap, &mThreadArgs[i]);
            if (rc)
            {
                cout<<"Thread Error; return code is "<<rc<<endl;
                return NULL;
            }
        }
        for(int i = 0; i < thread_num; ++i)
        {
            int rc = pthread_join(mThreads[i], NULL);
            if (rc)
            {
                cout<<"Thread Error; return code is "<<rc<<endl;
                return NULL;
            }
        }
        mParallelBitmap->mergeBitmaps();
    }
    return mParallelBitmap;
}
