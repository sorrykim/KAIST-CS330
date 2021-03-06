#include "page.h"

void
page_init (void) {
  return;
}

unsigned page_hash (const struct hash_elem *p_, void *aux UNUSED) {

  const struct sup_page_entry *p = hash_entry(p_, struct sup_page_entry , hash_elem);
  return hash_bytes(&p->upage, sizeof p->upage);

}

bool page_less (const struct hash_elem *a_, const struct hash_elem *b_, void *aux UNUSED) {

  const struct sup_page_entry *a = hash_entry(a_, struct sup_page_entry, hash_elem);
  const struct sup_page_entry *b = hash_entry(b_, struct sup_page_entry, hash_elem);

  return a->upage < b->upage;
}

struct sup_page_entry *
sup_page_entry_create (void *upage, void *kpage, struct file *file) {
  // store va to pa mapping in sup page table.
  struct sup_page_entry *sup_pte = (struct sup_page_entry *)malloc(sizeof(struct sup_page_entry));
  sup_pte->upage = upage;
  sup_pte->kpage = kpage;
  sup_pte->is_valid = true; //???????
  sup_pte->file_address = file;
  sup_pte->swap_address = NULL;
  hash_insert(&thread_current()->sup_page_table, &sup_pte->hash_elem);
  return sup_pte;
}

struct sup_page_entry *
sup_page_table_lookup (struct hash *sup_page_table, const void *upage) {
  struct sup_page_entry sup_pte;
  struct hash_elem *e;

  sup_pte.upage = (void *)upage;
  e = hash_find(sup_page_table, &sup_pte.hash_elem);
  return e != NULL ? hash_entry(e, struct sup_page_entry, hash_elem) : NULL;
}

struct sup_page_entry *
sup_page_table_file_lookup (struct hash *sup_page_table, struct file* file) {
  struct sup_page_entry sup_pte;
  struct hash_elem *e;
  sup_pte.file_address = file;
  e = hash_find(sup_page_table, &sup_pte.hash_elem);
  return e != NULL ? hash_entry(e, struct sup_page_entry, hash_elem) : NULL;
}

void
sup_page_entry_delete(struct sup_page_entry* sup_pte){
  struct hash *sup_page_table = &thread_current()->sup_page_table;
  hash_delete(sup_page_table, &sup_pte->hash_elem);
  pagedir_clear_page (thread_current()->pagedir, sup_pte->upage);
  if (sup_pte->swap_address != NULL) {
    free(sup_pte->swap_address);
  }
  free(sup_pte);
}
