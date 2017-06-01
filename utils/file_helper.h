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
#include "../graphics/tinyobj_loader_c.h"

typedef struct{
    FILE *fp;
    unsigned char *buffer;
    long buflen;
} fileObj;

typedef struct{
    tinyobj_attrib_t *attrib;
    tinyobj_shape_t **shapes;
    size_t *num_shapes;
    tinyobj_material_t **materials;
    size_t *num_materials;
    const char *buf;
    size_t len;
} compositeWavefrontObj;

fileObj loadFileBuffer(const char *fileLocation);
void deleteFileBuffer(fileObj *fo);

#endif /* defined(__CSERW__file_helper__) */
