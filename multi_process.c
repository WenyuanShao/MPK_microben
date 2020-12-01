#include "MPK_helper.h"
#include <assert.h>

int
main(void)
{
	int status;
	int pkey;
	int *buffer;
	int pid;

	buffer = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (buffer == MAP_FAILED)
		errExit("mmap");

	*buffer = __LINE__;
	printf("buffer contains: %d\n", *buffer);
	
	pid = fork();
	assert(pid >= 0);
	if (pid != 0) {
		pkey = _pkey_alloc();
		printf("pkey: %d\n", pkey);
		if (pkey == -1)
			errExit("pkey_alloc");

		_pkey_set(pkey, 0, 0);

		status = _pkey_mprotect(buffer, getpagesize(), PROT_READ | PROT_WRITE, pkey);

		//printf("about to read buffer again...\n");

		//status = _pkey_free(pkey);
		//if (status == -1)
		//	errExit("pkey_free");
	} else {
		pkey = _pkey_alloc();
		printf("pkey: %d\n", pkey);
		_pkey_set(pkey, 0, 0);
		/*while(1) {
			printf("buffer contains: %d\n", *buffer);
		}*/	
	}
	exit(EXIT_SUCCESS);
	return 0;
}
