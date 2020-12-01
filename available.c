#include "MPK_helper.h"

int
main(void)
{
	int status;
	int pkey;
	int *buffer;

	buffer = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (buffer == MAP_FAILED)
		errExit("mmap");

	*buffer = __LINE__;
	printf("buffer contains: %d\n", *buffer);

	pkey = _pkey_alloc();
	printf("pkey: %d\n", pkey);
	if (pkey == -1)
		errExit("pkey_alloc");

	_pkey_set(pkey, PKEY_DISABLE_ACCESS, 0);

	status = _pkey_mprotect(buffer, getpagesize(), PROT_READ | PROT_WRITE, pkey);

	printf("read buffer again...\n");
	/* fail to read buffer again */
	printf("buffer contains: %d\n", *buffer);

	status = _pkey_free(pkey);
	if (status == -1)
		errExit("pkey_free");

	exit(EXIT_SUCCESS);
	return 0;
}
