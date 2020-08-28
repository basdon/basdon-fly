static
void mem_unprotect(int address, int length)
{
	int pagesize, start, end;

	pagesize = getpagesize();
	start = address - (address % pagesize);
	address += length - 1;
	end = address - (address % pagesize);
	while (start <= end) {
		mprotect((void*) start, pagesize, PROT_READ | PROT_WRITE | PROT_EXEC);
		start += pagesize;
	}
}

static
void mem_mkjmp(int at, void *to)
{
	int toaddr = (int) to;

	mem_unprotect(at, 5);
	*(char*) at = 0xE9;
	at++;
	*((int*) at) = toaddr - at - 4;
}
