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
}__attribute__((aligned));

int pkey[2];
static unsigned long long token  = ~(unsigned long long)0;
static unsigned long pkru_userlevel_kernel = 0;
static unsigned long pkru_callee = 0xfffffff0;
static unsigned long pkru_caller = 0xffffffcc;
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
	unsigned long long end, start;
	start = mpk_tsc();
	__asm__ __volatile__("movq $0xfffffffffffffff0, %%r15\n\t" // caller_token
	                     "xor %%rcx, %%rcx\n\t"
	                     "xor %%rdx, %%rdx\n\t"
	                     "movq %%rsp, %0\n\t"
	                     "movl $pkru_userlevel_kernel, %%eax\n\t"
                         "wrpkru\n\t"
	                     "movq $0x1, %%rax\n\t" // tid = 0x1
	                     "shl $0x7, %%rax\n\t"
	                     "movq $s, %%rcx\n\t"
	                     "lea 0x0(%%rcx, %%rax, 1), %%rdx\n\t" // get per thread invstk
	                     "movq (%%rdx), %%rcx\n\t"
	                     "shl $0x4, %%rcx\n\t"
	                     "lea 0x10(%%rdx, %%rcx, 1), %%rax\n\t" // get head
	                     "movq %%rsp, (%%rax)\n\t" // save sp
	                     "add $0x8, %%rax\n\t"
	                     "lea (%%rip), %%rcx\n\t"
	                     "movq %%rcx, (%%rax)\n\t" //save ip
	                     "add $0x1, (%%rdx)\n\t"
	                     "xor %%rcx, %%rcx\n\t"
	                     "xor %%rdx, %%rdx\n\t"
	                     "movl $pkru_callee, %%eax\n\t"
	                     "wrpkru\n\t"
	                     "cmp $0xfffffffffffffff0, %%r15\n\t" // caller_token check
	                     "jne 1f\n\t"
	                   //"call caller_func\n\t"
	                     "jmp 2f\n\t"
	                     "1:\n\t"
	                     "call callgate_abuse\n\t"
	                     "2:\n\t"
	                     "movq $0xfffffffffffffff1, %%r15\n\t" // callee_token
	                     "xor %%rcx, %%rcx\n\t"
	                     "xor %%rdx, %%rdx\n\t"
	                     "movl $pkru_userlevel_kernel, %%eax\n\t"
	                     "wrpkru\n\t"
	                     "movq $0x1, %%rax\n\t" // tid = 0x0
	                     "shl $0x7, %%rax\n\t"
	                     "add $s, %%rcx\n\t"
	                     "lea 0x0(%%rcx, %%rax, 1), %%rdx\n\t"
	                     //"movq %%rax, %%rcx\n\t"
	                     "movq (%%rdx), %%rcx\n\t"
	                     "shl $0x4, %%rcx\n\t"
	                     "lea 0x10(%%rdx, %%rcx, 1), %%rax\n\t"
	                    // "movq (%%rax), %%rsp\n\t"
	                     "add $0x8, %%rax\n\t"
	                    // "movq (%%rax), %%rip\n\t"
	                     "sub $1, (%%rdx)\n\t"
	                     "xor %%rcx, %%rcx\n\t"
	                     "xor %%rdx, %%rdx\n\t"
	                     "movl $pkru_caller, %%eax\n\t"
	                     "wrpkru\n\t"
	                     "cmp $0xfffffffffffffff1, %%r15\n\t" // callee_token check
	                     "jne 3f\n\t"
	                   //"call callee_func\n\t"
	                     "jmp 4f\n\t"
	                     "3:\n\t"
	                     "call callgate_abuse\n\t"
	                     "4:"
	                     : "=r" (verifier)
	                     :
	                     : "memory", "cc");
	end = mpk_tsc();
	printf("overhead: %llu\n", (end-start)/100);
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
	_pkey_set(key, PKEY_DISABLE_ACCESS, 0);

	return key;
}

void
client_call(int *s_buffer, int *c_buffer)
{
	callgate();
	//printf("expr read buffer: %d\n", *c_buffer);
}

int
main(void)
{
	int *s_buffer, *c_buffer;
	unsigned long pkru = 0xfffffffc;

	printf("PKEY_DISABLE_ACCESS:%d\n", PKEY_DISABLE_ACCESS);
	pkey[0] = init(&s_buffer);
	//pkru = test_rdpkru();
	//printf("pkru: %x, pkey: %d\n", pkru, pkey[0]);
	pkey[1] = init(&c_buffer);
	wrpkru(pkru);
	pkru = test_rdpkru();
	printf("pkru: %x, pkey: %d\n", pkru, pkey[1]);
	//printf("expr read buffer: %d\n", *s_buffer);

	//_pkey_set(pkey[0], PKEY_DISABLE_ACCESS, 0);
	client_call(s_buffer, c_buffer);

	return 0;
}
