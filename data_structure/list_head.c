#include "data_structure/list_head.h"

void init_list_head (list_head *head) {
    head->next = head;
    head->prev = head;
}

void __list_add (list_head *new_node, list_head *prev, list_head *next) {
    next->prev = new_node;
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;
}

void list_add_front (list_head *node, list_head *head) {
    __list_add (node, head, head->next);
}

void list_add_back (list_head *node, list_head *head) {
    __list_add (node, head->prev, head);
}

void __list_del (list_head *prev, list_head *next) {
    prev->next = next;
    next->prev = prev;
}
void list_del (list_head *node) {
    __list_del (node->prev, node->next);
    node->prev = node->next = NULL;
}

void list_move_front (list_head* node, list_head* head) {
    list_del (node);
    list_add_front (node, head);
}

void list_move_back (list_head *node, list_head *head) {
    list_del (node);
    list_add_back (node, head);
}

void list_rotate_left (list_head *head) {
    list_head *first;
    if (!list_empty (head)) {
        first = head->next;
        list_move_back (first, head);
    }
}

void list_replace (list_head *old, list_head *new_node) {
    new_node->next = old->next;
    new_node->next->prev = new_node;
    new_node->prev = old->prev;
    new_node->prev->next = new_node;
}

void list_replace_init (list_head *old, list_head *new_node) {
    list_replace (old, new_node);
    init_list_head (old);
}

int list_empty (const list_head *head) {
    return head->next == head;
}

int list_is_singular (const list_head *head) {
    return !list_empty (head) && (head->next == head->prev);
}

