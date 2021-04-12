#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NO_CAST_IMM_FUNCPTR

#define cell int
typedef void (*cb_t)(void* data);
#define STATIC_ASSERT(E) typedef char __static_assert_[(E)?1:-1]
#define EXPECT_SIZE(S,SIZE) STATIC_ASSERT(sizeof(S)==(SIZE))
/*May also use offsetof(), but that requires stddef.h*/
#define MEMBER_OFFSET(S,M) (__builtin_offsetof(S,M))
#define STATIC_ASSERT_MEMBER_OFFSET(S,M,O)
EXPECT_SIZE(char, 1);
EXPECT_SIZE(short, 2);
EXPECT_SIZE(int, 4);
EXPECT_SIZE(float, 4);
struct vec3 {
	float x, y, z;
};
EXPECT_SIZE(struct vec3, 3 * sizeof(int));
struct vec4 {
	struct vec3 coords;
	float r;
};
EXPECT_SIZE(struct vec4, 4 * sizeof(int));
struct quat {
	/*The w component comes first in SAMP, do not change this order.*/
	float qw, qx, qy, qz;
};
EXPECT_SIZE(struct quat, 4 * sizeof(int));
#define NC_PARS(X)
#include "../game_sa_data.c"
#include "../a_samp.h"
#include "../samp.h"
#define csprintf(DST,FMT,...) atoci(DST,sprintf((char*)DST,FMT,__VA_ARGS__))
struct SampPlayer *player[MAX_PLAYERS];
static unsigned short _cc[MAX_PLAYERS];
static int userid[MAX_PLAYERS];
short players[MAX_PLAYERS];
int playercount;
unsigned char playeronlineflag[MAX_PLAYERS];
struct {
	struct dbvehicle *dbvehicle;
} gamevehicles[MAX_VEHICLES];

char *_test_name;

void test();

int main(int argc, char **argv)
{
	test();
	return 0;
}

void _test_exit_failure()
{
	if (_test_name[0]) {
		printf("failed test: %s\n", _test_name);
	}
	exit(1);
}

#define _assert(X) _assert_(__FILE__,__LINE__,X)
static
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
