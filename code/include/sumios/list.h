#ifndef SUMIOS_LIST_H
#define SUMIOS_LIST_H

#include <sumios/kernel.h>

struct list_head {
    struct list_head *next;
    struct list_head *prev;
};

#define list_entry(ptr, type, member) container_of(ptr, type, member)


static inline void __list_add_prev(struct list_head *entry, 
                                   struct list_head *prev)
{
    entry->prev->next = prev;
    prev->prev = entry->prev;
    prev->next = entry;
    entry->prev = prev;
}

/* add a new node to the list as entry->prev */
static inline void list_add_prev(struct list_head *entry, 
                                   struct list_head *prev)
{
    __list_add_prev(entry, prev);
}

static inline void __list_add_next(struct list_head *entry, 
                                   struct list_head *next)
{
    entry->next->prev = next;
    next->prev = entry;
    next->next = entry->next;
    entry->next = next;
}

/* add a new node to the list as entry->next */
static inline void list_add_next(struct list_head *entry, 
                                   struct list_head *next)
{
    __list_add_next(entry, next);
}

static inline void __list_del(struct list_head *entry)
{
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
}

/* unlink a node from its list */
static inline void list_del(struct list_head *entry)
{
    __list_del(entry);
}

#endif