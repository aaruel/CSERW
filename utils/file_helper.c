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
    rewind(fo.fp);
    
    // allocate buffer=filelength+null terminator
    fo.buffer = malloc(sizeof(char) * fo.buflen);
    if (!fo.buffer) { printf("breh ur memory is rip"); return fo; }
    
    const int result = (const int)fread(fo.buffer, 1, fo.buflen, fo.fp);
    if (result != fo.buflen) { printf("wtf the file is shit tbh"); return fo; };
    
    return fo;
}

void deleteFileBuffer(fileObj *fo) {
    fclose(fo->fp);
}