#include <stdio.h>

#define MAXSIZE 10
static unsigned long T = 111;

struct record {
	unsigned long long sp;
	unsigned long long ip;
};

struct stack {
	unsigned long long top;
	struct record r[10];
	//int r[10];
};

void
if_test(int token)
{
	if (token == T) {
		token += 2;	
	} else {
		token += 1;
	}
}

static int aaaa  = 1;
static struct stack s[2];

void test (void)
{
	int  i =0;
	int j = 0;
	s[i].r[j].sp = 0x2222;
	s[i].r[j].ip = 0x3333;
	s[1].top++;
	printf("s: %p\n", &s);
	printf("s: %d\n", aaaa);
	
	return 0;
}

int main(void) {
	test();
	return 0;
}
