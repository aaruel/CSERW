//
//  graphicsRenderer.c
//  CSERW
//

#include "graphicsRenderer.h"
#include "../main_header.h"
#include "graphicsShaders.h"
#include "lodepng.h"
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "file_helper.h"
#include "../math/mathMatrix.h"
#include "custom_obj_loader.h"

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

void destroytinyobj(compositeWavefrontObj *so) {
    tinyobj_attrib_free(so->attrib);
    tinyobj_shapes_free(*so->shapes, *so->num_shapes);
    tinyobj_materials_free(*so->materials, *so->num_materials);
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

drawObject uploadObject(const char * fileLocation) {
    // Parse an OBJ file
    compositeWavefrontObj com;
    parseObjFileVerticies(fileLocation, &com);
    
    // vertices * 3 = #elements
    com.attrib->num_vertices  *= 3;
    com.attrib->num_texcoords *= 2;
    com.attrib->num_normals   *= 3;
    
    GLuint *indicies = malloc(sizeof(GLuint) * com.attrib->num_faces);
    GLuint *normalin = malloc(sizeof(GLuint) * com.attrib->num_faces);
    GLuint *textures = malloc(sizeof(GLuint) * com.attrib->num_faces);
    
    // get indicies
    size_t face_offset = 0;
    for (int i = 0; i < com.attrib->num_face_num_verts; ++i) {
        assert(com.attrib->face_num_verts[i] % 3 == 0);
        
        indicies[face_offset + 0] = (com.attrib->faces)[face_offset + 0].v_idx;
        indicies[face_offset + 1] = (com.attrib->faces)[face_offset + 1].v_idx;
        indicies[face_offset + 2] = (com.attrib->faces)[face_offset + 2].v_idx;
        
        normalin[face_offset + 0] = (com.attrib->faces)[face_offset + 0].vn_idx;
        normalin[face_offset + 1] = (com.attrib->faces)[face_offset + 1].vn_idx;
        normalin[face_offset + 2] = (com.attrib->faces)[face_offset + 2].vn_idx;
        
        textures[face_offset + 0] = (com.attrib->faces)[face_offset + 0].vt_idx;
        textures[face_offset + 1] = (com.attrib->faces)[face_offset + 1].vt_idx;
        textures[face_offset + 2] = (com.attrib->faces)[face_offset + 2].vt_idx;
        
        face_offset += (size_t)com.attrib->face_num_verts[i];
    }
    
    // Bind a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // copy vertex buffer data
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, com.attrib->num_vertices*sizeof(GLfloat), com.attrib->vertices, GL_STATIC_DRAW);
    
    // copy UV data
    GLuint uvo;
    glGenBuffers(1, &uvo);
    glBindBuffer(GL_ARRAY_BUFFER, uvo);
    glBufferData(GL_ARRAY_BUFFER, com.attrib->num_texcoords*sizeof(GLfloat), com.attrib->texcoords, GL_STATIC_DRAW);
    
    // copy normal data
    GLuint nvo;
    glGenBuffers(1, &nvo);
    glBindBuffer(GL_ARRAY_BUFFER, nvo);
    glBufferData(GL_ARRAY_BUFFER, com.attrib->num_normals*sizeof(GLfloat), com.attrib->normals, GL_STATIC_DRAW);
    
    // copy vertex indicies data
    GLuint vibo;
    glGenBuffers(1, &vibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, com.attrib->num_faces*sizeof(GLuint), indicies, GL_STATIC_DRAW);
    
    GLuint vto;
    glGenTextures(1, &vto);
    glBindTexture(GL_TEXTURE_2D, vto);
    
    {
        char * filename = (*com.materials)->diffuse_texname;
        unsigned char * image;
        unsigned int height = 512;
        unsigned int width  = 256;
        if (1) {
            filename[strlen(filename)-1] = '\0';
            unsigned int error = lodepng_decode32_file(&image, &width, &height, filename);
            if (!error) {
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = no smoothing
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            }
            else {
                printf("error %u: %s\n", error, lodepng_error_text(error));
            }
            free(image);
        }
    }
    
    // get attribute position contrast to using the explicit shader location syntax
    GLuint program = compileShaders("vShader.vert", "fShader.frag");
    drawObject DO = {
        program,
        vbo,
        vao,
        uvo,
        nvo,
        vibo,
        vto,
        com.attrib->num_vertices,
        com.attrib->num_faces
    };
    
    // initialize shader attribs
    _3D(&DO);
    
    destroytinyobj(&com);
    free(indicies);
    free(normalin);
    free(textures);
    return DO;
}