#include "vm/frame.h"
#include "threads/malloc.h"
#include "userprog/syscall.h"
#include "threads/palloc.h"


static struct frame **frame_table;     /* array storing the frame table */

static struct lock frame_table_lock;   /* lock for mutual exclusion */

static size_t num_frames;              /* number of frames in the table */

struct frame* select_frame(void);

void frame_init(int num_of_frames) {
  num_frames = 0;
  void * addr;

  frame_table = malloc(sizeof(struct frame*)*num_of_frames);
  
  /* Keeps trying to allocate pages until it runs out */
  while((addr = palloc_get_page(PAL_USER | PAL_ZERO))) {
    frame_table[num_frames] = malloc(sizeof(struct frame));
    frame_table[num_frames]->addr = addr;
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
  frame_lock(frame->page);
  if(!page_out_memory(frame->page)) {
    lock_release(&frame->lock);
    return NULL;
  }

  frame->page = page;
  lock_release(&frame->lock);
  return frame;
}


struct frame* select_frame() {
  int i = 0;
  for (; i < num_frames * 2; i++) {
    struct frame *frame = frame_table[i%num_frames];
    if (!lock_try_acquire(&frame->lock)) {
      continue;
    }

    if (frame->page) {
      /* Second chance */
      if (!page_accessed_recently(frame->page)) {
        lock_release(&frame->lock);
        return frame;
      }
    }
  }
  return NULL;

}

/* Releases page from frame frame */
void frame_release(struct frame *frame) {
  if (!frame) {
    return;
  }
  frame->page = NULL;
}

/* Acquires the lock of the frame associated with the page */
void frame_lock (struct page *page) {
  if (page->frame)
    {
      lock_acquire (&page->frame->lock);
    }
}

/* Destroys frame_table */
void frame_destroy() {
  free(frame_table);
}
