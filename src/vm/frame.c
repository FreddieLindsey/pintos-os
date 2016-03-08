#include "vm/frame.h"
#include "threads/malloc.h"


static void** frame_table;

void frame_init(int num_of_frames) {

  frame_table = malloc(sizeof(void*)*num_of_frames);

}
