//
//  graphicsShaders.c
//  CSERW
//

#include "graphicsShaders.h"
#include "../main_header.h"
#include <stdio.h>
#include <stdlib.h>

GLuint compileShaders(const char *vertexShaderSource,
                      const char *fragmtShaderSource) {
    FILE *vp, *fp;
    vp = fopen(vertexShaderSource, "rb");
    fp = fopen(fragmtShaderSource, "rb");
    
    if(vp == NULL || fp == NULL) {
        printf("Could not locate shader files\n");
        exit(0);
    }
    
    // load file into buffer
    fseek(vp, 0, SEEK_END);
    long vlength = ftell(vp);
    char *vbuffer = malloc(vlength+1);
    fseek(vp, 0, SEEK_SET);
    fread(vbuffer, vlength, 1, vp);
    vbuffer[vlength] = '\0';
    
    fseek(fp, 0, SEEK_END);
    long flength = ftell(fp);
    char *fbuffer = malloc(flength+1);
    fseek(fp, 0, SEEK_SET);
    fread(fbuffer, flength, 1, fp);
    fbuffer[flength] = '\0';
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmtShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertexShader, 1, (const GLchar * const *)&vbuffer, NULL);
    glShaderSource(fragmtShader, 1, (const GLchar * const *)&fbuffer, NULL);
    free(vbuffer); free(fbuffer);
    glCompileShader(vertexShader);
    glCompileShader(fragmtShader);
    GLint status0, status1;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status0);
    if(!status0) {
        char buffer[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
        printf("%s\n", buffer);
    }
    glGetShaderiv(fragmtShader, GL_COMPILE_STATUS, &status1);
    if(!status1) {
        char buffer[512];
        glGetShaderInfoLog(fragmtShader, 512, NULL, buffer);
        printf("%s\n", buffer);
    } else {
        printf("Shaders compiled successfully\n");
    }
    
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmtShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    
    return shaderProgram;
}