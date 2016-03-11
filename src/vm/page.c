#include "vm/page.h"
#include "threads/malloc.h"

void page_add_page(struct list *page_table, void *addr, void *data) {
  struct page *page = malloc(sizeof(struct page));
  page->addr = addr;
  page->data = data;
  list_push_back(page_table, &page->elem);
}
