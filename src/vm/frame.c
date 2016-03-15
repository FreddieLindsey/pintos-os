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
  return NULL;
}

void frame_free(struct frame *f) {
  if (!f) {
    return;
  }
  lock_release(&f->lock);
  free(f);

}

void frame_lock (struct page *p) {
  struct frame *f = p->frame;
  if (f != NULL)
    {
      lock_acquire (&f->lock);
      if (f != p->frame)
        {
          lock_release (&f->lock);
        }
    }
}

void frame_unlock(struct frame* f) {
  ASSERT(lock_held_by_current_thread(&f->lock))
  lock_release(&f->lock);
}
