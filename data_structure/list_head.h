/**
 * @file
 * @brief Linked list implementation
 *
 *  You can use list_head to create linked list with any data type
 *  https://www.programmersought.com/article/54236337207/
 */

#ifndef __LIST_H
#define __LIST_H

#include <stddef.h>

#define container_of(ptr, type, member) ({ \
        const typeof(((type *)0)->member) *__mptr = (ptr); \
        (type *)( (char *)__mptr - offsetof(type, member) );})

// List head iteration
#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define list_for_each(pos, head) \
	for (pos = (head)->next; \
	     pos != (head); \
         pos = pos->next)

#define list_for_each_safe(pos, tmp, head) \
    for (pos = (head)->next, tmp = pos->next; \
		 pos != (head); \
		 pos = tmp, tmp = pos->next)
         
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; \
	     pos != (head); \
         pos = pos->prev)
         
#define list_for_each_prev_safe(pos, tmp, head) \
    for (pos = (head)->prev, tmp = pos->prev; \
         pos != (head); \
         pos = tmp, tmp = pos->prev)

struct list_head {
    struct list_head *next;
    struct list_head *prev;
};
typedef struct list_head list_head;

void init_list_head (list_head *head);

int list_empty (const list_head *head);

void list_add_front (list_head *node, list_head *head);

void list_add_back (list_head *node, list_head *head);

void list_del (list_head *node);

void list_move_front (list_head* node, list_head* head);

void list_move_back (list_head *node, list_head *head);

void list_rotate_left (list_head *head);

void list_replace (list_head *old, list_head *new_node);

void list_replace_init (list_head *old, list_head *new_node);

int list_empty (const list_head *head);

int list_is_singular (const list_head *head);

#endif
