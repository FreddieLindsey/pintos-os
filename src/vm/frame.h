#ifndef FRAME_H
#define FRAME_H

#include "userprog/process.h"
#include "vm/page.h"

struct frame {
  void* base;  /* base address */
  struct page *page; /* pointer to page associated with frame (if it exists) */
  struct lock lock; /* lock associated with frame for synchronisation */
  pid_t pid;   /* id of the process that owns the frame */
};

void frame_init(int num_of_frames);
struct frame* frame_alloc(struct page* page);
void frame_free(void* page);


#endif
