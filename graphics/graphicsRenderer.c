//
//  graphicsRenderer.c
//  CSERW
//

#include "graphicsRenderer.h"
#include "../main_header.h"
#include "graphicsShaders.h"
#include "lodepng.h"
#include "file_helper.h"
#include "../math/mathMatrix.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

const mat4x4 matinit_1 = (mat4x4)  {1,0,0,0,
                                    0,1,0,0,
                                    0,0,1,0,
                                    0,0,0,1};

/// Private

void sendDataBufferToGPU(GLenum target, GLuint * buffer, GLsizeiptr size, const GLvoid * data, GLenum usage) {
    glGenBuffers(1, buffer);
    glBindBuffer(target, *buffer);
    glBufferData(target, size, data, usage);
}

void sendGLBufferToShader(GLuint program, const char * variableName, GLuint buffer, GLuint size) {
    GLint attrib = glGetAttribLocation(program, variableName);
    glEnableVertexAttribArray(attrib);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(attrib, size, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void loadTextureObject(const char * filename, GLuint * buffer, GLuint program) {
    unsigned char * image, * png = 0;
    unsigned int height = 0, width = 0;
    size_t pngsize = 0;
    
    glGenTextures(1, buffer);
    glBindTexture(GL_TEXTURE_2D, *buffer);
    
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
    
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *buffer);
    glUniform1d(glGetAttribLocation(program, "sampler"), 0);
    
    free(image);
}

/// Public

void _3D(drawObject *dO) {
    mat4x4 projection = mat_perspective(
        deg_to_rads(45.f),
        w_width/w_height,
        1.f,
        10.f
    );
    
    mat4x4 view = mat_lookat(
        (vec4){1.2f,1.2f,1.2f,0},
        (vec4){0,0,0,0},
        (vec4){0,0,1,0}
    );
    
    mat4x4 model = matinit_1;
    mat4x4 MVP = {0}, MVP0 = {0};
    mat_multiply(&projection, &view, &MVP);
    mat_multiply(&MVP, &model, &MVP0);

    glUniformMatrix4fv(
        glGetUniformLocation(dO->programID,"MVP"),
        1,
        GL_FALSE,
        &MVP0.main.iter[0]
    );
}

drawObject uploadObject(const char * fileLocation) {
    const struct aiScene * importedObject = aiImportFile(fileLocation, aiProcess_Triangulate);
    struct aiMesh * mesh = importedObject->mMeshes[0];
    const unsigned int numVertices = mesh->mNumVertices;
    const unsigned int numElements = mesh->mNumVertices * 3;
    const unsigned int numFaces = mesh->mNumFaces;

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
    sendDataBufferToGPU(
        GL_ARRAY_BUFFER,
        &drawobject.vbo,
        numVertices * sizeof(Vertex),
        vertices,
        GL_STATIC_DRAW
    );
    // send vertices to shader
    sendGLBufferToShader(
        drawobject.programID,
        "position",
        drawobject.vbo,
        3
    );
    
    // copy UV data
    sendDataBufferToGPU(
        GL_ARRAY_BUFFER,
        &drawobject.uvo,
        numVertices * sizeof(Texture),
        texcoords,
        GL_STATIC_DRAW
    );
    // send UV coordinates to shader
    sendGLBufferToShader(
        drawobject.programID,
        "UV_coordinates",
        drawobject.uvo,
        2
    );
    
    // copy normal data
    sendDataBufferToGPU(
        GL_ARRAY_BUFFER,
        &drawobject.nvo,
        numVertices * sizeof(Normal),
        normals,
        GL_STATIC_DRAW
    );
    
    // send normals to shader
    sendGLBufferToShader(
        drawobject.programID,
        "normals",
        drawobject.nvo,
        3
    );
    
    // copy vertex indicies data
    GLuint vibo;
    sendDataBufferToGPU(
        GL_ELEMENT_ARRAY_BUFFER,
        &vibo,
        numFaces * sizeof(Index),
        indices,
        GL_STATIC_DRAW
    );
    
    loadTextureObject(
        "terran.png",
        &drawobject.vto,
        drawobject.programID
    );
    
    free(vertices);
    free(texcoords);
    free(normals);
    free(indices);
    
    return drawobject;
}
