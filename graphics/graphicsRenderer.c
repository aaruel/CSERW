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
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

typedef struct {
    float x;
    float y;
} Texture;

typedef struct {
    float x;
    float y;
    float z;
} Vertex;

typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int z;
} Index;

typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int z;
} Normal;

void printTexture(Texture t) {
    printf("(%f, %f)\n", t.x, t.y);
}

drawObject uploadObject2(const char * fileLocation) {
    const struct aiScene * importedObject = aiImportFile(fileLocation, aiProcess_Triangulate);
    struct aiMesh * mesh = importedObject->mMeshes[0];
    unsigned int numVertices = mesh->mNumVertices;
    unsigned int numElements = mesh->mNumVertices * 3;
    unsigned int numFaces = mesh->mNumFaces;

    Vertex * vertices = malloc(sizeof(Vertex) * numVertices);
    Texture * texcoords = malloc(sizeof(Texture) * numVertices);
    Normal * normals = malloc(sizeof(Normal) * numVertices);
    Index * indices = malloc(sizeof(Index) * numFaces);
    
    for (int i = 0; i < numVertices; ++i) {
        vertices[i] = (Vertex) {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };
        
        if (mesh->mTextureCoords[0]) {
            texcoords[i] = (Texture) {
                1.f - mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        }
        
        if (mesh->mNormals) {
            normals[i] = (Normal) {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            };
        }
    }
    
    for (int i = 0; i < numFaces; ++i) {
        indices[i] = (Index) {
            mesh->mFaces[i].mIndices[0],
            mesh->mFaces[i].mIndices[1],
            mesh->mFaces[i].mIndices[2]
        };
    }
    
    drawObject drawobject = {
        .programID = compileShaders("vShader.vert", "fShader.frag"),
        .icount = numElements,
        .vcount = numVertices
    };
    
    // Bind a vertex array object
    glGenVertexArrays(1, &drawobject.vao);
    glBindVertexArray(drawobject.vao);
    
    // copy vertex buffer data
    glGenBuffers(1, &drawobject.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, drawobject.vbo);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), vertices, GL_STATIC_DRAW);
    
    // copy UV data
    glGenBuffers(1, &drawobject.uvo);
    glBindBuffer(GL_ARRAY_BUFFER, drawobject.uvo);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Texture), texcoords, GL_STATIC_DRAW);
    
    // copy normal data
    glGenBuffers(1, &drawobject.nvo);
    glBindBuffer(GL_ARRAY_BUFFER, drawobject.nvo);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Normal), normals, GL_STATIC_DRAW);
    
    // copy vertex indicies data
    GLuint vibo;
    glGenBuffers(1, &vibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFaces * sizeof(Index), indices, GL_STATIC_DRAW);

    glGenTextures(1, &drawobject.vto);
    glBindTexture(GL_TEXTURE_2D, drawobject.vto);
    
    char * filename = "terran.png";
    unsigned char * image, * png = 0;
    unsigned int height = 0, width = 0;
    size_t pngsize = 0;
    lodepng_load_file(&png, &pngsize, filename);
    unsigned int error = lodepng_decode32(&image, &width, &height, png, pngsize);
    if (!error) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    }
    else {
        printf("error %u: %s\n", error, lodepng_error_text(error));
    }
    
    free(image);
    free(vertices);
    free(texcoords);
    free(normals);
    free(indices);
    
    return drawobject;
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
    Texture *textures= malloc(sizeof(Texture) * com.attrib->num_faces);
    
    // get indicies
    
// So :
// # is a comment, just like // in C++
// usemtl and mtllib describe the look of the model. We won’t use this in this tutorial.
// v is a vertex
// vt is the texture coordinate of one vertex
// vn is the normal of one vertex
// f is a face
// v, vt and vn are simple to understand. f is more tricky. So, for f 8/11/7 7/12/7 6/10/7 :
//        
// 8/11/7 describes the first vertex of the triangle
// 7/12/7 describes the second vertex of the triangle
// 6/10/7 describes the third vertex of the triangle (duh)
// For the first vertex, 8 says which vertex to use. So in this case, -1.000000 1.000000 -1.000000 (index start to 1, not to 0 like in C++)
// 11 says which texture coordinate to use. So in this case, 0.748355 0.998230
// 7 says which normal to use. So in this case, 0.000000 1.000000 -0.000000
    
    size_t face_offset = 0;
    for (int i = 0; i < com.attrib->num_face_num_verts; ++i) {
        assert(com.attrib->face_num_verts[i] % 3 == 0);
        
        indicies[face_offset + 0] = (com.attrib->faces)[face_offset + 0].v_idx;
        indicies[face_offset + 1] = (com.attrib->faces)[face_offset + 1].v_idx;
        indicies[face_offset + 2] = (com.attrib->faces)[face_offset + 2].v_idx;
        
        normalin[face_offset + 0] = (com.attrib->faces)[face_offset + 0].vn_idx;
        normalin[face_offset + 1] = (com.attrib->faces)[face_offset + 1].vn_idx;
        normalin[face_offset + 2] = (com.attrib->faces)[face_offset + 2].vn_idx;
        
        const unsigned int vt_idx1 = (com.attrib->faces)[face_offset + 0].vt_idx * 2;
        const unsigned int vt_idx2 = (com.attrib->faces)[face_offset + 1].vt_idx * 2;
        const unsigned int vt_idx3 = (com.attrib->faces)[face_offset + 2].vt_idx * 2;
        
        textures[face_offset + 0] = (Texture){
            com.attrib->texcoords[ vt_idx1 + 0 ],
            com.attrib->texcoords[ vt_idx1 + 1 ]
        };
        
        textures[face_offset + 1] = (Texture){
            com.attrib->texcoords[ vt_idx2 + 0 ],
            com.attrib->texcoords[ vt_idx2 + 1 ]
        };
        
        textures[face_offset + 2] = (Texture){
            com.attrib->texcoords[ vt_idx3 + 0 ],
            com.attrib->texcoords[ vt_idx3 + 1 ]
        };
        
        printf("%u", 1+vt_idx1/2); printTexture(textures[face_offset + 0]);
        printf("%u", 1+vt_idx2/2); printTexture(textures[face_offset + 1]);
        printf("%u", 1+vt_idx3/2); printTexture(textures[face_offset + 2]);
        
        face_offset += (size_t)com.attrib->face_num_verts[i];
    }
    
//    for (int i = 0; i < com.attrib->num_texcoords/2; ++i) {
//        textures[i] = (Texture){
//            com.attrib->texcoords[ 2 * (com.attrib->faces)[i].vt_idx + 0 ],
//            com.attrib->texcoords[ 2 * (com.attrib->faces)[i].vt_idx + 1 ]
//        };
//        printf("%d, %d\n", 2 * (com.attrib->faces)[i].vt_idx + 0, 2 * (com.attrib->faces)[i].vt_idx + 1);
//    }
    
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
    glBufferData(GL_ARRAY_BUFFER, com.attrib->num_texcoords*sizeof(GLfloat), textures, GL_STATIC_DRAW);
    
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
    
    if (*com.materials != NULL) {
        char * filename = (*com.materials)->diffuse_texname;
        unsigned char * image, * png = 0;
        unsigned int height = 0, width = 0;
        size_t pngsize = 0;
        if (1) {
            filename[strlen(filename)-1] = '\0';
            lodepng_load_file(&png, &pngsize, filename);
            unsigned int error = lodepng_decode32(&image, &width, &height, png, pngsize);
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