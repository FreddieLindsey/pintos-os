#include "threads/malloc.h"
#include "threads/thread.h"
#include "vm/page.h"
#include "filesys/file.h"
#include <stdbool.h>
#include <list.h>
#include <string.h>
#include "userprog/pagedir.h"
#include "threads/vaddr.h"

/* Add a mapping from the addr to a page. Fails if mapping already exists. */
struct page* page_alloc(void *addr, bool read_only) {
  struct thread *t = thread_current();
  if (&t->page_table == NULL) {
    return NULL;
  }

  addr = pg_round_down(addr);
  struct page *p = malloc(sizeof(struct page));
  p->addr = addr;
  p->read_only = read_only;
  p->sector = (block_sector_t) -1;
  p->file = NULL;
  p->frame = NULL;
  p->thread = thread_current();


  /* return NULL if page already exists associated with address */
  if (page_from_addr(addr)) {
    free(p);
    return NULL;
  }
  list_push_back(&t->page_table, &p->elem);
  return p;
}

/* tries to get page associated with addr, otherwise returns NULL */
struct page* page_from_addr(void *addr) {
  struct thread *t = thread_current();
  struct list_elem *e;
  addr = pg_round_down(addr);
  for (e = list_begin(&t->page_table); e != list_end(&t->page_table); e = list_next(e)) {
    struct page* page = list_entry(e, struct page, elem);

    /* Found a page associated with addr so return */
    if (page->addr == addr) {
       return page;
    }
  }
  return NULL;

}

/* Loads page into memory */
bool page_into_memory (void *addr) {
  addr = pg_round_down(addr);

  /* Locate page that faulted in supplemental page table */
  if (&thread_current()->page_table == NULL) {
    return false;
  }

  struct page *page = page_from_addr(addr);
  if (!page) {
    return false;
  }


  if (!page->frame) {
    /* Obtain a frame for page */
    page->frame = frame_alloc(page);
  }

  frame_lock(page);

  if (page->sector != (block_sector_t) -1) {
       /* read from swap */
       swap_free(page);
  } else if (page->file) {
      /* read data from file */
      off_t bytes_read = file_read_at(page->file, page->frame->addr, page->read_bytes,
                                      page->file_offset);
      off_t zero_bytes = PGSIZE - bytes_read;
      /* sets rest of frame to 0 */
      memset(page->frame->addr + bytes_read, 0, zero_bytes);
  } else {
      /* zero page */
      memset(page->frame->addr, 0, PGSIZE);
  }


  /* Point the points the page for the faulting virtual address to the frame */
  bool success = pagedir_set_page(thread_current()->pagedir, page->addr,
                              page->frame->addr, !page->read_only);

  lock_release(&page->frame->lock);
  return success;
}

/* Checks if page has been recently accessed sets bit to 0 if it has */
bool page_accessed_recently(struct page* page) {
  struct thread *t = page->thread;
  bool accessed = pagedir_is_accessed(t->pagedir, page->addr);
  if (accessed) {
    pagedir_set_accessed(t->pagedir, page->addr, false);
    if (page->frame) {
      pagedir_set_accessed(t->pagedir, page->frame->addr, false);
    }
  }

  return accessed;
}

bool page_out_memory(struct page* page) {
  bool success;

  /* Have to clear the page associated with the virtual address */
  pagedir_clear_page(page->thread->pagedir, page->addr);

  if(page->file) {
    if (pagedir_is_dirty(page->thread->pagedir, page->addr)) {
      if (page->mapped) {
        off_t bytes_written = file_write_at(page->file, page->frame->addr,
                              page->read_bytes, page->file_offset);
        /* Has to have written all of the bytes to be successful */
        success = bytes_written == page->read_bytes;
      } else {
        swap_alloc(page);
        success = true;

      }
    } else {
      success = true;
    }
  } else {
    /* Have to write to swap space */
    swap_alloc(page);
    success = true;
  }

  /* If writing was successful then page does not hold frame anymore */
  if (success) {
    page->frame = NULL;
  }

  return success;
}

void page_remove(void* addr) {
  struct page* page = page_from_addr(addr);

  if (!page) {
    return;
  }

  if (page->frame) {
    /* If it was a mmapped file, then evict properly */
    if(page->mapped && page->file) {
      page_out_memory(page);
    }
    frame_release(page->frame);
  }
  list_remove(&page->elem);
  free(page);
}

/* destroys current process' page table */
void page_destroy() {
  struct list *page_table = &thread_current()->page_table;
  struct list_elem *e;

  while(!list_empty(page_table)){
      e = list_pop_front(page_table);
      struct page *p = list_entry (e, struct page, elem);
      list_remove(&p->elem);
      free(p);
  }

}
