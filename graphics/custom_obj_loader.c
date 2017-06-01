//
//  custom_obj_loader.c
//  CSERW
//

#include "custom_obj_loader.h"
#include <stdlib.h>
#include <stdio.h>

void parseObjFileVerticies(const char * filename, compositeWavefrontObj * com) {
    fileObj fo;
    // file buffer
    fo = loadFileBuffer(filename);
    
    
    tinyobj_attrib_t *attrib       = malloc(sizeof(tinyobj_attrib_t));
    tinyobj_shape_t **shapes       = malloc(sizeof(tinyobj_shape_t*));
    size_t *num_shapes             = malloc(sizeof(size_t));
    tinyobj_material_t **materials = malloc(sizeof(tinyobj_material_t*));
    size_t *num_materials          = malloc(sizeof(size_t));
    const char *buf                = (const char*)fo.buffer;
    size_t len                     = fo.buflen;
    
    
    tinyobj_parse_obj(attrib, shapes, num_shapes, materials, num_materials, buf, len, TINYOBJ_FLAG_TRIANGULATE);
    
    com->attrib = attrib;
    com->shapes = shapes;
    com->num_shapes = num_shapes;
    com->materials = materials;
    com->num_materials = num_materials;
    com->buf = buf;
    com->len = len;
}

void deleteVertexObject(vertexObject *vo) {
    free(vo->base);
}