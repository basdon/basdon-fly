/**
@param protection mask with PROT_READ, PROT_WRITE, PROT_EXEC
*/
static
void mem_protect(int address, int length, int protection)
{
	TRACE;
	int pagesize, start, end;

	pagesize = getpagesize();
	start = address - (address % pagesize);
	address += length - 1;
	end = address - (address % pagesize);
	while (start <= end) {
		mprotect((void*) start, pagesize, protection);
		start += pagesize;
	}
}

static
void mem_mkjmp(int at, void *to)
{
	TRACE;
	int toaddr = (int) to;

	mem_protect(at, 5, PROT_READ | PROT_WRITE | PROT_EXEC);
	*(char*) at = 0xE9;
	at++;
	*((int*) at) = toaddr - at - 4;
}

static
void mem_redirectjmp(int at, void *to)
{
	TRACE;

	at++;
	mem_protect(at, 4, PROT_READ | PROT_WRITE | PROT_EXEC);
	*((int*) at) = (int) to - at - 4;
}

/*For functionality that we hooked and replaced, redirect code paths to this so the server*/
/*crashes if it's executed somehow anyways instead of causing hard-to-trace corruption.*/
static
__attribute__((noreturn))
void crash__this_codepath_should_be_unreachable()
{
	TRACE;

	asmcrash();
}
