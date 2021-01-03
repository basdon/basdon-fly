#include <assert.h>
#include <stdio.h>
#include <string.h>

#define INFILE "samp.asm"
#define OUTFILE "__sampasm.h"
#define MAX_TESTS 50

int main()
{
	FILE *in, *out;
	char line[1024];
	char funcname[72];
	char tests[MAX_TESTS][200];
	int num_tests;
	int i, j;

	in = fopen(INFILE, "r");
	if (!in) {
		puts("preprocess: can't open "INFILE" for reading");
		return 1;
	}

	out = fopen(OUTFILE, "w");
	if (!out) {
		fclose(in);
		puts("preprocess: can't open "OUTFILE" for reading");
		return 1;
	}

	num_tests = 0;
	while (fgets(line, sizeof(line), in)) {
		if (strncmp(";prot ", line, 6) == 0) {
			fputs(&line[6], out);
			for (i = 6; i < sizeof(line); i++) {
				if (line[i] == 0) {
					break;
				}
				if (line[i] == '(') {
					for (j = i; j >= 6; j--) {
						if (line[j] == ' ') {
							memcpy(funcname, &line[j + 1], i - j - 1);
							funcname[i - j - 1] = 0;
							break;
						}
					}
					break;
				}
			}
		} else if (strncmp(";test ", line, 6) == 0) {
			for (i = 0; i < sizeof(line); i++) {
				if (line[i] == 0) {
					if (i > 0 && line[i - 1] == '\n') {
						line[i - 1] = 0;
					}
					break;
				}
			}
			sprintf(tests[num_tests], "\tassert(((void) \"asmtest %s\", %s));\n", funcname, &line[6]);
			num_tests++;
			assert(num_tests < MAX_TESTS);
		}
	}

	fputs("\n#ifdef DEV\nstatic\nvoid sampasm_sanitycheck()\n{\n", out);
	for (i = 0; i < num_tests; i++) {
		fputs(tests[i], out);
	}
	fputs("}\n#endif", out);

	fclose(in);
	fclose(out);
	return 0;
}
