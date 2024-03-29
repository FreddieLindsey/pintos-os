#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
#include "filesys/filesys.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

int process_generate_fd(struct file *file);
struct file* process_get_file(int fd);
void process_remove_fd(int fd);

/* Process identifier type. */
typedef int pid_t;

#endif /* userprog/process.h */
