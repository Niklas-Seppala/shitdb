#if !defined(SDB_PAGER_H)
#define SDB_PAGER_H

#include <inttypes.h>
#include "btree.h"

typedef struct {
    int fd;
    uint32_t f_size;
    uint32_t num_pages;
    void *pages[TABLE_MAX_PAGES];
} SDBPager;


SDBPager *sdb_pager_open(const char *filename);

void *sdb_pager_get_page(SDBPager *pager, uint32_t page_num);

void sdb_pager_flush(SDBPager *pager, uint32_t page_num);

uint32_t sdb_pager_unused_page_num(SDBPager* pager);

#endif // SDB_PAGER_H
