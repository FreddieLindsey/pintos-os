#include "userprog/syscall.h"
#include "userprog/process.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "filesys/filesys.h"

#define MAX_ARGS 3

static struct lock filesys_lock;

static void syscall_handler (struct intr_frame *);
void read_args(void* esp, int num, void** args);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&filesys_lock);
}

static void
syscall_handler (struct intr_frame *f)
{

  /* Read the number of the system call */
  int syscall_num = *(int*)(f->esp);

  /* Read the arguments of the system call */

  /* array which holds the arguments of the system call */
  /* also passes to the appropriate function */
  void* args[MAX_ARGS];
  switch (syscall_num) {
    case SYS_HALT: halt(); break;
    case SYS_EXIT: read_args(f->esp, 1, args);
                   exit(*(int*)args[0]); break;
    case SYS_EXEC: read_args(f->esp, 1, args);
                   f->eax = exec((char*)args[0]); break;
    case SYS_WAIT: read_args(f->esp, 1, args);
                   f->eax = wait(*(int*)args[0]); break;
    case SYS_CREATE: read_args(f->esp, 2, args);
                     f->eax = create((char*)args[0], *(unsigned*)args[1]); break;
    case SYS_REMOVE: read_args(f->esp, 1, args);
                     f->eax = remove((char*)args[0]); break;
    case SYS_OPEN: read_args(f->esp, 1, args);
                     f->eax = open((char*)args[0]); break;
    case SYS_FILESIZE: read_args(f->esp, 1, args);
                       f->eax = filesize(*(int*)args[0]); break;
    case SYS_READ: read_args(f->esp, 3, args);
                   f->eax = read(*(int*)args[0], args[1], *(unsigned*)args[2]);
                   break;
    case SYS_WRITE: read_args(f->esp, 3, args);
                    f->eax = write(*(int*)args[0], args[1], *(unsigned*)args[2]);
                    break;
    case SYS_SEEK: read_args(f->esp, 2, args);
                   seek(*(int*)args[0], *(unsigned*)args[1]); break;
    case SYS_TELL: read_args(f->esp, 1, args);
                   f->eax = tell(*(int*)args[0]); break;
    case SYS_CLOSE: read_args(f->esp, 1, args);
                    close (*(int*)args[0]); break;
  }

  printf ("system call!\n");
  thread_exit ();
}

/* Reads num arguments from esp and stores them in args */
void read_args(void* esp, int num, void** args) {
  int i = 0;
  for (; i < num; i++) {
    esp++;
    args[i] = esp;
  }
}

void halt (void) {
  shutdown_power_off();
}

void exit (int status UNUSED) {
  process_exit();
}

int exec (const char *file UNUSED) {
  return 0;
}

int wait (int pid UNUSED) {
  return 0;
}

bool create (const char *file, unsigned initial_size) {
  lock_acquire(&filesys_lock);
  bool success = filesys_create(file, initial_size);
  lock_release(&filesys_lock);
  return success;
}

bool remove (const char *file) {
  lock_acquire(&filesys_lock);
  bool success = filesys_remove(file);
  lock_release(&filesys_lock);
  return success;
}

int open (const char *file) {
  lock_acquire(&filesys_lock);
  struct file *f = filesys_open(file);

  /* If the file could not be opened, return -1 */
  if (f == NULL) {
    lock_release(&filesys_lock);
    return -1;
  }

  /* Adds file to file descriptor table */
  int fd = process_generate_fd(f);

  lock_release(&filesys_lock);
  return fd;
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
