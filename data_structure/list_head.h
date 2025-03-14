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

#include "yar_defs.h"

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
