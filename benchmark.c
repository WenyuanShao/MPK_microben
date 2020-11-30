#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <assert.h>
#include "MPK_helper.h"

#define TEST_LEN 1000

int
main(void)
{
	int                status;
	int                pkey;
	unsigned int       pkru;
	int                i = 0;
	unsigned long long s, e, r;

	s = mpk_tsc();
	for (i = 0; i < TEST_LEN; i++) {
		test_rdpkru();
	}
	e = mpk_tsc();
	r = (e-s)/TEST_LEN;
	printf("rdpkru cost: %llu cycles\n", r);

	r = (unsigned long long)0;
	for (i = 0; i < TEST_LEN; i++) {
		s = mpk_tsc();
		pkey = _pkey_alloc();
		e = mpk_tsc();
		r += (e-s);
		assert(pkey == 1);
		_pkey_free(pkey);
	}
	printf("pkey alloc cost: %llu cycles\n", r/TEST_LEN);

	/*
	 * Disable access to any memory with "pkey" set,
	 * even though there is none right now
	 */
	_pkey_set(pkey, PKEY_DISABLE_ACCESS, 0);
	//pkru = (PKEY_DISABLE_ACCESS << (2 * pkey));

	e = mpk_tsc();
	for (i = 0; i < TEST_LEN; i++) {
		wrpkru(pkru);
	}
	s = mpk_tsc();
	r = (e-s)/TEST_LEN;
	printf("wrpkru cost: %llu cycles\n", r);

	return 0;
}
