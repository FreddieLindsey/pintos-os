#ifndef PAGE_H
#define PAGE_H

#include <list.h>
#include <stdint.h>
#include <stdbool.h>
#include "devices/block.h"
#include "vm/frame.h"

#define MAX_STACK 8 * 1024 * 1024 /* maximum stack size (8MB) */

struct page {
  void *addr; /* virtual address of page */
  bool read_only;

  struct frame *frame; /* The frame that is associated with the page */

  block_sector_t sector;

  struct file *file;
  uint32_t read_bytes;
  off_t file_offset;

  struct list_elem elem;
};


struct page* page_alloc(void *addr, bool read_only);
struct page* page_from_addr(void *addr);
bool page_into_memory (void *addr);
void page_destroy(void);

#endif
