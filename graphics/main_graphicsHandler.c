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

//const uint32_t w_height = 800;
//const uint32_t w_width = 600;
static GLFWwindow * window;

void GLFW_error_callback(int ret, const char* error) {
    fprintf(stderr, "GLFW ERROR: %s \nCode: %i", error, ret);
}

unsigned int initGLFW() {
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
    
    glewExperimental = GL_TRUE;
    if(glewInit()){
        fprintf(stderr, "GLEW failed...?");
        return 0;
    }
    
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    return GL_TRUE;
}

GLFWwindow *getWindowAddr() {
    return window;
}

void main_graphicsLoop() {
    drawObject dO = uploadObject();
    glfwSetWindowUserPointer(window, &dO);
    float currentTime=0, lastTime=0;
    glfwSetCursorPos(window, CENTER_LENGTH, CENTER_HEIGHT);
    while(!glfwWindowShouldClose(window)){
        currentTime = glfwGetTime();
        dO.deltaTime = currentTime - lastTime;
        
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, dO.vcount);
        
        masterControls();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        lastTime = currentTime;
    }
    glDeleteProgram(dO.programID);
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