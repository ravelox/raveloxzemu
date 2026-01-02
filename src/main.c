#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "register.h"

int main( int argc, char *argv[] )
{

    register_init();
    
    register_display();

    register_destroy();

    return 0;

}