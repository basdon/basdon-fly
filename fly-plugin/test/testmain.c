#include <alloca.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TRACE

#define STATIC_ASSERT(E) typedef char __static_assert_[(E)?1:-1]
#define EXPECT_SIZE(S,SIZE) STATIC_ASSERT(sizeof(S)==(SIZE))
EXPECT_SIZE(char, 1);
EXPECT_SIZE(short, 2);
EXPECT_SIZE(int, 4);
EXPECT_SIZE(float, 4);

char *_test_suite, *_test_name;

void _test_exit_failure()
{
	if (_test_name[0]) {
		printf("failed test: %s: %s\n", _test_suite, _test_name);
	}
	exit(1);
}

#define _assert(X) _assert_(__FILE__,__LINE__,X)
static
__attribute__((unused))
void _assert_(char *file, int line, int condition)
{
	if (!condition) {
		printf("failed: _assert at %s:%d\n", file, line);
		_test_exit_failure();
	}
}

#define _assert_equals(X,Y) _assert_equals_(__FILE__,__LINE__,X,Y)
static
void _assert_equals_(char *file, int line, int a, int b)
{
	if (a != b) {
		printf("failed: _assert_equals at %s:%d, expected %d got %d\n", file, line, a, b);
		_test_exit_failure();
	}
}

#define _assert_strcmp(X,Y) _assert_strcmp_(__FILE__,__LINE__,X,Y)
static
__attribute__((unused))
void _assert_strcmp_(char *file, int line, char *a, char *b)
{
	if (strcmp(a, b)) {
		printf("failed: _assert_strcmp at %s:%d\n>>>\n%s\n===\n%s\n<<<\n", file, line, a, b);
		_test_exit_failure();
	}
}

#define _assert_strcmpex(X,Y) _assert_strcmpex_(__FILE__,__LINE__,X,Y)
static
void _assert_strcmpex_(char *file, int line, char *a, char *b)
{
	char *zero_terminated_b;
	int len, i;

	len = strlen(a);
	if (strncmp(a, b, len)) {
		zero_terminated_b = alloca(len + 1);
		zero_terminated_b[len] = 0;
		for (i = 0; i < len; i++) {
			if (!(zero_terminated_b[i] = b[i])) {
				break;
			}
		}
		printf("failed: _assert_strcmpex at %s:%d\n>>> expected\n%s\n=== actual\n%s\n<<<\n", file, line, a, zero_terminated_b);
		_test_exit_failure();
	}
}

#include "test_splitclientmessage.c"

int main(int argc, char **argv)
{
	test_splitclientmessage();
	return 0;
}