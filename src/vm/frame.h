#ifndef FRAME_H
#define FRAME_H

#include "userprog/process.h"

struct frame {
  void* page;  /* pointer to user page */
  pid_t pid;   /* id of the process that owns the frame */
};

void frame_init(int num_of_frames);
void frame_alloc(void* page);


#endif
