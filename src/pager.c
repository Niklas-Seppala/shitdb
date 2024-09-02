#include "pager.h"
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "trees.h"

SDBPager *sdb_pager_open(const char *filename) {
    if (access(filename, F_OK) == 0) {
        printf("[OPEN] database %s\n", filename);
    } else {
        printf("[CREATE] database %s\n", filename);
    }
    
    int fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
    if (fd == -1) {
        perror("Failed to open database file");
        exit(EXIT_FAILURE);
    }

    SDBPager *pager = calloc(1, sizeof(*pager));
    pager->fd = fd;
    off_t file_length = lseek(fd, 0, SEEK_END);;
    pager->f_size = file_length;
    pager->num_pages = (file_length / PAGE_SIZE);

    if (file_length % PAGE_SIZE != 0) {
        printf("Db file is not a whole number of pages. Corrupt file.\n");
        exit(EXIT_FAILURE);
    }
    
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pager->pages[i] = NULL;
    }
    return pager;
}

void *sdb_get_page(SDBPager *pager, uint32_t page_num) {
    if (page_num > TABLE_MAX_PAGES) {
        printf("Tried to fetch a page %d outside of bounds (%d)\n", page_num, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (pager->pages[page_num] == NULL) {
        // Cache miss
        void *page = malloc(PAGE_SIZE);
        uint32_t num_pages = pager->f_size / PAGE_SIZE;

        if (pager->f_size % PAGE_SIZE) {
            num_pages++;
        }

        if (page_num < num_pages) {
            lseek(pager->fd, page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager->fd, page, PAGE_SIZE);
            if (bytes_read == -1) {
                perror("[ERROR] reading file");
                exit(EXIT_FAILURE);
            }
        }
        pager->pages[page_num] = page;

        if (page_num >= pager->num_pages) {
            pager->num_pages++;
        }
    }

    return pager->pages[page_num];
}


void sdb_pager_flush(SDBPager *pager, uint32_t page_num) {
    assert(pager != NULL);

    if (pager->pages[page_num] == NULL) {
        printf("Attempt to flush NULL page at offset %d * %d", page_num, PAGE_SIZE);
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->fd, page_num * PAGE_SIZE, SEEK_SET);
    if (offset == -1) {
        perror("[ERROR] Failed seeking offset");
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_written = write(pager->fd, pager->pages[page_num], PAGE_SIZE);
    if (bytes_written == -1) {
        perror("[ERROR] flushing the page");
        exit(EXIT_FAILURE);
    }
}
