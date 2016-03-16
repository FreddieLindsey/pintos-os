#include "vm/swap.h"
#include "threads/malloc.h"
#include "threads/vaddr.h"
#include <bitmap.h>

static struct block *swap_space;      /* pointer to swap space */
static size_t n_slots;                /* number of slots of page size available
                                         in swap space */
static uint32_t n_page_sectors;       /* number of sectors per page */

static struct list swap_table;        /* table of swapped pages */
static struct bitmap* used_slots;      /* slots in the swap space */
static struct lock swap_table_lock;   /* lock for mutual exclusion of table
                                         and bitmap. The swap space takes care
                                         of its own locks*/

void swap_init() {
  swap_space = block_get_role(BLOCK_SWAP);
  n_slots = block_size(swap_space) * BLOCK_SECTOR_SIZE / PGSIZE;
  n_page_sectors = PGSIZE / BLOCK_SECTOR_SIZE;
  used_slots = bitmap_create(n_slots);
  list_init(&swap_table);
}

void swap_alloc(struct page *page) {
  size_t offset;

  /* find the first available slot */
  for (offset = 0; offset < n_slots; offset++) {
    if (!bitmap_test(used_slots, offset)) {
      break;
    }
  }

  /* the swap space has been saturated */
  if (offset == n_slots) {
    return; // POSSIBLY BREAK EVERYTHING
  }

  /* set the list */
  struct swap* entry = malloc(sizeof(struct swap));
  entry->page = page;
  entry->slot = offset;
  list_push_back(&swap_table, &entry->elem);

  /* write on swap space the page */
  uint32_t i;
  for (i = 0; i < n_page_sectors; i++ ) {
    block_write (swap_space, offset + i, page + i * BLOCK_SECTOR_SIZE);
  }

  /* update the used_slots bitmap */
  bitmap_set (used_slots, offset, true);
}

 void swap_free(struct page *page) {

 }
