#include "vm/frame.h"
#include "threads/malloc.h"
#include "userprog/syscall.h"


static struct frame **frame_table;
static size_t num_frames;

void frame_init(int num_of_frames) {

  num_frames = num_of_frames;
  frame_table = malloc(sizeof(struct frame*)*num_frames);

}

void frame_alloc(void* page) {
  unsigned f;
  int allocated = 0;
  for(f = 0; f < num_frames; f++) {
    if (frame_table[f] == NULL) {
      allocated = 1;
      struct frame *frame = malloc(sizeof(struct frame));
      frame->page = page;
      frame->pid = thread_current()->tid;
      frame_table[f] = frame;
    }
  }

  if (!allocated) {
    exit(-1);
  }
}
