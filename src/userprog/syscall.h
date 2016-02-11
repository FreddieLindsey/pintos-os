#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>


void syscall_init (void);

/* pid_t should be defined appropriately (for exec and wait) */

/* Tasks 2 and later system calls. */
void halt (void);
void exit (int status);
int exec (const char *file);
int wait (int);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd);
int read (int fd, void *buffer, unsigned length);
int write (int fd, const void *buffer, unsigned length);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);

#endif /* userprog/syscall.h */
