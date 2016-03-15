#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include "userprog/process.h"

/* Map region identifier. */
typedef int mapid_t;
#define MAP_FAILED ((mapid_t) -1)

void syscall_init (void);

/* pid_t should be defined appropriately (for exec and wait) */

/* Tasks 2 and later system calls. */
void halt (void);
void exit (int status);
pid_t exec (const char *file);
int wait (pid_t);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd);
int read (int fd, void *buffer, unsigned length);
int write (int fd, const void *buffer, unsigned length);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);
mapid_t mmap (int fd, void *data);
void munmap (mapid_t map);


#endif /* userprog/syscall.h */
