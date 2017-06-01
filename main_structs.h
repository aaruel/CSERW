//
//  main_structs.h
//  CSERW
//

#ifndef CSERW_main_structs_h
#define CSERW_main_structs_h

typedef struct{
    unsigned int programID;
    unsigned int vbo;
    unsigned int vao;
    unsigned int uvo;
    unsigned int nvo;
    unsigned int ebo;
    unsigned int vto;
    unsigned int vcount;
    unsigned int icount;
    float deltaTime;
} drawObject;

#endif
