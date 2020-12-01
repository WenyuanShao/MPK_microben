#include "MPK_helper.h"
#include <assert.h>

#define TEST_LEN 1000

int
main(void)
{
	int status;
	int pkey;
	int *buffer;
	unsigned long long e, s, r;
	unsigned int rd_pkru, pkru;
	int  i = 0;

	buffer = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (buffer == MAP_FAILED)
		errExit("mmap");

	*buffer = __LINE__;
	printf("buffer contains: %d\n", *buffer);

	s = mpk_tsc();
	for (i = 0; i < TEST_LEN; i++) {
		rd_pkru = test_rdpkru();
	}
	e = mpk_tsc();
	r = (e-s)/TEST_LEN;
	printf("rdpkru cost: %llu cycles\n", r);

	r = (unsigned long long)0;
	for ( i = 0; i < TEST_LEN; i++) {
		s = mpk_tsc();
		pkey = _pkey_alloc();
		e = mpk_tsc();
		r += (e-s);
		assert(pkey > 0);
		_pkey_free(pkey);
	}
	r /= TEST_LEN;
	printf("pkey alloc cost: %llu cycles\n", r);

	pkey = _pkey_alloc();
	assert(pkey > 0);
	printf("pkey: %d\n", pkey);
	pkru = (PKEY_DISABLE_ACCESS << (2 * pkey));

	s = mpk_tsc();
	for (i = 0; i < TEST_LEN; i++) {
		test_wrpkru(pkru);
	}
    e = mpk_tsc();
	r = (e-s)/TEST_LEN;
	printf("wrpkru cost: %llu cycles\n", r);

	rd_pkru = test_rdpkru();
	printf("rdpkru: 0x%x, wrpkru: 0x%x\n", rd_pkru, pkru);

	status = _pkey_mprotect(buffer, getpagesize(), PROT_READ | PROT_WRITE, pkey);

	printf("about to read buffer again...\n");

	printf("buffer contains: %d\n", *buffer);

	status = _pkey_free(pkey);
	if (status == -1)
		errExit("pkey_free");

	exit(EXIT_SUCCESS);
	return 0;
}
