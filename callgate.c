#include "MPK_helper.h"
#include <assert.h>
#include <stdlib.h>

static inline unsigned long long
get_token(void)
{
	unsigned long long ret = ((unsigned long long)rand() << 32) | rand();
}

static inline void
callgate_abuse(void)
{
	printf("callgate_abuse\n");
	assert(0);
}

static inline void
callgate(void)
{
	unsigned long long token = get_token();
	unsigned int pkey = 2;

	__asm__ __volatile__("movq %[token], %%r15\n\t"
						 "xor %%rcx, %%rcx\n\t"
						 "xor %%rdx, %%rdx\n\t"
						 "cmp %[token], %%r15\n\t"
						 "jne callgate_abuse\n\t"
						 "movl %[pkey], %%eax\n\t"
						 "wrpkru"
						 :
						 : [token] "r" (token), [pkey] "r" (pkey)
						 :);
}

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
	//assert(status > 0);
	printf("read buffer: %d\n", *buffer);
	
	callgate();

	_pkey_set(ckey, 0, 0);
	_pkey_set(skey, PKEY_DISABLE_ACCESS, 0);
	printf("read buffer again: %d\n", *buffer);
	exit(EXIT_SUCCESS);
	return 0;
}
