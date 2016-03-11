#ifndef FRAME_H
#define FRAME_H

#include "userprog/process.h"

struct frame {
  void* page;  /* pointer to user page */
  uint32_t num; /* frame number */
  pid_t pid;   /* id of the process that owns the frame */
};

void frame_init(int num_of_frames);
struct frame* frame_alloc(void* page);
void frame_free(void* page);


#endif
