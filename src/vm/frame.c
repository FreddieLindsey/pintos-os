#include "vm/frame.h"
#include "threads/malloc.h"
#include "userprog/syscall.h"


static struct frame **frame_table;

static struct lock frame_table_lock; /* lock for mutual exclusion */

static size_t num_frames;

void frame_init(int num_of_frames) {
  num_frames = num_of_frames;
  frame_table = malloc(sizeof(struct frame*)*num_frames);
  if (!frame_table) {
    PANIC ("out of memory to allocated frame table");
  }
  lock_init(&frame_table_lock);

}

/* Allocates and locks a page to a frame */
struct frame* frame_alloc(struct page *page) {

  if (!page) {
    return NULL;
  }

  unsigned i;
  lock_acquire(&frame_table_lock);
  for(i = 0; i < num_frames; i++) {
    struct frame* f = frame_table[i];
    if (f == NULL) {
      struct frame *frame = malloc(sizeof(struct frame));
      frame->page = page;
      frame->pid = thread_current()->tid;
      lock_init(&frame->lock);
      lock_acquire(&frame->lock);
      lock_release(&frame_table_lock);
      return frame;
    }
  }
  lock_release(&frame_table_lock);

  // Will contain eviction code
  exit(-1);
}

void frame_free(void* page) {
  if (!page) {
    return;
  }

  unsigned f;
  for(f = 0; f < num_frames; f++) {
    lock_acquire(&frame_table_lock);
    if (frame_table[f] && frame_table[f]->page == page) {
      free(frame_table[f]);
      lock_release(&frame_table_lock);
      return;
    }
    lock_release(&frame_table_lock);
  }

}
