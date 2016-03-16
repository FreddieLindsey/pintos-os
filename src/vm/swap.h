#ifndef SWAP_H
#define SWAP_H

#include "devices/block.h"
#include <list.h>

struct swap {
  struct page *page;       /* evicted page */
  size_t slot;             /* swap slot */
  struct list_elem elem;  /* needed for list */
};

void swap_init(void);
void swap_alloc(struct page *page);
void swap_free(struct page *page);

#endif
