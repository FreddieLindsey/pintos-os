#ifndef PAGE_H
#define PAGE_H

#include <list.h>
#include <stdint.h>

struct page {
  uint32_t page; /* Address of page */
  struct list_elem elem;
};


#endif
