//
//  graphicsRenderer.c
//  CSERW
//

#include "graphicsRenderer.h"
#include "../main_header.h"
#include "graphicsShaders.h"
#include "file_helper.h"
#include "../math/mathMatrix.h"
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

typedef struct {
    tinyobj_attrib_t attr;
    tinyobj_shape_t *shapes;
    size_t nshapes;
    tinyobj_material_t *mout;
    size_t nmat;
}shapeObj;

const mat4x4 matinit_1 = (mat4x4)  {1,0,0,0,
                                    0,1,0,0,
                                    0,0,1,0,
                                    0,0,0,1};

const GLfloat vertices[] = {
    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
    -0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom-left
};

const GLuint elements[] = {
    0,1,2,
    2,3,0
};

void parseObj(const char *objFilename,
              const char *mtlFilename,
              shapeObj *so) {
    fileObj obj = loadFileBuffer(objFilename);
    fileObj mtl = loadFileBuffer(mtlFilename);
    
    if(!tinyobj_parse_obj(&so->attr,
                          &so->shapes,
                          &so->nshapes,
                          &so->mout,
                          &so->nmat,
                          (char*)obj.buffer,
                          obj.buflen, 0))
    {
        printf("Could not fully parse objects\n");
    }
    deleteFileBuffer(&obj);
    deleteFileBuffer(&mtl);
}

void destroytinyobj(shapeObj *so) {
    tinyobj_attrib_free(&so->attr);
    tinyobj_shapes_free(so->shapes, so->nshapes);
    tinyobj_materials_free(so->mout, so->nmat);
}

void _3D(drawObject *dO) {
    mat4x4 projection = mat_perspective(deg_to_rads(45.f), w_width/w_height, 1.f, 10.f);
    vec4 rot = {0,0,0,0};
    mat4x4 view = mat_lookat((vec4){1.2f,1.2f,1.2f,0}, rot, (vec4){0,0,1,0});
    mat4x4 model = matinit_1;
    mat4x4 MVP = {0},MVP0 = {0};
    mat_multiply(&projection, &view, &MVP);
    mat_multiply(&MVP, &model, &MVP0);

    glUniformMatrix4fv(glGetUniformLocation(dO->programID,"MVP"),1, GL_FALSE, &MVP0.main.iter[0]);
}

drawObject uploadObject() {
    // Parse an OBJ file
    shapeObj so;
    parseObj("cornell_box.obj", "cornell_box.mtl", &so);
    tinyobj_shape_t sa[so.nshapes];
    for(int i = 0; i < so.nshapes; ++i) {
        sa[i] = so.shapes[i];
    }
    // Bind a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // copy vertex buffer data
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, so.attr.num_vertices*sizeof(GL_FLOAT), so.attr.vertices, GL_STATIC_DRAW);
    
    // copy index buffer data
//    GLuint ebo;
//    glGenBuffers(1,&ebo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    
    // get attribute position contrast to using the explicit shader location syntax
    GLuint program = compileShaders("vShader.vert", "fShader.frag");
    GLint posAttrib = glGetAttribLocation(program, "position");
    //GLint colAttrib = glGetAttribLocation(program, "color");
    
    // set shader attributes
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);
    
    destroytinyobj(&so);
    drawObject DO = {program, vbo, vao, 0, so.attr.num_vertices};
    
    _3D(&DO);
    
    return DO;
}