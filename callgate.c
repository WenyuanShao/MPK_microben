#include "MPK_helper.h"
#include <assert.h>
#include <stdlib.h>

struct record {
	unsigned long long sp;
	unsigned long long ip;
};

struct stack {
	unsigned long long top;
	unsigned long long pad;
	struct record r[7];
};

int pkey[2];
static unsigned long long token  = 1;
static unsigned long pkru_invstk = 0;
static unsigned long pkru_callee = 0;
static unsigned long long tid    = 0;
static struct stack s[8];
unsigned long long verifier;

void
callgate_abuse(void)
{
	printf("callgate_abuse\n");
	assert(0);
	return;
}

void
caller_func(void)
{
	//printf("push invstk\n");
	return;
}

static inline void
callgate()
{
	/* 
	 * Composite version should rely on stubs to save current state of
	 * a thread. As a result, in this prototype, I don't consider it.
	 */
	unsigned long long e, s;
	printf("top: %llu\n", s[0].r[s[0].top]);
	s = mpk_tsc();
	__asm__ __volatile__("movq $token, %%r15\n\t"
						 "xor %%rcx, %%rcx\n\t"
						 "xor %%rdx, %%rdx\n\t"
						 "movq %%rsp, %0\n\t"
						 "movl $pkru_invstk, %%eax\n\t"
						 "wrpkru\n\t"
						 "movq $tid, %%rax\n\t"
						 "movq (%%rax), %%rax\n\t"
						 "shl $0x7, %%rax\n\t"
						 "add $s, %%rax\n\t"
						 "movq %%rax, %%rcx\n\t"
						 "movq (%%rax), %%rdx\n\t"
						 "shl $0x4, %%rdx\n\t"
						 "add %%rdx, %%rax\n\t"
						 "add $16, %%rax\n\t"
						 "movq %%rsp, (%%rax)\n\t"
						 "add $1, (%%rcx)\n\t"
						 "xor %%rcx, %%rcx\n\t"
						 "xor %%rdx, %%rdx\n\t"
						 "movl $pkru_callee, %%eax\n\t"
						 "wrpkru\n\t"
						 "cmp $token, %%r15\n\t"
						 "jne 1f\n\t"
						 //"call caller_func\n\t"
						 "jmp 2f\n\t"
						 "1:\n\t"
						 "call callgate_abuse\n\t"
						 "2:"
						 : "=r" (verifier)
						 :
						 : "memory", "cc");
	e = mpk_tsc();
	printf("overhead: %llu\n", e-s);
	printf("top: %llu\n", s[0].r[s[0].top]);
}

int
init(int **buffer) {
	int status;
	int key = _pkey_alloc();

	assert(key >= 0);
	*buffer = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (*buffer == MAP_FAILED)
		errExit("mmap");

	**buffer = __LINE__;
	_pkey_set(key, 0, 0);
	status = _pkey_mprotect(*buffer, getpagesize(), PROT_READ | PROT_WRITE, key);
	assert(status >= 0);
//	_pkey_set(key, PKEY_DISABLE_ACCESS, 0);

	return key;
}

void
client_call(int *s_buffer)
{
	callgate();
	printf("expr read buffer: %d\n", *s_buffer);
}

int
main(void)
{
	int *s_buffer, *c_buffer;

	pkey[0] = init(&s_buffer);
	pkey[1] = init(&c_buffer);

	//_pkey_set(pkey[0], PKEY_DISABLE_ACCESS, 0);
	client_call(s_buffer);

	return 0;
}
