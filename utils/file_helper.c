//
//  file_helper.c
//  CSERW
//

#include "file_helper.h"
#include <stdlib.h>

fileObj loadFileBuffer(const char* fileLocation) {
    fileObj fo;
    fo.fp = fopen(fileLocation, "rb");
    if(fo.fp == NULL) {
        printf("Could not locate file at %s", fileLocation);
        return fo;
    }
    // figure out file length
    fseek(fo.fp, 0, SEEK_END);
    fo.buflen = ftell(fo.fp);
    // allocate buffer=filelength+null terminator
    fseek(fo.fp, fo.buflen, SEEK_SET);
    // read data into buffer
    fread(fo.buffer, fo.buflen, 1, fo.fp);
    fo.buffer = fo.fp->_p;
    return fo;
}

void deleteFileBuffer(fileObj *fo) {
    fclose(fo->fp);
}