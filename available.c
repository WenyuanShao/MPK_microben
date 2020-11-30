#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <assert.h>
#include "MPK_helper.h"

int main(void)
{
	int          status;
	int          pkey;
	unsigned int rdkey;
	int         *buffer;

	/*
	 *Allocate one page of memory
	 */
	buffer = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (buffer == MAP_FAILED)
		errExit("mmap");

	*buffer = __LINE__;
	printf("buffer contains: %d\n", *buffer);

	/*
	 * Allocate a protection key:
	 */
	rdkey= test_rdpkru();
	printf("rdpkru: 0x%x\n", rdkey);

	pkey = _pkey_alloc();
	printf("pkey: %d\n", pkey);

	if (pkey == -1)
		errExit("pkey_alloc");

	/*
	 * Disable access to any memory with "pkey" set,
	 * even though there is none right now
	 */
	_pkey_set(pkey, PKEY_DISABLE_ACCESS, 0);

	/*
     * Set the protection key on "buffer".
	 * Note that it is still read/write as far as mprotect() is
	 * concerned and the previous pkey_set() overrides it.
	 */
	status = _pkey_mprotect(buffer, getpagesize(), PROT_READ | PROT_WRITE, pkey);
	if (status == -1)
		errExit("pkey_mprotect");

	printf("about to read buffer again...\n");

	/*
	 * This will crash, because we have disallowed access
	 */
	printf("buffer contains: %d\n", *buffer);

	status = _pkey_free(pkey);
	if (status == -1)
		errExit("pkey_free");

	exit(EXIT_SUCCESS);
	return 0;
}