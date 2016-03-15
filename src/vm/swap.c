#include "vm/swap.h"
#include "threads/malloc.h"
#include <list.h>
#include <bitmap.h>
#include "threads/vaddr.h"

static struct block *swap_space;
static size_t n_slots;

static struct list swap_table;        /* table of swapped pages */
static struct lock swap_table_lock;   /* lock for mutual exclusion of table
                                          and bitmap. The swap space takes care
                                          of its own locks*/
static struct bitmap used_slots;      /* slots in the swap space */

void swap_init() {
  swap_space = block_get_role(BLOCK_SWAP);
  n_slots = block_size(swap_space) * BLOCK_SECTOR_SIZE / PGSIZE;
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
  block_write()

  /* update the used_slots bitmap */
}
