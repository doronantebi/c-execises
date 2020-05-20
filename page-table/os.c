#define _GNU_SOURCE

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <sys/mman.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include "os.h"
#include "SPBufferset.h"

/* 2^20 pages ought to be enough for anybody */
#define NPAGES	(1024*1024)

static void *pages[NPAGES];

uint64_t alloc_page_frame(void) {
	static uint64_t nalloc;
	uint64_t ppn;
	void *va;

	if (nalloc == NPAGES)
		errx(1, "out of physical memory");

	/* OS memory management isn't really this simple */
	ppn = nalloc;
	nalloc++;

	va = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
			-1, 0);
	if (va == MAP_FAILED)
		err(1, "mmap failed");

	pages[ppn] = va;
	return ppn;
}

void* phys_to_virt(uint64_t phys_addr) {
	uint64_t ppn = phys_addr >> 12;
	uint64_t off = phys_addr & 0xfff;
	void *va = NULL;
    if (ppn < NPAGES) {
        va = pages[ppn] + off;
    }
	return va;
}
uint64_t get_random_vpn() {
	return rand() & 0x1FFFFFFFFFFF; /*/ 45 bits*/
}

uint64_t get_random_ppn() {
	return rand() & 0xFFFFFFFFFFFFF; /* 13 'F's*/
}

void update_and_check(uint64_t pt, uint64_t vpn, uint64_t ppn) {
	 /*printf("updating [%llX] = %llX\n", vpn, ppn);*/
	page_table_update(pt, vpn, ppn);
	assert(page_table_query(pt, vpn) == ppn);
}

void perform_random_move(uint64_t pt) {
	uint64_t vpn = get_random_vpn();
	uint64_t ppn = get_random_ppn();

	if (rand() % 10 < 3)
		ppn = NO_MAPPING;

	update_and_check(pt, vpn, ppn);
}

int main(int argc, char **argv) {

	uint64_t pt = alloc_page_frame();

	assert(page_table_query(pt, 0xcafe) == NO_MAPPING);
	printf("1 is ok\n");
	page_table_update(pt, 0xcafe, 0xf00d);
	printf("update ended\n");
	assert(page_table_query(pt, 0xcafe) == 0xf00d);
    printf("2 is ok\n");
	page_table_update(pt, 0xcafe, NO_MAPPING);
	assert(page_table_query(pt, 0xcafe) == NO_MAPPING);
    printf("3 is ok\n");
	page_table_update(pt, 0x0, 0x55f4);
	assert(page_table_query(pt, 0) == 0x55f4);
    printf("4 is ok\n");
	page_table_update(pt, 0xfffffffffffff, 0x55f4);
	printf("after update\n");
	assert(page_table_query(pt, 0xfffffffffffff) == 0x55f4);
    printf("5 is ok\n");
	page_table_update(pt, 0x5cafe, NO_MAPPING);
	assert(page_table_query(pt, 0x5cafe) == NO_MAPPING);
    printf("6 is ok\n");
	assert(page_table_query(pt, 0xc54ae) == NO_MAPPING);
    printf("7 is ok\n");
	page_table_update(pt, 0xffffff, 0x55df4);
	page_table_update(pt, 0xffffff, 0x55af4);
	assert(page_table_query(pt, 0xffffff) == 0x55af4);
    printf("8 is ok\n");
	page_table_update(pt, 0x0, 0x55dfa);
	page_table_update(pt, 0x1, 0x55dfb);
	page_table_update(pt, 0x2, 0x55dfc);
	page_table_update(pt, 0x1ff, 0x55dfd);
	assert(page_table_query(pt, 0x2) == 0x55dfc);
    printf("9 is ok\n");
	assert(page_table_query(pt, 0xcafe) == NO_MAPPING);
    printf("10 is ok\n");

	printf("got here\n");
	for (int i = 0; i <pow(2,10); i++) {
	    printf("i = %d\n", i);
        perform_random_move(pt);
    }
	printf("yessss");
	return 0;
}

