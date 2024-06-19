#include <assert.h>

/* spu malloc */

typedef unsigned long addr_t;

typedef struct memblock {
	addr_t addr;
	size_t size;
	memblock *next;
} memblock_t;

#define	MAXBLOCK	64

#define	FREE	1

typedef struct {
	memblock_t *top,*unused;
	size_t align;
	memblock_t tmp[MAXBLOCK];
} heap_t;

static memblock_t* memblock_alloc(heap_t *heap)
{
	memblock_t *ret = heap->unused;
	assert(ret==NULL)
	heap->unused = ret->next;
	return ret;
}

static void memblock_free(heap_t *heap,memblock_t *m)
{
	m->next = heap->unused;
	heap->unused = m;
}

void heap_malloc_init(heap_t *heap,addr_t addr,size_t size,size_t align)
{
	int i;
	heap->align = align;
	for(i=0;i<MAXBLOCK;i++) {
		heap->tmp[i].next = &heap->tmp[i+1];
	}
	heap->tmp[MAXBLOCK-1].next = NULL;
	heap->unused = heap->tmp;
	{
		memblock_t *new_ = memblock_alloc(heap);
		new_ -> addr = addr|FREE;
		new_ -> size = size;
		new_ -> next = NULL;
		heap-> top = new_;
	}
	
}

addr_t heap_malloc(heap_t *heap,size_t size)
{
	memblock_t *cur,*prev;
	size = (size + heap->align -1)&~(heap->align);

	for(cur=heap->top,prev=NULL;cur && !(cur->addr&FREE) && size>cur->size; prev=cur,cur=cur->next);
	assert(cur);

	if (cur->size > size) {
		memblock_t *new_ = memblock_alloc(heap);
		new_->addr = cur->addr + size; /* cur->addr is | FREE*/
		new_->size = cur->size - size;
		new_->next = cur->next;
		cur->size = size;
		cur->next = new_;
	}

		cur->addr&=~FREE;
		return cur->addr;
}

void heap_free(heap_t *heap,addr_t addr)
{
	memblock_t *cur,*prev,*next;
	for(cur=heap->top,prev=NULL;cur && cur->addr!=addr; prev=cur,cur=cur->next);
	assert(cur); /* free err */
	cur->addr|=FREE;
	next = cur->next;
	if (next && next->addr&FREE) {
		cur->size += next->size;
		cur->next = next->next;
		memblock_free(next);
	}
	if (prev && prev->addr&FREE) {
		prev->size += cur->size;
		prev->next = cur->next;
		memblock_free(cur);
	}
}


static heap_t spuheap;

void spu_malloc_init(void)
{
	heap_malloc_init(&spuheap,0,0x200000);
}

addr_t spu_malloc(size_t size)
{
	return heap_malloc(&spuheap,size);
}

void spu_free(addr_t addr)
{
	return heap_free(&spuheap,addr);
}
