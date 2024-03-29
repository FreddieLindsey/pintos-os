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
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "vm/page.h"

#define MAX_ARGS 3

struct lock filesys_lock;

static void syscall_handler (struct intr_frame *);
static mapid_t insert_mapping(struct list *filemap, int fd,
                       void *addr, int num_pages);
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
  /* update esp of process to esp on frame. Used for
     checking stack operations in kernel page faults  */
  thread_current()->process_esp = f->esp;
  /* array which holds the arguments of the system call */
  /* also passes to the appropriate function */
  void* args[MAX_ARGS];

  /* Read the arguments of the system call and call the appropriate one */

  switch (syscall_num) {
    case SYS_HALT:
    halt();
    break;

    case SYS_EXIT:
    read_args(f->esp, 1, args);
    exit(*(int*)args[0]);
    break;

    case SYS_EXEC:
    read_args(f->esp, 1, args);
    f->eax = exec(*(char**)args[0]); thread_yield();
    break;

    case SYS_WAIT:
    read_args(f->esp, 1, args);
    f->eax = wait(*(int*)args[0]);
    break;

    case SYS_CREATE:
    read_args(f->esp, 2, args);
    f->eax = create(*(char**)args[0], *(unsigned*)args[1]);
    break;

    case SYS_REMOVE:
    read_args(f->esp, 1, args);
    try_acquire_filesys();
    f->eax = remove(*(char**)args[0]);
    try_release_filesys();
    break;

    case SYS_OPEN:
    read_args(f->esp, 1, args);
    try_acquire_filesys();
    f->eax = open(*(char**)args[0]);
    try_release_filesys();
    break;

    case SYS_FILESIZE:
    read_args(f->esp, 1, args);
    f->eax = filesize(*(int*)args[0]);
    break;

    case SYS_READ:
    read_args(f->esp, 3, args);
    try_acquire_filesys();
    f->eax = read(*(int*)args[0], *(void**)args[1], *(unsigned*)args[2]);
    try_release_filesys();
    break;

    case SYS_WRITE:
    read_args(f->esp, 3, args);
    try_acquire_filesys();
    f->eax = write(*(int*)args[0], *(void**)args[1], *(unsigned*)args[2]);
    try_release_filesys();
    break;

    case SYS_SEEK:
    read_args(f->esp, 2, args);
    seek(*(int*)args[0], *(unsigned*)args[1]);
    break;

    case SYS_TELL:
    read_args(f->esp, 1, args);
    f->eax = tell(*(int*)args[0]);
    break;

    case SYS_CLOSE:
    read_args(f->esp, 1, args);
    close(*(int*)args[0]);
    break;

    case SYS_MMAP:
    read_args(f->esp, 2, args);
    f->eax = mmap(*(int*)args[0], *(void**)args[1]);
    break;

    case SYS_MUNMAP:
    read_args(f->esp, 1, args);
    munmap(*(mapid_t*)args[0]);
    break;
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
  /* Tries to exit, but needs to wait for parent to call wait*/
  sema_down(&thread_current()->parent->wait_sema);
  thread_current()->exit_status = status;
  printf ("%s: exit(%d)\n", thread_current()->proc_name, status);
  process_exit();
  thread_yield();
  thread_exit();
}

pid_t exec (const char *file_name) {
  check_valid_ptr(file_name);

  char *arg, *save_ptr;
  int str_len = strlen(file_name) + 1;
  char file_name_copy[str_len];
  strlcpy(file_name_copy, file_name, str_len);
  arg = strtok_r(file_name_copy, " ", &save_ptr);

  pid_t pid = -1;
  if (filesys_open(arg)) {
    pid = process_execute(file_name);
  }
  return pid;
}

int wait (pid_t pid) {
  return process_wait(pid);
}

bool create (const char *file, unsigned initial_size) {
  /* Checks if file is null or an invalid pointer */
  check_valid_ptr(file);

  /* Checks if filename is empty string */
  if(!strcmp(file, "")) {
    return false;
  }

  bool success = filesys_create(file, initial_size);
  return success;
}

bool remove (const char *file) {
  /* Checks if file is null or an invalid pointer */
  check_valid_ptr(file);

  /* Checks if filename is empty string */
  if(!strcmp(file, "")) {
    return false;
  }
  bool success = filesys_remove(file);

  return success;
}

int open (const char *file) {
  /* Checks if file is null or an invalid pointer */
  check_valid_ptr(file);

  /* Checks if filename is empty string */
  if(!strcmp(file, "")) {
    return -1;
  }

  struct file *f = filesys_open(file);

  /* If the file could not be opened, return -1 */
  if (f == NULL) {
    return -1;
  }
  /* Adds file to file descriptor table */
  int fd = process_generate_fd(f);
  return fd;
}

int filesize (int fd) {
  return file_length(process_get_file(fd));
}

int read (int fd, void *buffer, unsigned length) {

  /* Checks if buffer is in user memory */
  if (!buffer ||!is_user_vaddr(buffer)) {
    exit(-1);
  }

  if (fd == STDIN_FILENO) {
    input_getc();
    return length;
  }

  struct file *file = process_get_file(fd);
  if (!file) {
    lock_release(&filesys_lock);
    exit(-1);
  }

  int bytes_read = file_read(file, buffer, length);
  return bytes_read;

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
  int bytes_written = file_write(file, buffer, length);
  return bytes_written;
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
  process_remove_fd(fd);
}

/* Maps an opened file fd to memory, at the address addr. */
mapid_t mmap (int fd, void *addr) {
  /* Get corresponding file and its size */
  struct file *f = process_get_file(fd);
  if(!f) {
    return MAP_FAILED;
  }
  off_t size = file_length(f);

  /* Determine the number of pages needed */
  int num_pages = size / PGSIZE + 1;

  /* Fail if the file has a lenght of zero, if the address is not page
     aligned, if the address is 0, or if the file descriptor is 0 or 1 */
  bool page_aligned = (int) addr % PGSIZE == 0;
  if (!page_aligned || size == 0 || addr == 0 || fd == 0 || fd == 1) {
    return MAP_FAILED;
  }

  /* Check if new mapping will overlap existing memory, in which case fail */
  int i;
  for (i = 0; i < num_pages; ++i) {
    if (page_from_addr(addr + i * PGSIZE)) return MAP_FAILED;
  }

  /* Read a page at a time from the file until there is nothing more to read */
  for (i = 0; i < num_pages; ++i) {


    struct page *curr_page = page_alloc(addr + i * PGSIZE, false);

    /* If curr_page is null then it overlaps */
    if (!curr_page) {
      int j;
      for (j = 0; j < i; j++) {
        page_remove(addr + j * PGSIZE);
      }
      return MAP_FAILED;
    }
    curr_page->file = f;
    curr_page->file_offset = i * PGSIZE;
    curr_page->read_bytes = size >= PGSIZE ? PGSIZE : size;
    curr_page->mapped = true;
    size -= PGSIZE;
  }

  return insert_mapping(&thread_current()->filemap, fd, addr, num_pages);
}

/* Unmaps mapped memory. */
void munmap (mapid_t map) {
  /* Find the mapping in the file map table */
  struct thread *t = thread_current();
  struct list_elem *e;
  struct filemap_elem *mapping = NULL;
  for (e = list_begin (&t->filemap); e != list_end (&t->filemap);) {
    struct filemap_elem *fm = list_entry (e, struct filemap_elem, elem);
    if (fm->id == map) {
      mapping = fm;
      break;
    }
  }
  /* Mapping not found in the file map table */
  if (mapping == NULL) {
    return;
  }

  /* Get current process's page table */
  uint32_t *pd = thread_current()->pagedir;
  if (!pd) {
    return;
  }
  /* Write each page back to the file if required, and remove the page and
     its index in the page directory */
  int i;
  for (i = 0; i < mapping->num_pages; ++i) {
    /* Clear the address mapping in the page directory */
    pagedir_clear_page(pd, mapping->addr + i * PGSIZE);

    /* Remove page from page table */
    page_remove(mapping->addr + i * PGSIZE);
  }

  /* Remove mapping from the file map table */
  list_remove(&mapping->elem);
  free(mapping);

}

/* Inserts the new mapping into the first available slot in the filemap,
   returning the index into which it is mapped as a mapid_t */
static mapid_t insert_mapping(struct list *filemap, int fd,
                       void *addr, int num_pages) {
  /* Get the last element in the filemap */
  struct list_elem *back = list_rbegin(filemap);
  mapid_t id = 0;
  /* Set new map ID based on the last, if the list is not empty */
  if (list_head(filemap) != back) {
    struct filemap_elem *fm = list_entry (back, struct filemap_elem, elem);
    id = fm->id + 1;
  }
  /* Set up struct members for file mapping */
  struct filemap_elem *mapping = malloc(sizeof(struct filemap_elem));
  mapping->id = id;
  mapping->fd = fd;
  mapping->addr = addr;
  mapping->num_pages = num_pages;
  list_push_back(filemap, &mapping->elem);
  return id;
}

void check_valid_ptr(void* ptr) {
  if (!ptr) {
    exit(-1);
  }

  if (!is_user_vaddr(ptr)) {
    exit(-1);
  }

  if (!pagedir_get_page(thread_current()->pagedir, ptr)) {
    free(ptr);
    exit(-1);
  }
}

void try_release_filesys() {
  if(filesys_lock.holder == thread_current()) {
    lock_release(&filesys_lock);
  }
}

void try_acquire_filesys() {
  if(filesys_lock.holder != thread_current()) {
    lock_acquire(&filesys_lock);
  }
}
