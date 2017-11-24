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
    fo.buflen = ftell(fo.fp)+1;
    rewind(fo.fp);
    
    // allocate buffer=filelength+null terminator
    fo.buffer = malloc(sizeof(char) * fo.buflen);
    if (!fo.buffer) { return fo; }
    
    const int result = (const int)fread(fo.buffer, 1, fo.buflen, fo.fp) + 1;
    if (result != fo.buflen) { return fo; };
    
    return fo;
}

void deleteFileBuffer(fileObj *fo) {
    fclose(fo->fp);
}