#include "MPK_helper.h"
#include <assert.h>

int
main(void)
{
	int status;
	int skey, ckey;
	int *buffer;
	int pid;

	buffer = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (buffer == MAP_FAILED)
		errExit("mmap");

	*buffer = __LINE__;
	printf("buffer contains: %d\n", *buffer);
	
	skey = _pkey_alloc();
	ckey = _pkey_alloc();
	printf("pkey: %d\n", skey);
	printf("pkey: %d\n", ckey);
	/*if (pkey == -1)
		errExit("pkey_alloc");

	_pkey_set(pkey, 0, 0);

	status = _pkey_mprotect(buffer, getpagesize(), PROT_READ | PROT_WRITE, pkey);

	//printf("about to read buffer again...\n");

	//status = _pkey_free(pkey);
	//if (status == -1)
	//	errExit("pkey_free");
	}*/
	exit(EXIT_SUCCESS);
	return 0;
}
