//
//  main_graphicsHandler.c
//  CSERW
//

#include "main_graphicsHandler.h"
#include "graphicsRenderer.h"
#include "graphicsShaders.h"
#include "../main_header.h"
#include "../controls/main_controls.h"
#include "../math/mathMatrix.h"
#include <ncurses.h>

//const uint32_t w_height = 800;
//const uint32_t w_width = 600;
static GLFWwindow * window;

void GLFW_error_callback(int ret, const char* error) {
    fprintf(stderr, "GLFW ERROR: %s \nCode: %i", error, ret);
}

unsigned int initGLFW() {
    glewExperimental = GL_TRUE;
    
    if(!glfwInit()){ fprintf(stderr, "Failed to init GLFW\n"); return 0; };
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    window = glfwCreateWindow(w_width, w_height, "Space Engineers", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    glfwSetErrorCallback(GLFW_error_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    
    
    if(glewInit()){
        fprintf(stderr, "GLEW failed...?");
        return 0;
    }
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    return GL_TRUE;
}

GLFWwindow *getWindowAddr() {
    return window;
}

void main_graphicsLoop() {
//    drawObject dO = uploadObject("cube.obj");
    drawObject dO = uploadObject2("uv_sphere.obj");
    glfwSetWindowUserPointer(window, &dO);
    float currentTime=0, lastTime=0;
    glfwSetCursorPos(window, CENTER_LENGTH, CENTER_HEIGHT);
    
    GLint posAttrib = glGetAttribLocation(dO.programID, "position");
    GLuint uvsAttrib = glGetAttribLocation(dO.programID, "UV_coordinates");
    
    /*  SHADER LOADING  */
        
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dO.vto);
    glUniform1d(glGetAttribLocation(dO.programID, "sampler"), 0);
    
    glEnableVertexAttribArray(posAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, dO.vbo);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(uvsAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, dO.uvo);
    glVertexAttribPointer(uvsAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    /*  --------------  */
    
    while(!glfwWindowShouldClose(window)){
        currentTime = glfwGetTime();
        dO.deltaTime = currentTime - lastTime;
        
        glClearColor(0.196078f, 0.6f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDrawElements(GL_TRIANGLES, dO.icount, GL_UNSIGNED_INT, (void*)0);
        
        masterControls();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        lastTime = currentTime;
    }
    
    glDeleteProgram(dO.programID);
    glDeleteBuffers(1, &dO.uvo);
    glDeleteBuffers(1, &dO.nvo);
    glDeleteBuffers(1, &dO.vbo);
    glDeleteVertexArrays(1, &dO.vao);
}

void destroyGLFW(){
    glfwTerminate();
}

void main_Graphics() {
    if(!initGLFW()){ return; };
    
    main_graphicsLoop();
    destroyGLFW();
}