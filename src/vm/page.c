#include "vm/page.h"
#include "threads/malloc.h"

void page_add_page(struct list *page_table, void *addr, void *data) {
  struct page *page = malloc(sizeof(struct page));
  page->addr = addr;
  page->data = data;
  list_push_back(page_table, &page->elem);
}

struct page* page_get_page(struct list *page_table, void *addr) {

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
