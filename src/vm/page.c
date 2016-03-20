#include "threads/malloc.h"
#include "threads/thread.h"
#include "vm/page.h"
#include "filesys/file.h"
#include <stdbool.h>
#include <list.h>
#include <string.h>
#include "userprog/pagedir.h"
#include "threads/vaddr.h"

/* Add a mapping from VADDR to page. Fails if mapping already exists. */
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
  struct page *p;
  bool success;

  addr = pg_round_down(addr);

  /* Locate page that faulted in supplemental page table */
  if (&thread_current()->page_table == NULL) {
    return false;
  }

  p = page_from_addr(addr);
  if (!p) {
    return false;
  }

  if (p->frame == NULL) {
    /* Obtain a frame */
    p->frame = frame_alloc(p);
  }

  frame_lock(p);

  if (p->sector != (block_sector_t) -1) {
       /* read from swap */
       swap_free(p);
  } else if (p->file) {
      /* read from file */
      off_t read_bytes = file_read_at(p->file, p->frame->base,  p->read_bytes, p->file_offset);
      off_t zero_bytes = PGSIZE - read_bytes;
      /* sets rest of frame to 0 */
      memset(p->frame->base + read_bytes, 0, zero_bytes);
  } else {
      /* zero page */
      memset(p->frame->base, 0, PGSIZE);
  }

  /* Point the page table entry for the faulting virtual address to the frame */
  success = pagedir_set_page (thread_current()->pagedir, p->addr,
                              p->frame->base, !p->read_only);

  frame_unlock(p->frame);

  return success;
}


bool page_out_memory(struct page* page) {
  ASSERT(page->frame);
  bool modified;
  bool success;

  pagedir_clear_page(page->thread->pagedir, page->addr);

  modified = pagedir_is_dirty(page->thread->pagedir, page->addr);

  if(page->file) {
    if (modified) {
      success = file_write_at(page->file, page->frame->base, page->read_bytes, page->file_offset) == page->read_bytes;
    } else {
      success = true;
    }
  } else {
    swap_alloc(page);
    success = true;
  }

  if (success) {
    page->frame = NULL;
  }

  return success;


}

/* destroys current process' page table */
void page_destroy() {
  struct list *page_table = &thread_current()->page_table;
  struct list_elem *e;

  while(!list_empty(page_table)){
      e = list_pop_front(page_table);
      struct page *p = list_entry (e, struct page, elem);
      free(p);
  }

}
