#include "vm/frame.h"
#include "threads/malloc.h"
#include "userprog/syscall.h"
#include "threads/palloc.h"


static struct frame **frame_table;

static struct lock frame_table_lock; /* lock for mutual exclusion */

static size_t num_frames;

struct frame* select_frame(void);

void frame_init(int num_of_frames) {
  num_frames = 0;
  void * base;

  frame_table = malloc(sizeof(struct frame*)*num_of_frames);
  if (!frame_table) {
    PANIC ("out of memory to allocated frame table");
  }
  while((base = palloc_get_page(PAL_USER | PAL_ZERO))!= NULL) {
    frame_table[num_frames] = malloc(sizeof(struct frame));
    frame_table[num_frames]->base = base;
    frame_table[num_frames]->page = NULL;
    lock_init(&frame_table[num_frames]->lock);
    num_frames++;
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
    if (!frame_table[i]->page) {
      frame_table[i]->page = page;
      frame_table[i]->pid = thread_current()->tid;
      lock_release(&frame_table_lock);
      return frame_table[i];
    }
  }
  lock_release(&frame_table_lock);

  /* Select frame to evict */
  struct frame* frame = select_frame();
  /* Attempt to move page from memory */
  if(!page_out_memory(frame->page)) {
    frame_unlock(frame);
    return NULL;
  }

  frame->page = page;
  return frame;
}


struct frame* select_frame() {
  return frame_table[0];
}

void frame_free(struct frame *f) {
  if (!f) {
    return;
  }
  palloc_free_page(f->base);
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

void frame_destroy() {
  free(frame_table);
}
