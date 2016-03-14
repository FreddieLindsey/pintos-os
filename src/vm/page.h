#ifndef PAGE_H
#define PAGE_H

#include <list.h>
#include <stdint.h>
#include <stdbool.h>
#include "devices/block.h"

struct page {
  void *addr; /* virtual address of page */
  bool read_only;

  struct frame *frame; /* The frame that is associated with the page */

  block_sector_t sector;

  struct file *file;
  struct list_elem elem;
};


struct page* page_alloc(struct list *page_table, void *addr, bool read_only);
struct page* page_get_page_from_addr(struct list *page_table, void *addr);
bool page_in (void *addr);

#endif
