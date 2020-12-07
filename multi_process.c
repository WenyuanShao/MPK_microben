#include "MPK_helper.h"
#include <assert.h>

int
main(void)
{
	int status;
	int skey, ckey;
	int *buffer;

	buffer = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (buffer == MAP_FAILED)
		errExit("mmap");

	*buffer = __LINE__;
	printf("buffer contains: %d\n", *buffer);
	
	skey = _pkey_alloc();
	assert(skey > 0);
	ckey = _pkey_alloc();
	assert(ckey > 0);
	printf("pkey: %d\n", skey);
	printf("pkey: %d\n", ckey);

	_pkey_set(skey, 0, 0);
	status = _pkey_mprotect(buffer, getpagesize(), PROT_READ | PROT_WRITE, skey);
	printf("read buffer: %d\n", *buffer);

	_pkey_set(ckey, 0, 0);
	//status = _pkey_mprotect(buffer, getpagesize(), PROT_READ | PROT_WRITE, skey);
	printf("read buffer again: %d\n", *buffer);
	//status = _pkey_free(pkey);
	//if (status == -1)
	//	errExit("pkey_free");
	exit(EXIT_SUCCESS);
	return 0;
}
