#define CMDPARAMS const int playerid,const char *cmdtext,int parseidx

/*
Gets next int parameter in cmdtext after parseidx.

Preceding whitespace(s) are skipped.
On match, parseidx is the index right after the value, so either space or \0.

@return non-zero on success, with int in value parameter.
*/
static
int cmd_get_int_param(const char *cmdtext, int *parseidx, int *value)
{
	char *pc = (char*) cmdtext + *parseidx;
	int val = 0, sign = 1;

	/*not using atoi since parseidx needs to be updated*/
	while (*pc == ' ') {
		pc++;
	}
	if (*pc == '+') {
		pc++;
	} else if (*pc == '-') {
		sign = -1;
		pc++;
	}

nextchar:
	if (*pc < '0' || '9' < *pc) {
		return 0;
	}
	val = val * 10 + *pc - '0';
	pc++;
	if (*pc == 0 || *pc == ' ') {
		*parseidx = pc - cmdtext;
		*value = sign * val;
		return 1;
	}
	goto nextchar;
}

/*
Gets next player parameter in cmdtext after parseidx.

Preceding whitespace(s) are skipped.
On match, parseidx is the index right after the value, so either space or \0.
If a valid player id was given but id is not taken, INVALID_PLAYER_ID is used.

@return non-zero on success, with playerid in playerid parameter.
*/
static
int cmd_get_player_param(const char *cmdtext, int *parseidx, int *playerid)
{
	char name[MAX_PLAYER_NAME + 1], val;
	char *pc = (char*) cmdtext + *parseidx;
	char *n = name, *nend = name + sizeof(name);
	int maybe_id = 0, isnumeric = 1, i;

	while (*pc == ' ') {
		pc++;
	}

nextchar:
	if (n == nend) {
		/*longer than allowed player name length*/
		return 0;
	}
	val = *pc++;
	if (val <= ' ') {
		if (n == name) {
			/*zero length*/
			return 0;
		}
		*n = 0; /*add zero term to name buffer*/
		goto gotvalue;
	}
	*n++ = val;
	if (isnumeric && '0' <= val && val <= '9') {
		if ((maybe_id = maybe_id * 10 + val - '0') >= MAX_PLAYERS) {
			isnumeric = 0;
		}
		goto nextchar;
	}
	isnumeric = 0;
	if ('A' <= val && val <= 'Z') {
		/*adjust capitals*/
		*(n - 1) = val | 0x20;
		goto nextchar;
	}
	/*give up on invalid player name characters*/
	if ((val < 'a' || 'z' < val) && (val < '0' || '9' < val) &&
		val != '[' && val != ']' && val != '(' &&
		val != ')' && val != '$' && val != '@' &&
		val != '.' && val != '_' && val != '=')
	{
		return 0;
	}
	goto nextchar;

gotvalue:
	*parseidx = pc - cmdtext;
	*playerid = INVALID_PLAYER_ID;
	if (isnumeric) {
		if (IsPlayerConnected(maybe_id)) {
			*playerid = maybe_id;
		}
		return 1;
	}

	for (i = 0; i < MAX_PLAYERS; i++) {
		if (IsPlayerConnected(i) &&
			strstr(pdata[i]->normname, name) != NULL)
		{
			*playerid = i;
			return 1;
		}
	}
	return 1;
}

/*
Gets next string parameter in cmdtext after parseidx.

Preceding whitespace(s) are skipped.
On match, parseidx is the index right after the value, so either space or \0.

@return non-zero on success, with filled in buffer.
*/
static
int cmd_get_str_param(const char* cmdtext, int *parseidx, char *buf)
{
	char *pc = (char*) cmdtext + *parseidx;
	char *b = buf;

	while (*pc == ' ') {
		pc++;
	}
	while ((*b = *pc) > ' ') {
		b++;
		pc++;
	}
	if (b - buf > 0) {
		*b = 0;
		*parseidx = pc - cmdtext;
		return 1;
	}
	return 0;
}

/**
Gets next vehicle model parameter in cmdtext after parseidx.

Vehicle model can be either the id or the model name or the vehicle name.
Preceding whitespace(s) are skipped.
On match, parseidx is the index right after the value, so either space or \0.
Note: parseidx will also advance if a non-valid id was given.

@return non-zero on success, with filled in modelid parameter.
*/
static
int cmd_get_vehiclemodel_param(const char *cmdtext, int *parseidx, int *modelid)
{
	int i, matchindex, minimum_pos, startpos, pos;

	if (cmd_get_int_param(cmdtext, parseidx, &modelid)) {
		if (VEHICLE_MODEL_MIN  <= modelid && modelid < VEHICLE_MODEL_MAX) {
			return 1;
		}
		return 0;
	}

	if (cmd_get_str_param(cmdtext, parseidx, cbuf144)) {
		strlower(cbuf144);

		/*Modelnames, full match.*/
		for (i = 0; i < VEHICLE_MODEL_TOTAL; i++) {
			if (!strcmp(vehmodelnames[i], cbuf144)) {
				*modelid =  i + VEHICLE_MODEL_MIN;
				return 1;
			}
		}

		/*Vehicle names, partial match.*/
		/*Return the one where the occurence is the earliest,
		eg when inputting 'hy' it should prioritise 'HYdra' before 'dingHY'.*/
		matchindex = -1;
		minimum_pos = 1000;
		for (i = 0; i < VEHICLE_MODEL_TOTAL; i++) {
			startpos = 0;
			do {
				pos = 0;
				while ((vehnames[i][startpos + pos] | 0x20) == cbuf144[pos]) {
					pos++;
					if (cbuf144[pos] == 0) {
						if (startpos == 0) {
							*modelid = i + VEHICLE_MODEL_MIN;
							return 1;
						}
						if (startpos < minimum_pos) {
							minimum_pos = startpos;
							matchindex = i;
						}
						break;
					}
				}
				startpos++;
			} while (vehnames[i][startpos]);
		}
		if (matchindex != -1) {
			*modelid = matchindex + VEHICLE_MODEL_MIN;
			return 1;
		}
	}

	return 0;
}
