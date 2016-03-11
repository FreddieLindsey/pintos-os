#ifndef PAGE_H
#define PAGE_H

#include <list.h>
#include <stdint.h>

struct page {
  void *addr; /* virtual address of page */
  void *data;  /* The data that should be in the page */
  void *frame; /* The frame that is associated with the page */
  struct list_elem elem;
};

void page_add_page(struct list *page_table, void *addr, void * data);
struct page* page_get_page(struct list *page_table, void *addr);


#endif
