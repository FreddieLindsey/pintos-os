#include "vm/frame.h"
#include "threads/malloc.h"


static struct frame* frame_table;

void frame_init(int num_of_frames) {

  frame_table = malloc(sizeof(struct frame)*num_of_frames);

}
