#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clock.h"
#include "memory.h"
#include "register.h"

#define CLOCK_DELAY 0
#define MEMORY_SIZE (uint16_t) (64*1024)-1

int main(int argc, char *argv[]) {

  if (register_init() != 0) {
    fprintf(stderr, "Cannot initialise registers\n");
    return -1;
  };

  if (clock_init(CLOCK_DELAY) != 0) {
    fprintf(stderr, "Cannot initialise clock\n");
    return -1;
  }

  if (memory_init(MEMORY_SIZE) != 0) {
    fprintf(stderr, "Cannot intialize memory\n");
    return -1;
  }

  fprintf( stdout, "Memory size: %04x\n", memory_get_size());
  // Set the stack pointer to the top of memory
  register_set_sp( memory_get_size() );

  while (1) {
    if (!clock_available())
      break;
    register_display();
    clock_delay();
    register_inc_pc();
  }

  clock_destroy();
  register_destroy();

  return 0;
}