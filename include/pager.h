#if !defined(SQUEEL_PAGER_H)
#define SQUEEL_PAGER_H

#include <inttypes.h>
#define TABLE_MAX_PAGES 100
#define PAGE_SIZE ((uint32_t)4096)

typedef struct {
    int fd;
    uint32_t f_size;
    void *pages[TABLE_MAX_PAGES];
} SqueelPager;


SqueelPager *squeel_pager_open(const char *filename);

void *squeel_get_page(SqueelPager *pager, uint32_t page_num);

void squeel_pager_flush(SqueelPager *pager, uint32_t page_num, uint32_t nBytes);

#endif // SQUEEL_PAGER_H
