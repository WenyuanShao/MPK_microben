#include "MPK_helper.h"
#include <assert.h>
#include <stdlib.h>

int pkey[2];

static inline unsigned long long
get_token(void)
{
	unsigned long long ret = ((unsigned long long)rand() << 32) | rand();
}

void
callgate_abuse(void)
{
	printf("callgate_abuse\n");
	assert(0);
	return;
}

#define S_KEY 1
#define C_KEY 2

static inline void
callgate()
{
	unsigned long long caller_addr;
	unsigned long long token = get_token();
	unsigned int pkru = (0 << (2 * S_KEY));
	pkru = (PKEY_DISABLE_ACCESS << (2 * C_KEY));
	//printf("pkru: 0x%x\n", pkru);

	/* 
	 * Composite version should rely on stubs to save current state of
	 * a thread. As a result, in this prototype, I don't consider it.
	 */
	__asm__ __volatile__("movq %[token], %%r15\n\t"
						 "xor %%rcx, %%rcx\n\t"
						 "xor %%rdx, %%rdx\n\t"
						 "movq %%rsp, %[caller_addr]\n\t"
						 "movl %[pkru], %%eax\n\t"
						 "wrpkru\n\t"
						 "cmp %[token], %%r15\n\t"
						 "jne 1f\n\t"
						 "call push_invstk\n\t"
						 "jmp 2f\n\f"
						 "1:\n\t"
						 "call callgate_abuse\n\t"
						 "2:"
						 : [caller_addr] "=rm" (caller_addr)
						 : [pkru] "rm" (pkru), [token] "rm" (token)
						 :);
}

int
init(int *buffer) {
	int status;
	int key = _pkey_alloc();

	assert(key >= 0);
	buffer = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (buffer == MAP_FAILED)
		errExit("mmap");

	*buffer = __LINE__;
	_pkey_set(key, 0, 0);
	status = _pkey_mprotect(buffer, getpagesize(), PROT_READ | PROT_WRITE, key);
	assert(status >= 0);

	return key;
}

void
client_call(int *s_buffer)
{
	callgate();
	printf("read buffer: %d\n", *s_buffer);
}

int
main(void)
{
	int status;
	int skey, ckey;
	int *s_buffer, *c_buffer;

	pkey[0] = init(s_buffer);
	pkey[1] = init(c_buffer);

	client_call(s_buffer);

	return 0;
}
