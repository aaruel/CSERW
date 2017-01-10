//
//  file_helper.h
//  CSERW
//
//  Created by Aaron Ruel on 12/25/16.
//  Copyright (c) 2016 AAR. All rights reserved.
//

#ifndef __CSERW__file_helper__
#define __CSERW__file_helper__

#include <stdio.h>

typedef struct{
    FILE *fp;
    unsigned char *buffer;
    long buflen;
} fileObj;

fileObj loadFileBuffer(const char *fileLocation);
void deleteFileBuffer(fileObj *fo);

#endif /* defined(__CSERW__file_helper__) */
