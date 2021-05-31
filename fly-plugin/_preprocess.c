#include <assert.h>
#include <stdio.h>
#include <string.h>

static
FILE *checked_fopen(char *filename, char *mode)
{
	FILE *file;

	file = fopen(filename, mode);
	if (!file) {
		printf("preprocess: can't fopen '%s' with mode '%s'\n", filename, mode);
		return 0;
	}
	return file;
}

static
int generate_settings_from_ini_file()
{
	FILE *in, *out;
	char line[1024];

	in = checked_fopen("../settings.ini", "r");
	if (!in) {
		return 1;
	}

	out = checked_fopen("__settings.h", "w");
	if (!out) {
		fclose(in);
		return 1;
	}

	while (fgets(line, sizeof(line), in)) {
		if (line[0] && line[0] != '\n' && line[0] != '#') {
			fwrite("#define SETTING__", 17, 1, out);
			fputs(line, out);
		}
	}

	fclose(in);
	fclose(out);
	return 0;
}

static
int generate_sampasm_header_from_asm_file()
{
#define SAMPASM_MAX_TESTS 50

	FILE *in, *out;
	char line[1024];
	char funcname[72];
	char tests[SAMPASM_MAX_TESTS][200];
	int num_tests;
	int i, j;

	in = checked_fopen("samp.asm", "r");
	if (!in) {
		return 1;
	}

	out = checked_fopen("__sampasm.h", "w");
	if (!out) {
		fclose(in);
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
			assert(num_tests < SAMPASM_MAX_TESTS);
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

int main()
{
	return generate_settings_from_ini_file() || generate_sampasm_header_from_asm_file();
}
