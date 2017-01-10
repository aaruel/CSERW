//
//  main.c
//  CSERW
//

#include "graphics/main_graphicsHandler.h"
#include <stdio.h>

int main(int argc, const char * argv[]) {
    // start the graphics
    main_Graphics();
    
#ifndef NDEBUG
    printf("Finished execution\n");
#endif
    
    return 0;
}
