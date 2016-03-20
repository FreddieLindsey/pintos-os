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
  lock_init(&swap_table_lock);
}

/* Allocates and locks a page to an offset in the swap space */
void swap_alloc(struct page *page) {
  lock_acquire(&swap_table_lock);
  block_sector_t offset;

  /* find the first available slot */
  for (offset = 0; offset < n_slots; offset++) {
    if (!bitmap_test(used_slots, offset)) {
      break;
    }
  }

  /* the swap space has been saturated */
  if (offset == n_slots) {
    lock_release(&swap_table_lock);
    return; // POSSIBLY BREAK EVERYTHING
  }

  /* update the used_slots bitmap */
  bitmap_set (used_slots, offset, true);
  lock_release(&swap_table_lock);

  /* set the list */
  struct swap* entry = malloc(sizeof(struct swap));
  entry->page = page;
  entry->page->sector = offset;
  entry->slot = offset;
  lock_init(&entry->lock);
  list_push_back(&swap_table, &entry->elem);

  /* write on swap space the page */
  uint32_t i;
  for (i = 0; i < n_page_sectors; i++ ) {
    block_write (swap_space, offset + i, page + i * BLOCK_SECTOR_SIZE);
  }
}

/* Frees the page from swap table and copies it in the frame table */
 void swap_free(struct page *page) {
   /* identify table entry */
   struct list_elem *e;
   struct swap *swap_entry;
   for (e = list_begin(&swap_table); e != list_end (&swap_table);
        e = list_next (e))
        {
          swap_entry = list_entry (e, struct swap, elem);
          if (swap_entry->page == page) {
            lock_acquire(&swap_entry->lock);
            break;
          }
        }
   /* if page not found it breaks */
   if (!swap_entry) {
     return; // BREAK HARD
   }

   /* copy back to frame table */
   uint32_t i;
   for (i = 0; i < n_page_sectors; i++ ) {
     block_read(swap_space,
                swap_entry->slot + i,
                page->frame->base + i * BLOCK_SECTOR_SIZE);
   }
   page->sector = (block_sector_t) -1;

   /* clear used_slots */
   lock_acquire(&swap_table_lock);
   bitmap_set(used_slots, swap_entry->slot, false);
   lock_release(&swap_table_lock);

   /* remove node from table */
   list_remove(&swap_entry->elem);
   lock_release(&swap_entry->lock);
   free(swap_entry);
 }
