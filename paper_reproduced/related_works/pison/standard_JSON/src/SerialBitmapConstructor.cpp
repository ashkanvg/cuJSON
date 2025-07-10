#include "SerialBitmapConstructor.h"
#include <string.h>
#include <sys/time.h>
#include <sys/file.h>

SerialBitmap* SerialBitmapConstructor::construct(Record* record, int level_num) {
    char* record_text = NULL;
    long length = 0;
    if (record->rec_start_pos > 0) record_text = record->text + record->rec_start_pos;
    else record_text = record->text;
    if (record->rec_length > 0) length = record->rec_length;
    else length = strlen(record->text);
    SerialBitmap* bitmap = new SerialBitmap(record_text, level_num);
    bitmap->setRecordLength(length);
    bitmap->indexConstruction();
    return bitmap;
}
