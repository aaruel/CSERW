//
//  main_controls.c
//  CSERW
//
//  Created by Aaron Ruel on 12/20/16.
//  Copyright (c) 2016 AAR. All rights reserved.
//

#include "main_controls.h"
#include "../main_header.h"
#include "../math/mathMatrix.h"
#include "../main_structs.h"
#include <math.h>

typedef struct{
    vec4 position;
    vec4 direction;
    vec4 right;
    vec4 up;
    vec4 rot;
    float cameraSpeed;
    float speedMulti;
    float mouseSpeed;
    float FoV;
    float aspectRatio;
    float horizontalAngle;
    float verticalAngle;
    float cursorX;
    float cursorY;
    mat4x4 projection;
    mat4x4 view;
    mat4x4 model;
} positionAttrs;

#define matinit_2  (mat4x4)  {1,0,0,0,\
                              0,1,0,0,\
                              0,0,1,0,\
                              0,0,0,1}

static positionAttrs pa = {
    .position =  {1.2f,1.2f,1.2f,0.f},
    .direction = {0.f,-1.2f,-1.2f,0.f},
    .right =     {1.f, 0.f, 0.f, 0.f},
    .up =        {0.f, 1.f, 0.f, 0.f},
    .rot =       {0.f, 0.f, 0.f, 0.f},
    .cameraSpeed    = 3.f,
    .speedMulti     = 1.f,
    .mouseSpeed     = 0.005f,
    .FoV            = 45.f,
    .model          = matinit_2,
    .horizontalAngle= 0.f,
    .verticalAngle  = 0.f,
    .cursorX        = 0,
    .cursorY        = 0
};

static uint8_t pkeys[1024]={0};
static uint8_t pkeystates[1024]={0};
drawObject *dO;

void closeWindow() {
    if(pkeys[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(getWindowAddr(), GL_TRUE);
    }
}

void sendData() {
    GLuint MVP_ID = glGetUniformLocation(dO->programID,"MVP");
    
    mat4x4 MVP = {0},MVP0 = {0};
    mat_multiply(&pa.projection, &pa.view, &MVP);
    mat_multiply(&MVP, &pa.model, &MVP0);
    
    glUniformMatrix4fv(MVP_ID, 1, GL_FALSE, &MVP0.main.iter[0]);
}

void cursorCompute() {
    glfwSetCursorPos(getWindowAddr(), CENTER_LENGTH, CENTER_HEIGHT);
    pa.horizontalAngle += pa.mouseSpeed * (CENTER_LENGTH - pa.cursorX);
    pa.verticalAngle   += pa.mouseSpeed * (CENTER_HEIGHT - pa.cursorY);
    
    // vertical limiter
    if(pa.verticalAngle > 0.95) {
        pa.verticalAngle = 0.95;
    } else if (pa.verticalAngle < -0.95) {
        pa.verticalAngle = -0.95;
    }
    
    pa.direction = (vec4){(float)(cos(pa.verticalAngle)*sin(pa.horizontalAngle)),
                          (float)(sin(pa.verticalAngle)),
                          (float)(cos(pa.verticalAngle)*cos(pa.horizontalAngle)), 0.f};
    
    pa.right = (vec4)  {sin(pa.horizontalAngle-3.14f/2.0f),
        0,
        cos(pa.horizontalAngle-3.14f/2.0f)};
    
    pa.up = vec_cross(&pa.right, &pa.direction);
}

void computeKeys() {
    dO = glfwGetWindowUserPointer(getWindowAddr());
    if(pkeys[GLFW_KEY_W]) {
        pa.position = vec_add(pa.position, vec_mult1(pa.direction, pa.cameraSpeed * dO->deltaTime * pa.speedMulti));
    }
    if(pkeys[GLFW_KEY_S]) {
        pa.position = vec_subtract(pa.position, vec_mult1(pa.direction, pa.cameraSpeed * dO->deltaTime * pa.speedMulti));
    }
    if(pkeys[GLFW_KEY_D]) {
        pa.position = vec_add(pa.position, vec_mult1(pa.right, pa.cameraSpeed * dO->deltaTime * pa.speedMulti));
    }
    if(pkeys[GLFW_KEY_A]) {
        pa.position = vec_subtract(pa.position, vec_mult1(pa.right, pa.cameraSpeed * dO->deltaTime * pa.speedMulti));
    }
    if(pkeys[GLFW_KEY_SPACE]) {
        pa.position = vec_add(pa.position, vec_mult1(pa.up, pa.cameraSpeed * dO->deltaTime * pa.speedMulti));
    }
    if(pkeys[GLFW_KEY_C]) {
        pa.position = vec_subtract(pa.position, vec_mult1(pa.up, pa.cameraSpeed * dO->deltaTime * pa.speedMulti));
    }
    if(pkeys[GLFW_KEY_I] && pkeystates[GLFW_KEY_I]==GLFW_REPEAT) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    pa.projection = mat_perspective(deg_to_rads(45.f), w_width/w_height, .1f, 100.f);
    pa.view       = mat_lookat(pa.position, vec_add(pa.position, pa.direction), pa.up);
}

// Callable functions

void masterControls() {
    computeKeys();
    closeWindow();
    sendData();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(action == GLFW_PRESS) {
        pkeys[key] = GL_TRUE;
    } else if (action == GLFW_RELEASE) {
        pkeys[key] = GL_FALSE;
    }
    pkeystates[key] = action;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // temporal cursor dampener
    if(xpos > CENTER_LENGTH*0.5f && xpos < CENTER_LENGTH*1.5f) {
        pa.cursorX = xpos;
    }
    if(ypos > CENTER_HEIGHT*0.5f && ypos < CENTER_LENGTH*1.5f) {
        pa.cursorY = ypos;
    }
    cursorCompute();
}