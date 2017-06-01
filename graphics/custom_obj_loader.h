//
//  custom_obj_loader.h
//  CSERW
//

#ifndef __CSERW__custom_obj_loader__
#define __CSERW__custom_obj_loader__

#include "../utils/file_helper.h"

typedef struct {
    float * base;
    unsigned int size;
} vertexObject;

void parseObjFileVerticies(const char * filename, compositeWavefrontObj * com);

#endif /* defined(__CSERW__custom_obj_loader__) */
