#ifndef _LIST_H
#define _LIST_H

#include <stddef.h>
#include <stdint.h>

#ifndef LIST_HEADER_DUMMY_SIZE
#define LIST_HEADER_DUMMY_SIZE 8
#endif

typedef struct _list_t {
	uint8_t dummy[LIST_HEADER_DUMMY_SIZE];
	struct _list_t *next;
} ListHeader_t;

typedef struct {
	ListHeader_t *live;
	ListHeader_t *free;
	uint8_t size;
} ListInfo_t;

extern void init_list(ListInfo_t *info, void *list, uint8_t size, uint8_t len);
extern void *alloc_list(ListInfo_t *info) __z88dk_fastcall;
extern void free_list(ListInfo_t *info, ListHeader_t *obj);

#endif // !_LIST_H
