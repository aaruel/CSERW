//
//  main_header.h
//  CSERW
//

#ifndef CSERW_main_header_h
#define CSERW_main_header_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#define w_height 600.0
#define w_width 800.0
#define CENTER_HEIGHT (float)(w_height /2.f)
#define CENTER_LENGTH (float)(w_width /2.f)

// callback functions
GLFWwindow *getWindowAddr();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void destructor();

#endif
