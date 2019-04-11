#include "SortedList.h"
#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <string.h>

int opt_yield;
void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
  if(list == NULL || element == NULL)
    return;
  SortedList_t *head = list;
  SortedList_t *ptr = list->next;

  if(opt_yield & INSERT_YIELD)
    sched_yield();
  while(ptr != head) {
    if(strcmp(element->key, ptr->key) <= 0)
      break;
    ptr = ptr->next;
  }
  element->prev = ptr->prev;
  element->next = ptr;
  ptr->prev->next = element;
  ptr->prev = element;
}

int SortedList_delete( SortedListElement_t *element) {
  if(element == NULL)
    return 1;
  if((element->prev->next != element) || (element->next->prev != element))
    return -1;
  if(opt_yield & DELETE_YIELD)
    sched_yield();
  element->prev->next = element->next;
  element->next->prev = element->prev;
  return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
  if(list == NULL || key == NULL)
    return NULL;
  if(opt_yield & LOOKUP_YIELD)
    sched_yield();
  SortedListElement_t *ptr = list->next;
  while(ptr != list) {
    if((ptr->prev->next != ptr) || (ptr->next->prev != ptr))
      return NULL;
    if(strcmp(ptr->key, key) == 0)
      return ptr;
    ptr = ptr->next;
  }
  return NULL;
}

int SortedList_length(SortedList_t *list) {
  if(list == NULL)
    return -1;
  SortedListElement_t *ptr = list->next;
  int length = 0;
  if(opt_yield & LOOKUP_YIELD)
    sched_yield();
  while(ptr != list) {
    if((ptr->prev->next != ptr) || (ptr->next->prev != ptr))
      return -1;
    length++;
    ptr = ptr->next;
  }
  return length;
}
