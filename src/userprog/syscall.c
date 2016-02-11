#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
  printf ("system call!\n");
  thread_exit ();
}

void halt (void) {

}

void exit (int status UNUSED) {

}

int exec (const char *file UNUSED) {
  return 0;
}

int wait (int pid UNUSED) {
  return 0;
}

bool create (const char *file UNUSED, unsigned initial_size UNUSED) {
  return false;
}

bool remove (const char *file UNUSED) {
  return false;
}

int open (const char *file UNUSED) {
  return 0;
}

int filesize (int fd UNUSED) {
  return 0;
}

int read (int fd UNUSED, void *buffer UNUSED, unsigned length UNUSED) {
  return 0;
}

int write (int fd UNUSED, const void *buffer UNUSED, unsigned length UNUSED) {
  return 0;
}

void seek (int fd UNUSED, unsigned position UNUSED) {

}

unsigned tell (int fd UNUSED) {
  return 0;
}

void close (int fd UNUSED) {

}
