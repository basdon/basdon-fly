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

char *_test_name, *_last_test_name;
const char *_last_test_func;
int num_tests, num_functions;

static
void count_test(const char *func)
{
	if (_last_test_func != func) {
		_last_test_func = func;
		num_functions++;
	}
	if (_last_test_name != _test_name) {
		_last_test_name = _test_name;
		num_tests++;
	}
}

#define _print_tests_summary() _print_tests_summary_(__FILE__)
static
void _print_tests_summary_(char *file)
{
	printf("tests ok: %d passed, %d suites (%s)\n", num_tests, num_functions, file);
}

#define _assert(X) _assert_(__FILE__,__FUNCTION__,__LINE__,X)
static
__attribute__((unused))
void _assert_(char *file, const char *func, int line, int condition)
{
	count_test(func);
	if (!condition) {
		printf("\nTEST FAILED\n");
		printf("%s:%d \"%s: %s\"\n", file, line, func, _test_name);
		printf("_assert condition failed\n\n");
		exit(1);
	}
}

#define _assert_equals(X,Y) _assert_equals_(__FILE__,__FUNCTION__,__LINE__,X,Y)
static
__attribute__((unused))
void _assert_equals_(char *file, const char *func, int line, int a, int b)
{
	count_test(func);
	if (a != b) {
		printf("\nTEST FAILED\n");
		printf("%s:%d \"%s: %s\"\n", file, line, func, _test_name);
		printf("_assert_equals expected %d got %d\n\n", a, b);
		exit(1);
	}
}

#define _assert_strcmp(X,Y) _assert_strcmp_(__FILE__,__FUNCTION__,__LINE__,X,Y)
static
__attribute__((unused))
void _assert_strcmp_(char *file, const char *func, int line, char *a, char *b)
{
	count_test(func);
	if (strcmp(a, b)) {
		printf("\nTEST FAILED\n");
		printf("%s:%d \"%s: %s\"\n", file, line, func, _test_name);
		printf("_assert_strcmp\n");
		printf(">>> expected\n");
		printf("%s\n", a);
		printf("=== actual\n");
		printf("%s\n", b);
		printf("<<<\n\n");
		exit(1);
	}
}

#define _assert_strcmpex(X,Y) _assert_strcmpex_(__FILE__,__FUNCTION__,__LINE__,X,Y)
static
__attribute__((unused))
void _assert_strcmpex_(char *file, const char *func, int line, char *a, char *b)
{
	char *zero_terminated_b;
	int len, i;

	count_test(func);
	len = strlen(a);
	if (strncmp(a, b, len)) {
		zero_terminated_b = alloca(len + 1);
		zero_terminated_b[len] = 0;
		for (i = 0; i < len; i++) {
			if (!(zero_terminated_b[i] = b[i])) {
				break;
			}
		}
		printf("\nTEST FAILED\n");
		printf("%s:%d \"%s: %s\"\n", file, line, func, _test_name);
		printf("_assert_strcmpex\n");
		printf(">>> expected\n");
		printf("%s\n", a);
		printf("=== actual\n");
		printf("%s\n", zero_terminated_b);
		printf("<<<\n\n");
		exit(1);
	}
}
