#include "threads/malloc.h"
#include "vm/page.h"
#include <stdbool.h>
#include <list.h>


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
struct page* page_get_page_from_addr(struct list *page_table, void *addr) {

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
  /* Locate page that faulted in supplemental page table */

  /* Obtain a frame */

  /* Fetch data into the frame */

  /* Point the page table entry for the faulting virtual address to the frame */
  return addr == 0;
}
