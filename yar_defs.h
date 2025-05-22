#ifndef __YAR_DEFS_H
#define __YAR_DEFS_H

#define WRFILE_PERM             0644
#define RDFILE_PERM             0644

// #define container_of(ptr, type, member) ({ \
//         const typeof(((type *)0)->member) *__mptr = (ptr); \
//         (type *)( (char *)__mptr - offsetof(type, member) );})
//
//
// // List head iteration
// #define list_entry(ptr, type, member) container_of(ptr, type, member)
//
// #define list_for_each(pos, head) \
// 	for (pos = (head)->next; \
// 	     pos != (head); \
//          pos = pos->next)
//
// #define list_for_each_safe(pos, tmp, head) \
//     for (pos = (head)->next, tmp = pos->next; \
// 		 pos != (head); \
// 		 pos = tmp, tmp = pos->next)
//          
// #define list_for_each_prev(pos, head) \
// 	for (pos = (head)->prev; \
// 	     pos != (head); \
//          pos = pos->prev)
//          
// #define list_for_each_prev_safe(pos, tmp, head) \
//     for (pos = (head)->prev, tmp = pos->prev; \
//          pos != (head); \
//          pos = tmp, tmp = pos->prev)

#endif
