#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>

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
	unsigned int eax = 0;
	unsigned int ecx = 0;
	unsigned int edx = 0;

	__asm__ __volatile__("xor %%ecx, %%ecx\n\t"
                         "rdpkru"
                         : "=a" (eax), "=d" (edx)
                         :
                         :);

	return eax;
}

/*void
_pkey_set(int pkey, unsigned long rights, unsigned long flags)
{
	unsigned int pkru = (rights << (2 * pkey));
	wrpkru(pkru);
}

int
_pkey_mprotect(void *ptr, size_t size, unsigned long orig_prot,
            unsigned long pkey)
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
}*/

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

int
main(void)
{
	int          status;
	int          pkey;
	unsigned int rdkey;
	int         *buffer;

	/*
	 *Allocate one page of memory
	 */
	buffer = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (buffer == MAP_FAILED)
		errExit("mmap");

	*buffer = __LINE__;
	printf("buffer contains: %d\n", *buffer);

	/*
	 * Allocate a protection key:
	 */
	rdkey= test_rdpkru();
	printf("rdpkru: %u\n", rdkey);
/*	pkey = _pkey_alloc();
	printf("pkey: %d\n", pkey);
	rdkey= test_rdpkru();
	printf("rdpkru: %u\n", rdkey);
	if (pkey == -1)
		errExit("pkey_alloc");
*/
	/*
	 * Disable access to any memory with "pkey" set,
	 * even though there is none right now
	 */
	//_pkey_set(pkey, PKEY_DISABLE_ACCESS, 0);
	//if (status)
	//	errExit("pkey_set");

	/*
     * Set the protection key on "buffer".
	 * Note that it is still read/write as far as mprotect() is
	 * concerned and the previous pkey_set() overrides it.
	 */
	//status = _pkey_mprotect(buffer, getpagesize(), PROT_READ | PROT_WRITE, pkey);
	//if (status == -1)
	//	errExit("pkey_mprotect");

	printf("about to read buffer again...\n");

	/*
	 * This will crash, because we have disallowed access
	 */
	printf("buffer contains: %d\n", *buffer);

/*	status = _pkey_free(pkey);
	if (status == -1)
		errExit("pkey_free");
*/
	exit(EXIT_SUCCESS);
	return 0;
}
