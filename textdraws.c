#define TEXTDRAW_ALLOC_AS_NEEDED (-1)

#define TEXTDRAW_FLAG_BOX 0x1
#define TEXTDRAW_FLAG_LEFT 0x2
#define TEXTDRAW_FLAG_RIGHT 0x4
#define TEXTDRAW_FLAG_CENTER 0x8
#define TEXTDRAW_FLAG_PROPORTIONAL 0x10

#pragma pack(1)
struct TEXTDRAW {
	char *name;
	/**
	Amount of characters allocated for the text.
	May be TEXTDRAW_ALLOC_AS_NEEDED when passed to the loading function so
	it will allocate just as many characters as needed to fit the text that is set in the file.
	When TEXTDRAW_ALLOC_AS_NEEDED, this value will be replaced with the actual allocated amount,
	including a spot for the ending zero terminator.
	*/
	int allocated_text_length;
	/**
	Will be set by loading function.
	*/
	int rpcsize;
	struct RPCDATA_ShowTextDraw *rpcdata;
};

union TEXT_FILE_ENTRY {
	/*Using a union so the first 2 bytes of rpcdata can be overlapped with the name,*/
	/*since rpcdata has short textdrawid, that is not included in the text file.*/
	char name[40];
	struct {
		char namepadding[40-2];
		struct RPCDATA_ShowTextDraw rpcdata;
	} entry;
	char _size[905];
};
EXPECT_SIZE(union TEXT_FILE_ENTRY, 905);
#pragma pack()


/**
Load a map from file as specified in given map.

@param filename the file name, excluding path/extension.
@param base_textdraw_id the textdraw id for the first textdraw, the next ones will have consequtive ids
*/
static
void textdraws_load_from_file(char *filename, int base_textdraw_id, int numtextdraws, ...)
{
	union TEXT_FILE_ENTRY entry;
	struct TEXTDRAW *td;
	FILE *fs;
	char fullfilepath[100];
	int magic;
	int i;
	va_list va;

	sprintf(fullfilepath, "scriptfiles/texts/%s.text", filename);
	fs = fopen(fullfilepath, "rb");
	if (!fs) {
		logprintf("failed to open textdraw file %s", filename);
		goto ret;
	}

	/*read header*/
	if (!fread(&magic, 4, 1, fs)) {
		logprintf("corrupted textdraw file %s", filename);
		goto ret;
	}

	if (magic != 0x01545854) {
		logprintf("unknown textdraw spec %p in %s", magic, filename);
		goto ret;
	}

	while (fread(&entry, sizeof(entry), 1, fs)) {
		va_start(va, numtextdraws);
		for (i = 0; i < numtextdraws; i++) {
			td = va_arg(va, struct TEXTDRAW*);
			if (strcmp(td->name, entry.name) == 0) {
				if (td->allocated_text_length == TEXTDRAW_ALLOC_AS_NEEDED) {
					/*+1 for zero terminator*/
					td->allocated_text_length = entry.entry.rpcdata.text_length + 1;
				}
				/*-1 because struct includes a 1-sized char array for the text*/
				td->rpcsize = sizeof(struct RPCDATA_ShowTextDraw) - 1 + td->allocated_text_length;
				td->rpcdata = malloc(td->rpcsize);
				memcpy(td->rpcdata, &entry.entry.rpcdata, td->rpcsize);
				td->rpcdata->textdrawid = base_textdraw_id + i;
				/*the ending 0 byte is not needed in the rpc,
				but it's still allocated to be safe when checking out the string*/
				td->rpcsize--;
				break;
			}
		}
		va_end(va);
	}

ret:
	va_start(va, numtextdraws);
	for (i = 0; i < numtextdraws; i++) {
		td = va_arg(va, struct TEXTDRAW*);
		if (td->rpcdata == NULL) {
			/*TODO replace with logprintf once logprintf is patched*/
			printf("WARN: textdraw '%s' not found in '%s'\n", td->name, fullfilepath);
			td->rpcsize = sizeof(struct RPCDATA_ShowTextDraw) + 12;
			td->rpcdata = malloc(td->rpcsize);
			memset(td->rpcdata, 0, td->rpcsize);
			td->rpcsize--;
			td->rpcdata->flags = TEXTDRAW_FLAG_CENTER;
			td->rpcdata->font = 1;
			td->rpcdata->font_width = 1.0f;
			td->rpcdata->font_height = 4.0f;
			td->rpcdata->x = 320.0f;
			td->rpcdata->y = 224.0f;
			td->rpcdata->font_color = -1;
			td->rpcdata->text_length = 12;
			td->rpcdata->textdrawid = base_textdraw_id + i;
			strcpy(td->rpcdata->text, "missing text");
		}
	}
	va_end(va);

	if (fs) {
		fclose(fs);
	}
	return;
}

static
void textdraws_show(int playerid, int num, ...)
{
	struct TEXTDRAW *td;
	va_list va;

	va_start(va, num);
	while (num--) {
		td = va_arg(va, struct TEXTDRAW*);
		bitstream_freeform.numberOfBitsUsed = td->rpcsize * 8;
		bitstream_freeform.numberOfBitsAllocated = td->rpcsize * 8;
		bitstream_freeform.ptrData = td->rpcdata;
		SAMP_SendRPCToPlayer(RPC_ShowTextDraw, &bitstream_freeform, playerid, 2);
	}
	va_end(va);
}

static
void textdraws_hide(int playerid, int num, ...)
{
	struct TEXTDRAW *td;
	va_list va;

	bitstream_freeform.numberOfBitsUsed = sizeof(short) * 8;
	bitstream_freeform.numberOfBitsAllocated = sizeof(short) * 8;
	bitstream_freeform.ptrData = &rpcdata_freeform;
	va_start(va, num);
	while (num--) {
		td = va_arg(va, struct TEXTDRAW*);
		rpcdata_freeform.word[0] = td->rpcdata->textdrawid;
		SAMP_SendRPCToPlayer(RPC_HideTextDraw, &bitstream_freeform, playerid, 2);
	}
	va_end(va);
}

/**
Hides a number of textdraws that all have ids that follow each other.
*/
static
void textdraws_hide_consecutive(int playerid, int num, int base_id)
{
	bitstream_freeform.numberOfBitsUsed = sizeof(short) * 8;
	bitstream_freeform.numberOfBitsAllocated = sizeof(short) * 8;
	bitstream_freeform.ptrData = &rpcdata_freeform;
	while (num--) {
		rpcdata_freeform.word[0] = base_id;
		SAMP_SendRPCToPlayer(RPC_HideTextDraw, &bitstream_freeform, playerid, 2);
		base_id++;
	}
}
