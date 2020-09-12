/**
@param protection mask with PROT_READ, PROT_WRITE, PROT_EXEC
*/
static
void mem_protect(int address, int length, int protection)
{
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
	int toaddr = (int) to;

	mem_protect(at, 5, PROT_READ | PROT_WRITE | PROT_EXEC);
	*(char*) at = 0xE9;
	at++;
	*((int*) at) = toaddr - at - 4;
}
