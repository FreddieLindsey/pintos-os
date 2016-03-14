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
struct page* page_alloc(struct list *page_table, void *addr, bool read_only) {
  struct page *p = malloc(sizeof(struct page));
  p->addr = addr;
  p->read_only = read_only;
  p->sector = (block_sector_t) -1;
  p->file = NULL;
  list_push_back(page_table, &p->elem);
  return p;
}


/* gets page associated with addr */
struct page* page_from_addr(struct list *page_table, void *addr) {

  struct list_elem *e;
  for (e = list_begin(page_table); e != list_end(page_table); e = list_next(e)) {
    struct page* page = list_entry(e, struct page, elem);

    /* Found a page associated with addr so return */
    if (page->addr == addr) {
       return page;
    }
  }
  return NULL;

}

bool page_in (void *addr) {

  struct page *p;
  bool success;
  /* Locate page that faulted in supplemental page table */
  if (&thread_current()->page_table == NULL) {
    return false;
  }

  p = page_from_addr(&thread_current()->page_table, addr);
  if (!p) {
    return false;
  }

  frame_lock(p);
  if (p->frame == NULL) {
    /* Obtain a frame */
    frame_alloc(p);
    if (p->sector != (block_sector_t) -1) {
       /* read from swap */
       //swap_free(p);
    } else if (p->file != NULL) {
      /* read from file */
      off_t read_bytes = file_read_at(p->file, p->frame->base, p->file_offset, p->read_bytes);
      off_t zero_bytes = PGSIZE - read_bytes;
      /* sets rest of frame to 0 */
      memset(p->frame->base + read_bytes, 0, zero_bytes);
    } else {
      /* zero page */
      memset(p->frame->base, 0, PGSIZE);
    }
  }

  success = pagedir_set_page (thread_current()->pagedir, p->addr,
                              p->frame->base, !p->read_only);

  frame_unlock (p->frame);
  /* Point the page table entry for the faulting virtual address to the frame */
  return success;
}
