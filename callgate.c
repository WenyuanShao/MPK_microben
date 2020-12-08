#include "MPK_helper.h"
#include <assert.h>
#include <stdlib.h>

static inline unsigned long long
get_token(void)
{
	unsigned long long ret = ((unsigned long long)rand() << 32) | rand();
}

void
callgate_abuse(void)
{
	printf("callgate_abuse\n");
	return;
}

#define S_KEY 1
#define C_KEY 2

static inline void
callgate(void)
{
	unsigned long long token = get_token();
	unsigned int pkru = (0 << (2 * C_KEY));
	pkru = (PKEY_DISABLE_ACCESS << (2 * S_KEY));
	printf("pkru: 0x%x\n", pkru);

	__asm__ __volatile__("movq %[token], %%r15\n\t"
						 "xor %%rcx, %%rcx\n\t"
						 "xor %%rdx, %%rdx\n\t"
						 "cmp %[token], %%r15\n\t"
						 "jne callgate_abuse\n\t"
						 "wrpkru\n\t"
						 //"0:"
						 //"call callgate_abuse"
						 :
						 : "a" (pkru), [token] "rm" (token)
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
	unsigned int rdpkru = test_rdpkru();
	printf("rdpkru after callgate: 0x%x\n", rdpkru);

	_pkey_set(ckey, 0, 0);
	_pkey_set(skey, PKEY_DISABLE_ACCESS, 0);
	rdpkru = test_rdpkru();
	printf("rdpkru after normal wrpkru: 0x%x\n", rdpkru);
	printf("read buffer again: %d\n", *buffer);
	exit(EXIT_SUCCESS);
	return 0;
}
