#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#include <debug.h>
#include <list.h>
#include <stdint.h>
#include "synch.h"

/* States in a thread's life cycle. */
enum thread_status
  {
    THREAD_RUNNING,     /* Running thread. */
    THREAD_READY,       /* Not running but ready to run. */
    THREAD_BLOCKED,     /* Waiting for an event to trigger. */
    THREAD_DYING        /* About to be destroyed. */
  };

/* Thread identifier type.
   You can redefine this to whatever type you like. */
typedef int tid_t;
#define TID_ERROR ((tid_t) -1)          /* Error value for tid_t. */

/* Map region identifier. */
typedef int mapid_t;
#define MAP_FAILED ((mapid_t) -1)

/* Thread priorities. */
#define PRI_MIN 0                       /* Lowest priority. */
#define PRI_DEFAULT 31                  /* Default priority. */
#define PRI_MAX 63                      /* Highest priority. */

#define TIME_SLICE 4

/* A kernel thread or user process.

   Each thread structure is stored in its own 4 kB page.  The
   thread structure itself sits at the very bottom of the page
   (at offset 0).  The rest of the page is reserved for the
   thread's kernel stack, which grows downward from the top of
   the page (at offset 4 kB).  Here's an illustration:

        4 kB +---------------------------------+
             |          kernel stack           |
             |                |                |
             |                |                |
             |                V                |
             |         grows downward          |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             +---------------------------------+
             |              magic              |
             |                :                |
             |                :                |
             |               name              |
             |              status             |
        0 kB +---------------------------------+

   The upshot of this is twofold:

      1. First, `struct thread' must not be allowed to grow too
         big.  If it does, then there will not be enough room for
         the kernel stack.  Our base `struct thread' is only a
         few bytes in size.  It probably should stay well under 1
         kB.

      2. Second, kernel stacks must not be allowed to grow too
         large.  If a stack overflows, it will corrupt the thread
         state.  Thus, kernel functions should not allocate large
         structures or arrays as non-static local variables.  Use
         dynamic allocation with malloc() or palloc_get_page()
         instead.

   The first symptom of either of these problems will probably be
   an assertion failure in thread_current(), which checks that
   the `magic' member of the running thread's `struct thread' is
   set to THREAD_MAGIC.  Stack overflow will normally change this
   value, triggering the assertion. */
/* The `elem' member has a dual purpose.  It can be an element in
   the run queue (thread.c), or it can be an element in a
   semaphore wait list (synch.c).  It can be used these two ways
   only because they are mutually exclusive: only a thread in the
   ready state is on the run queue, whereas only a thread in the
   blocked state is on a semaphore wait list. */
struct thread
  {
    /* Owned by thread.c. */
    tid_t tid;                          /* Thread identifier. */
    int exit_status;                    /* exit status of the thread */
    enum thread_status status;          /* Thread state. */
    char name[16];                      /* Name (for debugging purposes). */
    uint8_t *stack;                     /* Saved stack pointer */
    int priority;                       /* Base priority of thread */
    struct list priorities;             /* Stack of donated priorities */
    struct list donations;              /* Stack of received donations */
    struct list_elem allelem;           /* List element for all threads list */
    int nice;
    int recent_cpu;

    int waited_upon;                /* Flag indicating whether its parent
                                        is waiting on it */
    struct list children;               /* List of child threads */

    /* Shared between thread.c and synch.c. */
    struct list_elem elem;              /* List element. */

    int64_t sleep_until;                /* Earliest time to wake thread. */
    struct semaphore sema;              /* Blocks and unblocks thread. */

#ifdef USERPROG
    /* Owned by userprog/process.c. */
    uint32_t *pagedir;                  /* Page directory. */
    struct list page_table;              /* Supplemental page table */
    struct list filemap;                /* File mapping array */
    int process_init;                   /* Process initiated */
    char* proc_name;
    struct list fd_list;
    struct file* file;                  /* Executable associated file */
    struct semaphore exec_sema;         /* Controls sync in exec */
    struct semaphore wait_sema;
    void *process_esp;
    struct thread* parent;              /* Parent process */
#endif

    /* Owned by thread.c. */
    unsigned magic;                     /* Detects stack overflow. */
  };

/* This is the struct describing a priority element in the priority
   stack in threads */
struct priority_elem {
  int priority;                         /* Actual priority value */
  struct lock *lock;                    /* Lock which needs to be acquired */
  struct thread* t;                     /* Thread who received the donation */
  struct list_elem elem;
};

struct tid_elem {
  tid_t tid;                            /* tid of thread */
  struct list_elem elem;
};

/* List element for mapping files into virtual addresses */
struct filemap_elem {
  mapid_t id;                           /* Map ID of the mapping */
  int fd;                               /* FD of the mapped file */
  void *addr;                           /* Virtual address of mappping */
  int num_pages;                        /* Number of pages mapped */
  struct list_elem elem;                /* List element */
};

struct fd_elem {
  int fd;
  struct file *file;
  struct list_elem elem;
};

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
extern bool thread_mlfqs;


void thread_init (void);
void thread_start (void);

void thread_tick (void);
void thread_print_stats (void);

typedef void thread_func (void *aux);
tid_t thread_create (const char *name, int priority, thread_func *, void *);
bool thread_is_child(tid_t tid);

void thread_block (void);
void thread_unblock (struct thread *);

bool thread_compare(const struct list_elem *a,
                    const struct list_elem *b,
                    void *aux);
bool priority_compare(const struct list_elem *a,
                      const struct list_elem *b,
                      void *aux);


struct thread *thread_current (void);
tid_t thread_tid (void);
const char *thread_name (void);

void thread_exit (void) NO_RETURN;
void thread_run_top(void);
void thread_yield (void);

/* Performs some operation on thread t, given auxiliary data AUX. */
typedef void thread_action_func (struct thread *t, void *aux);
void thread_foreach (thread_action_func *, void *);
struct thread * thread_find_thread(tid_t tid);

int thread_get_priority (void);
int thread_get_priority_of(struct thread *t);
void thread_set_priority (int);
void thread_donate_priority(struct thread *t, int priority, struct lock *lock);
void thread_add_priority(struct thread *t, int priority, struct lock *lock);
void thread_redonate(struct thread *t);

void thread_calculate_priority(struct thread *t);
int thread_get_nice (void);
void thread_set_nice (int);
int thread_get_recent_cpu (void);
void thread_calculate_cpu (struct thread *t);
void increment_r_cpu(void) ;
int thread_get_load_avg (void);

#endif /* threads/thread.h */
