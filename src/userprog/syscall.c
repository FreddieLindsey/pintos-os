#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "userprog/pagedir.h"
#include "threads/vaddr.h"

#define MAX_ARGS 3

static struct lock filesys_lock;


static void syscall_handler (struct intr_frame *);
void read_args(void* esp, int num, void** args);
void check_valid_ptr(void* ptr);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&filesys_lock);
}

static void
syscall_handler (struct intr_frame *f)
{
  /* Checks if stack pointer is valid */
  check_valid_ptr(f->esp);
  /* Read the number of the system call */
  int syscall_num = *(int*)(f->esp);

  //printf("Sup %d\n", syscall_num);
  /* array which holds the arguments of the system call */
  /* also passes to the appropriate function */
  void* args[MAX_ARGS];

  /* Read the arguments of the system call and call the appropriate one */
  switch (syscall_num) {
    case SYS_HALT: halt(); break;
    case SYS_EXIT: read_args(f->esp, 1, args);
                   exit(*(int*)args[0]); break;
    case SYS_EXEC: read_args(f->esp, 1, args);
                   f->eax = exec(*(char**)args[0]); thread_yield(); break;
    case SYS_WAIT: read_args(f->esp, 1, args);
                   f->eax = wait(*(int*)args[0]); break;
    case SYS_CREATE: read_args(f->esp, 2, args);
                     f->eax = create(*(char**)args[0], *(unsigned*)args[1]); break;
    case SYS_REMOVE: read_args(f->esp, 1, args);
                     f->eax = remove(*(char**)args[0]); break;
    case SYS_OPEN: read_args(f->esp, 1, args);
                     f->eax = open(*(char**)args[0]); break;
    case SYS_FILESIZE: read_args(f->esp, 1, args);
                       f->eax = filesize(*(int*)args[0]); break;
    case SYS_READ: read_args(f->esp, 3, args);
                   f->eax = read(*(int*)args[0], *(void**)args[1], *(unsigned*)args[2]);
                   break;
    case SYS_WRITE: read_args(f->esp, 3, args);
                    f->eax = write(*(int*)args[0], *(void**)args[1], *(unsigned*)args[2]);
                    break;
    case SYS_SEEK: read_args(f->esp, 2, args);
                   seek(*(int*)args[0], *(unsigned*)args[1]); break;
    case SYS_TELL: read_args(f->esp, 1, args);
                   f->eax = tell(*(int*)args[0]); break;
    case SYS_CLOSE: read_args(f->esp, 1, args);
                     (*(int*)args[0]); break;
  }
}

/* Reads num arguments from esp and stores them in args */
void read_args(void* esp, int num, void** args) {
  int i = 0;
  void* p = esp;
  for (; i < num; i++) {
    p += 4;
    if (!p || !is_user_vaddr(p)) {
      exit(-1);
    }
    args[i] = p;


  }
}

void halt (void) {
  shutdown_power_off();
}

void exit (int status) {
  thread_current()->exit_status = status;
  printf ("%s: exit(%d)\n", thread_current()->proc_name, status);
  process_exit();
  thread_yield();
  thread_exit();
}

pid_t exec (const char *file_name) {
  check_valid_ptr(file_name);

  char *arg, *save_ptr;
  int str_len = strlen(file_name);
  char file_name_copy[str_len];
  strlcpy(file_name_copy, file_name, str_len);

  arg = strtok_r(file_name_copy, " ", &save_ptr);
  return (!filesys_open (arg)) ?
    -1 :
    process_execute(file_name);
}

int wait (pid_t pid) {
  // TODO: Convert pid to corresponding tid
  return process_wait(pid);
}

bool create (const char *file, unsigned initial_size) {

  /* Checks if file is null or an invalid pointer */
  check_valid_ptr(file);

  /* Checks if filename is empty string */
  if(!strcmp(file, "")) {
    return false;
  }

  lock_acquire(&filesys_lock);
  bool success = filesys_create(file, initial_size);
  lock_release(&filesys_lock);
  return success;
}

bool remove (const char *file) {
  /* Checks if file is null or an invalid pointer */
  check_valid_ptr(file);


  /* Checks if filename is empty string */
  if(!strcmp(file, "")) {
    return false;
  }

  lock_acquire(&filesys_lock);
  bool success = filesys_remove(file);
  lock_release(&filesys_lock);
  return success;
}

int open (const char *file) {
  /* Checks if file is null or an invalid pointer */
  check_valid_ptr(file);

  /* Checks if filename is empty string */
  if(!strcmp(file, "")) {
    return -1;
  }

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

int filesize (int fd) {
  return file_length(process_get_file(fd));
}

int read (int fd, void *buffer, unsigned length) {


  if (!buffer || !is_user_vaddr(buffer)) {
    exit(-1);
  }

  if (fd == STDIN_FILENO) {
    input_getc();
    return length;
  }


  struct file *file = process_get_file(fd);
  if (!file) {
    exit(-1);
  }
  return file_read(file, buffer, length);
}

int write (int fd, const void *buffer, unsigned length) {

  check_valid_ptr(buffer);

  if (fd == STDOUT_FILENO) {
    putbuf(buffer, length);
    return length;
  }

  struct file *file = process_get_file(fd);
  if (!file) {
    return -1;
  }
  return file_write(file, buffer, length);
}

void seek (int fd, unsigned position) {
  struct file *file = process_get_file(fd);
  file_seek(file, position);
}

unsigned tell (int fd UNUSED) {
  struct file *file = process_get_file(fd);
  return file_tell(file);
}

void close (int fd) {
  struct file *file = process_get_file(fd);
  file_close(file);
  process_remove_fds(file);
}

void check_valid_ptr(void* ptr) {
  if (!(ptr && pagedir_get_page(thread_current()->pagedir, ptr))) {
    exit(-1);
  }
}
