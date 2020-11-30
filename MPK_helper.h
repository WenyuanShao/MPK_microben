#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>

static inline unsigned long long
mpk_tsc(void)
{
	unsigned int a, d, c;

	__asm__ __volatile__("rdtsc" : "=a" (a) , "=d" (d), "=c" (c) : : );

	return ((unsigned long long)d << 32) | (unsigned long long)a;
}

static inline void
wrpkru(unsigned int pkru)
{
	unsigned int eax = pkru;
	unsigned int ecx = 0;
	unsigned int edx = 0;

	asm volatile(".byte 0x0f,0x01,0xef\n\t"
                : : "a" (eax), "c" (ecx), "d" (edx));
}

static inline unsigned int
test_rdpkru(void)
{
	unsigned int a;

	__asm__ __volatile__("xor %%ecx, %%ecx\n\t"
                         "rdpkru"
                         : "=a" (a)
                         :
                         :);

	return a;
}

void
_pkey_set(int pkey, unsigned long rights, unsigned long flags)
{
	unsigned int pkru = (rights << (2 * pkey));
	wrpkru(pkru);
}

int
_pkey_mprotect(void *ptr, size_t size, unsigned long orig_prot, unsigned long pkey)
{
	return syscall(SYS_pkey_mprotect, ptr, size, orig_prot, pkey);
}
 
int
_pkey_alloc(void)
{
	return syscall(SYS_pkey_alloc, 0, 0);
}

int
_pkey_free(unsigned long pkey)
{
	return syscall(SYS_pkey_free, pkey);
}

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)
