#if !defined(SDB_PAGER_H)
#define SDB_PAGER_H

#include <inttypes.h>
#include "trees.h"

typedef struct {
    int fd;
    uint32_t f_size;
    uint32_t num_pages;
    void *pages[TABLE_MAX_PAGES];
} SDBPager;


SDBPager *sdb_pager_open(const char *filename);

void *sdb_get_page(SDBPager *pager, uint32_t page_num);

void sdb_pager_flush(SDBPager *pager, uint32_t page_num);

#endif // SDB_PAGER_H
