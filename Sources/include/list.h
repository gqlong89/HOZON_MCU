#ifndef __LIST_H__
#define __LIST_H__

#include <string.h>

typedef struct _list_t
{
    struct _list_t  *prev;
    struct _list_t  *next;
} list_t;

#define member_offset(type, member)     ((unsigned int)&(((type*)0)->member))
#define list_entry(node, type, member)  ((type*)((unsigned char*)(node) - member_offset(type, member)))


static inline void list_init(list_t *list)
{
    list->prev = list->next = list;
}

static inline void list_insert_before(list_t *list, list_t *node)
{
    node->prev = list->prev;
    node->next = list;
    list->prev->next = node;
    list->prev = node;
}

static inline void list_insert_after(list_t *list, list_t *node)
{
    node->next = list->next;
    node->prev = list;
    list->next->prev = node;
    list->next = node;
}

static inline void list_delete(list_t *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

static inline int list_empty(list_t *list)
{
    return (list->next == list || list->prev == list);
}

static inline int list_count(list_t *list)
{
    list_t *node;
    int cnt;

    for (cnt = 0, node = list->next; node != list; node = node->next)
    {
        cnt++;
    }

    return cnt;
}

static inline list_t *list_get_first(list_t *list)
{
    list_t *ret = NULL;

    if (!list_empty(list))
    {
        ret = list->next;
        list_delete(ret);
    }

    return ret;
}

static inline list_t *list_get_last(list_t *list)
{
    list_t *ret = NULL;

    if (!list_empty(list))
    {
        ret = list->prev;
        list_delete(ret);
    }

    return ret;
}

#endif
