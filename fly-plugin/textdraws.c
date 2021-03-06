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
Template textbox textdraw. Use {@link textdraws_set_textbox_properties}.
*/
static struct TEXTDRAW td_helpbox_template = { "58B6E0", 0, NULL };

/**
Load textdraws from a textdraw file.

@param filename the file name, excluding path/extension.
@param base_textdraw_id the textdraw id for the first textdraw, the next ones will have consecutive ids
*/
void textdraws_load_from_file_a(char *filename, int base_textdraw_id, int numtextdraws, struct TEXTDRAW **tds)
{
	struct TEXTDRAW *td;
	union TEXT_FILE_ENTRY entry;
	FILE *fs;
	char fullfilepath[100];
	int magic;
	int i;
	int allocated_size;

	sprintf(fullfilepath, "../textdraws/%s.text", filename);
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
		for (i = 0; i < numtextdraws; i++) {
			td = tds[i];
			if (strcmp(td->name, entry.name) == 0) {
				if (td->allocated_text_length == TEXTDRAW_ALLOC_AS_NEEDED) {
					/*+1 for zero terminator*/
					td->allocated_text_length = entry.entry.rpcdata.text_length + 1;
				}
				/*-1 because struct includes a 1-sized char array for the text*/
				allocated_size = sizeof(struct RPCDATA_ShowTextDraw) - 1 + td->allocated_text_length;
				td->rpcdata = malloc(allocated_size);
				memcpy(td->rpcdata, &entry.entry.rpcdata, allocated_size);
				td->rpcdata->textdrawid = base_textdraw_id + i;
				break;
			}
		}
	}

ret:
	for (i = 0; i < numtextdraws; i++) {
		td = tds[i];
		if (td->rpcdata == NULL) {
			logprintf("WARN: textdraw '%s' not found in '%s'", td->name, fullfilepath);
			td->allocated_text_length = 700;
			allocated_size = sizeof(struct RPCDATA_ShowTextDraw) - 1 + td->allocated_text_length;
			td->rpcdata = malloc(allocated_size);
			memset(td->rpcdata, 0, allocated_size);
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
		/*Put some propaganda in the client's unused allocated memory.*/
		if (td->rpcdata->font != 5) {
			memcpy(&td->rpcdata->preview_model, "basdon.net robin_be", 20);
		}
	}

	if (fs) {
		fclose(fs);
	}
}

/**
Load textdraws from a textdraw file.

@param filename the file name, excluding path/extension.
@param base_textdraw_id the textdraw id for the first textdraw, the next ones will have consequtive ids
*/
static
void textdraws_load_from_file(char *filename, int base_textdraw_id, int numtextdraws, ...)
{
	struct TEXTDRAW **tds, **tdp;
	va_list va;
	int i;

	tds = tdp = alloca(numtextdraws * sizeof(tds));
	va_start(va, numtextdraws);
	for (i = 0; i < numtextdraws; i++) {
		tdp[i] = va_arg(va, struct TEXTDRAW*);
	}
	va_end(va);
	textdraws_load_from_file_a(filename, base_textdraw_id, numtextdraws, tds);
}

/**
Will allocate memory.
*/
static
void textdraws_initialize_from(struct TEXTDRAW *template, struct TEXTDRAW *td, short textdrawid)
{
	int allocated_size;

	/*-1 because struct includes a 1-sized char array for the text*/
	allocated_size = sizeof(struct RPCDATA_ShowTextDraw) - 1 + template->allocated_text_length;
	td->rpcdata = malloc(allocated_size);
	td->allocated_text_length = template->allocated_text_length;
	memcpy(td->rpcdata, template->rpcdata, allocated_size);
	td->rpcdata->textdrawid = textdrawid;
}

#ifdef DEV
static
void textdraws_assert_text_length_within_bounds(struct TEXTDRAW *td)
{
	int i;

	if (td->rpcdata->text_length > td->allocated_text_length) {
		printf("ERR: textdraw '%s' text length %d exceed allocated length %d\n",
			td->name,
			td->rpcdata->text_length,
			td->allocated_text_length);
		printf("text is: ");
		for (i = 0; i < td->allocated_text_length; i++) {
			printf("%c", td->rpcdata->text[i]);
		}
		printf("\n");
		assert(0);
	}
}
#endif

static
void textdraws_strcpy(struct TEXTDRAW *textdraw, char* str)
{
	textdraw->rpcdata->text_length = sprintf(textdraw->rpcdata->text, "%s", str);
#ifdef DEV
	textdraws_assert_text_length_within_bounds(textdraw);
#endif
}

static
void textdraws_show(int playerid, int num, ...)
{
	struct BitStream bs;
	struct TEXTDRAW *td;
	va_list va;

	va_start(va, num);
	while (num--) {
		td = va_arg(va, struct TEXTDRAW*);
#ifdef DEV
		textdraws_assert_text_length_within_bounds(td);
#endif
		bs.ptrData = td->rpcdata;
		bs.numberOfBitsUsed = (sizeof(struct RPCDATA_ShowTextDraw) - 1 + td->rpcdata->text_length) * 8;
		SAMP_SendRPCToPlayer(RPC_ShowTextDraw, &bs, playerid, 2);
	}
	va_end(va);
}

static
void textdraws_show_a(int playerid, int num, struct TEXTDRAW **tds)
{
	struct BitStream bs;
	struct TEXTDRAW *td;

	while (num--) {
		td = tds[num];
#ifdef DEV
		textdraws_assert_text_length_within_bounds(td);
#endif
		bs.ptrData = td->rpcdata;
		bs.numberOfBitsUsed = (sizeof(struct RPCDATA_ShowTextDraw) - 1 + td->rpcdata->text_length) * 8;
		SAMP_SendRPCToPlayer(RPC_ShowTextDraw, &bs, playerid, 2);
	}
}

/**
Hides a number of textdraws that all have ids that follow each other.
*/
static
void textdraws_hide_consecutive(int playerid, int num, int base_id)
{
	struct RPCDATA_HideTextDraw rpcdata;
	struct BitStream bs;

	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
	while (num--) {
		rpcdata.textdrawid = (short) base_id;
		SAMP_SendRPCToPlayer(RPC_HideTextDraw, &bs, playerid, 2);
		base_id++;
	}
}

static
void textdraws_init()
{
	textdraws_load_from_file("help", 0, 1, &td_helpbox_template);
}

/**
Copies textdraw properties from the textbox template textdraw to the given textdraw data.
*/
static
void textdraws_set_textbox_properties(struct RPCDATA_ShowTextDraw *textdraw_rpcdata)
{
	memcpy(
		(void*) ((int) textdraw_rpcdata + MEMBER_OFFSET(struct RPCDATA_ShowTextDraw, flags)),
		(void*) ((int) td_helpbox_template.rpcdata + MEMBER_OFFSET(struct RPCDATA_ShowTextDraw, flags)),
		MEMBER_OFFSET(struct RPCDATA_ShowTextDraw, preview_model) - MEMBER_OFFSET(struct RPCDATA_ShowTextDraw, flags));
}
