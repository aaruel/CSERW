//
//  custom_obj_loader.c
//  CSERW
//

#include "custom_obj_loader.h"
#include <stdio.h>

fileObj parseObjFileVerticies(const char * filename) {
    fileObj fo;
    fo = loadFileBuffer(filename);
    for (int i = 0; i < fo.buflen; ++i) {
        if (fo.buffer[i] == 'v') {
            float* vertex[3];
            const char* t = (const char*)&fo.buffer[i];
            sscanf(t, "%*s %f %f %f", vertex[0], vertex[1], vertex[2]);
            printf("%f %f %f\n", *vertex[0], *vertex[1], *vertex[2]);
        }
    }
    
    return fo;
}