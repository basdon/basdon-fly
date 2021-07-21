#define JOBMAP_SIZE 80000.0f
#define JOBMAP_HALFSIZE 40000.0f

struct JOBMAP_ENTRY {
	struct AIRPORT *airport;
	int distance;
};

/*Current textdraws are made for max 15 entries per page.*/
#define JOBMAP_NUM_ENTRIES_PERPAGE (15)
#define JOBMAP_ENTRY_SORT -3
#define JOBMAP_ENTRY_PREV -2
#define JOBMAP_ENTRY_NEXT -1
#define NUM_JOBMAP_SORTS 3
#define JOBMAP_SORT_NAME_ASCENDING 0
#define JOBMAP_SORT_DISTANCE_DESCENDING 1
#define JOBMAP_SORT_DISTANCE_ASCENDING 2
static char *jobmap_sort_text[NUM_JOBMAP_SORTS] = {
	"Sort by: name",
	"Sort by: distance (descending)",
	"Sort by: distance (ascending)",
};
struct JOBMAP_DATA {
	/**Value should be -3 to JOBMAP_NUM_ENTRIES_PERPAGE (exclusive). See JOBMAP_ENTRY_*.*/
	char selected_entry;
	char page;
	char max_page;
	struct JOBMAP_ENTRY *entries;
	unsigned short num_entries;
	/**One of JOBMAP_SORT_*.*/
	unsigned char sort;
};
struct JOBMAP_DATA jobmapdatas[MAX_PLAYERS];

#define NUM_MAP_TEXTDRAWS (13)
static struct TEXTDRAW td_jobmap_keyhelp = { "keyhelp", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobmap_title = { "title", 80, NULL };
static struct TEXTDRAW td_jobmap_bg = { "bg", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobmap_header = { "header", 80, NULL };
static struct TEXTDRAW td_jobmap_header_right = { "header-right", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobmap_mapbg = { "mapbg", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobmap_sort = { "sort", 80, NULL };
static struct TEXTDRAW td_jobmap_prevpage = { "prevpage", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobmap_nextpage = { "nextpage", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
#define JOBMAP_ENTRY_MAX_TEXT_LENGTH (35) /*"Some very long airport name okay~n~"*/
#define JOBMAP_DISTANCE_MAX_TEXT_LENGTH (12) /*"1004.9Km~n~_"*/
static struct TEXTDRAW td_jobmap_opts = { "opts", JOBMAP_ENTRY_MAX_TEXT_LENGTH * JOBMAP_NUM_ENTRIES_PERPAGE, NULL };
static struct TEXTDRAW td_jobmap_optdists = { "optdists", JOBMAP_DISTANCE_MAX_TEXT_LENGTH * JOBMAP_NUM_ENTRIES_PERPAGE, NULL };
static struct TEXTDRAW td_jobmap_selection_left = { "selection-left", JOBMAP_ENTRY_MAX_TEXT_LENGTH, NULL };
static struct TEXTDRAW td_jobmap_selection_right = { "selection-right", JOBMAP_DISTANCE_MAX_TEXT_LENGTH, NULL };
/*Island/location texts*/
STATIC_ASSERT(NUM_MISSIONMAP_LOCATIONS_TEXTDRAWS == 9);
static struct TEXTDRAW td_jobmap_island_mainland = { "mapsqfull", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobmap_island_octa = { "mapsqmintopleft", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobmap_island_cata = { "mapsqminbotright", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobmap_island_igzu = { NULL, TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobmap_island_bnsa = { NULL, TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobmap_island_dune = { NULL, TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobmap_island_strs = { NULL, TEXTDRAW_ALLOC_AS_NEEDED, NULL };
STATIC_ASSERT(AIRPORT_CODE_LEN == 4); /*mapmin/mapmax text alloc depend on airport code being 4 chars at most*/
static struct TEXTDRAW td_jobmap_from = { "mapmin", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobmap_to = { "mapmax", TEXTDRAW_ALLOC_AS_NEEDED, NULL };

static float map_text_x_base, map_text_x_unit;
static float map_text_y_base, map_text_y_unit;
static float map_list_entry_line_height;

static
void jobmap_hide(int playerid)
{
	textdraws_hide_consecutive(playerid, NUM_MAP_TEXTDRAWS, TEXTDRAW_MISSIONMAP_BASE);
	textdraws_hide_consecutive(playerid, NUM_MISSIONMAP_LOCATIONS_TEXTDRAWS, TEXTDRAW_MISSIONMAP_LOCATIONS_BASE);
	free(jobmapdatas[playerid].entries);
	jobmapdatas[playerid].entries = NULL;
	kneeboard_unhide(playerid, KNEEBOARD_HIDE_REASON_JOBMAP);
	panel_unhide(playerid, PANEL_HIDE_REASON_JOBMAP);
}

static
int jobmap_format_header(char *buffer, struct JOBMAP_DATA *jobmapdata)
{
	return sprintf(buffer, "Destinations (page %d of %d)", jobmapdata->page + 1, jobmapdata->max_page + 1);
}

static
int jobmap_format_distance(char *buffer, int distance)
{
	if (distance < 1000) {
		return sprintf(buffer, "%dm", distance);
	} else {
		return sprintf(buffer, "%.1fkm", distance / 1000.0f);
	}
}

static
int jobmapentry_sortfunc_name_asc(const void *_a, const void *_b)
{
	return strcmp(((struct JOBMAP_ENTRY*) _a)->airport->name, ((struct JOBMAP_ENTRY*) _b)->airport->name);
}

static
int jobmapentry_sortfunc_distance_desc(const void *_a, const void *_b)
{
	return ((struct JOBMAP_ENTRY*) _b)->distance - ((struct JOBMAP_ENTRY*) _a)->distance;
}

static
int jobmapentry_sortfunc_distance_asc(const void *_a, const void *_b)
{
	return ((struct JOBMAP_ENTRY*) _a)->distance - ((struct JOBMAP_ENTRY*) _b)->distance;
}

static void *jobmap_sortfuncs[NUM_JOBMAP_SORTS] = {
	jobmapentry_sortfunc_name_asc,
	jobmapentry_sortfunc_distance_desc,
	jobmapentry_sortfunc_distance_asc,
};

/**
Call jobmap_update_entries_textdraws after.
*/
static
void jobmap_resort_entries(struct JOBMAP_DATA *jobmapdata)
{
	qsort(jobmapdata->entries, jobmapdata->num_entries, sizeof(struct JOBMAP_ENTRY), jobmap_sortfuncs[jobmapdata->sort]);
}

static
void jobmap_format_entries(
	char *optbuffer, short *out_optbuffer_len,
	char *optdistbuffer, short *out_optdistbuffer_len,
	struct JOBMAP_DATA *jobmapdata)
{
	register struct JOBMAP_ENTRY *entry;
	int i, max_plus_one;
	char *orig_optbuffer, *orig_optdistbuffer;

	if (!jobmapdata->num_entries) {
		*((short*) optbuffer) = (short) '_';
		*((short*) optdistbuffer) = (short) '_';
		*out_optbuffer_len = 1;
		*out_optdistbuffer_len = 1;
	} else {
		orig_optbuffer = optbuffer;
		orig_optdistbuffer = optdistbuffer;
		i = jobmapdata->page * JOBMAP_NUM_ENTRIES_PERPAGE;
		max_plus_one = i + JOBMAP_NUM_ENTRIES_PERPAGE;
		if (max_plus_one > jobmapdata->num_entries) {
			max_plus_one = jobmapdata->num_entries;
		}
		for (;;) {
			entry = jobmapdata->entries + i;
			optbuffer += sprintf(optbuffer, "%s", entry->airport->name);
			optdistbuffer += jobmap_format_distance(optdistbuffer, entry->distance);
			if (++i >= max_plus_one) {
				break;
			}
			/*The space/underscore is needed or for some reason all but the last entry
			will be one char too much to the right (right alignment with newlines quirk).*/
			*((int*) optdistbuffer) = '_' | ('~' << 8) | ('n' << 16) | ('~' << 24);
			optdistbuffer += 4;
			*((int*) optbuffer) = '~' | ('n' << 8) | ('~' << 16);
			optbuffer += 3;
		}
		*out_optbuffer_len = optbuffer - orig_optbuffer;
		*out_optdistbuffer_len = optdistbuffer - orig_optdistbuffer;
	}
}

static
void jobmap_update_selection_textdraws(int playerid, struct JOBMAP_DATA *jobmapdata)
{
	register struct JOBMAP_ENTRY *entry;
	register unsigned char absolute_selected_entry;

	switch (jobmapdata->selected_entry) {
	case JOBMAP_ENTRY_SORT:
		textdraws_strcpy(&td_jobmap_selection_left, jobmap_sort_text[jobmapdata->sort]);
		td_jobmap_selection_left.rpcdata->y = td_jobmap_sort.rpcdata->y;
		break;
	case JOBMAP_ENTRY_PREV:
		textdraws_strcpy(&td_jobmap_selection_left, td_jobmap_prevpage.rpcdata->text);
		td_jobmap_selection_left.rpcdata->y = td_jobmap_prevpage.rpcdata->y;
		break;
	case JOBMAP_ENTRY_NEXT:
		textdraws_strcpy(&td_jobmap_selection_left, td_jobmap_nextpage.rpcdata->text);
		td_jobmap_selection_left.rpcdata->y = td_jobmap_nextpage.rpcdata->y;
		break;
	default:
		absolute_selected_entry = (unsigned char) jobmapdata->selected_entry + jobmapdata->page * JOBMAP_NUM_ENTRIES_PERPAGE;
		entry = &jobmapdata->entries[absolute_selected_entry];
		textdraws_strcpy(&td_jobmap_selection_left, entry->airport->name);
		td_jobmap_selection_right.rpcdata->text_length =
			jobmap_format_distance(td_jobmap_selection_right.rpcdata->text, entry->distance);
		td_jobmap_selection_left.rpcdata->y =
			td_jobmap_selection_right.rpcdata->y =
			td_jobmap_opts.rpcdata->y + map_list_entry_line_height * jobmapdata->selected_entry;
		strcpy(td_jobmap_to.rpcdata->text, entry->airport->code); /*This would need to be textdraws_strcpy,
		                                                            but it's fine since the text length never changes*/
		td_jobmap_to.rpcdata->x = map_text_x_base + map_text_x_unit * (entry->airport->pos.x + JOBMAP_HALFSIZE);
		td_jobmap_to.rpcdata->y = map_text_y_base + map_text_y_unit * (-entry->airport->pos.y + JOBMAP_HALFSIZE);
		textdraws_show(playerid, 3, &td_jobmap_selection_left, &td_jobmap_selection_right, &td_jobmap_to);
		return;
	}
	td_jobmap_selection_right.rpcdata->y = 500.0f; /*out of screen*/
	textdraws_hide_consecutive(playerid, 1, td_jobmap_to.rpcdata->textdrawid);
	textdraws_show(playerid, 2, &td_jobmap_selection_left, &td_jobmap_selection_right);
}

/**
@param udkey must not be 0
*/
static
void jobmap_move_updown(int playerid, int udkey)
{
	register struct JOBMAP_DATA *jobmapdata = &jobmapdatas[playerid];
	register int selected_entry;
	int max_selection_plus_one;

	max_selection_plus_one = jobmapdata->num_entries - jobmapdata->page * JOBMAP_NUM_ENTRIES_PERPAGE;
	if (max_selection_plus_one > JOBMAP_NUM_ENTRIES_PERPAGE) {
		max_selection_plus_one = JOBMAP_NUM_ENTRIES_PERPAGE;
	}
	selected_entry = jobmapdata->selected_entry;
	if (udkey < 0) {
		PlayerPlaySound(playerid, MISSION_JOBMAP_MOVE_UP_SOUND);
		switch (selected_entry) {
		case JOBMAP_ENTRY_SORT:
			selected_entry = max_selection_plus_one;
		default:
			selected_entry--;
			if (selected_entry >= 0) {
				break;
			}
			if (jobmapdata->page < jobmapdata->max_page) {
				break;
			}
		case JOBMAP_ENTRY_NEXT:
			selected_entry = JOBMAP_ENTRY_PREV;
			if (jobmapdata->page) {
				break;
			}
		case JOBMAP_ENTRY_PREV:
			selected_entry = JOBMAP_ENTRY_SORT;
		}
	} else {
		PlayerPlaySound(playerid, MISSION_JOBMAP_MOVE_DOWN_SOUND);
		switch (selected_entry) {
		case JOBMAP_ENTRY_SORT:
			selected_entry = JOBMAP_ENTRY_PREV;
			if (jobmapdata->page) {
				break;
			}
		case JOBMAP_ENTRY_PREV:
			selected_entry = JOBMAP_ENTRY_NEXT;
			if (jobmapdata->page < jobmapdata->max_page) {
				break;
			}
		default:
			selected_entry++;
			if (selected_entry >= max_selection_plus_one) {
				selected_entry = JOBMAP_ENTRY_SORT;
			}
		}
	}
	jobmapdata->selected_entry = selected_entry;
	jobmap_update_selection_textdraws(playerid, jobmapdata);
}

static
void jobmap_change_page(int playerid, struct JOBMAP_DATA *jobmapdata, int direction)
{
	struct RPCDATA_TextDrawSetString rpcdata1, rpcdata2;
	struct BitStream bs;
	int prev_page;

	prev_page = jobmapdata->page;
	jobmapdata->page += direction;

	/*Update entries.*/
	jobmap_format_entries(
		rpcdata1.text, &rpcdata1.text_length,
		rpcdata2.text, &rpcdata2.text_length,
		jobmapdata
	);
	rpcdata1.textdrawid = td_jobmap_opts.rpcdata->textdrawid;
	rpcdata2.textdrawid = td_jobmap_optdists.rpcdata->textdrawid;
	bs.ptrData = &rpcdata1;
	bs.numberOfBitsUsed = (2 + 2 + rpcdata1.text_length) * 8;
	SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bs, playerid, 2);
	bs.ptrData = &rpcdata2;
	bs.numberOfBitsUsed = (2 + 2 + rpcdata2.text_length) * 8;
	SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bs, playerid, 2);

	/*Header.*/
	rpcdata2.textdrawid = td_jobmap_header.rpcdata->textdrawid;
	rpcdata2.text_length = jobmap_format_header(rpcdata2.text, jobmapdata);
	bs.numberOfBitsUsed = (2 + 2 + rpcdata2.text_length) * 8;
	SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bs, playerid, 2);

	/*Update prev/next enabled state (and if now first page, change selection to 'next page' and vice versa).*/
	if (jobmapdata->page < jobmapdata->max_page) {
		if (prev_page == jobmapdata->max_page) {
			td_jobmap_nextpage.rpcdata->font_color = COL_PANEL_ROW_NORMAL;
			textdraws_show(playerid, 1, &td_jobmap_nextpage);
		}
	} else {
		jobmapdata->selected_entry = JOBMAP_ENTRY_PREV;
		td_jobmap_nextpage.rpcdata->font_color = COL_PANEL_ROW_DISABLED;
		textdraws_show(playerid, 1, &td_jobmap_nextpage);
	}
	if (jobmapdata->page) {
		if (!prev_page) {
			td_jobmap_prevpage.rpcdata->font_color = COL_PANEL_ROW_NORMAL;
			textdraws_show(playerid, 1, &td_jobmap_prevpage);
		}
	} else {
		jobmapdata->selected_entry = JOBMAP_ENTRY_NEXT;
		td_jobmap_prevpage.rpcdata->font_color = COL_PANEL_ROW_DISABLED;
		textdraws_show(playerid, 1, &td_jobmap_prevpage);
	}

	jobmap_update_selection_textdraws(playerid, jobmapdata);
}

/**
@return airport when one has been selected, otherwise NULL (in case of sort/pagination actions)
*/
static
struct AIRPORT *jobmap_do_accept_button(int playerid)
{
	register struct JOBMAP_DATA *jobmapdata = &jobmapdatas[playerid];
	register unsigned char entry;
	struct RPCDATA_TextDrawSetString rpcdata1, rpcdata2;
	struct BitStream bs;

	switch (jobmapdata->selected_entry) {
	case JOBMAP_ENTRY_SORT:
		jobmapdata->sort = (jobmapdata->sort + 1) % NUM_JOBMAP_SORTS;
		jobmap_resort_entries(jobmapdata);
		jobmap_format_entries(
			rpcdata1.text, &rpcdata1.text_length,
			rpcdata2.text, &rpcdata2.text_length,
			jobmapdata
		);
		rpcdata1.textdrawid = td_jobmap_opts.rpcdata->textdrawid;
		rpcdata2.textdrawid = td_jobmap_optdists.rpcdata->textdrawid;
		bs.ptrData = &rpcdata1;
		bs.numberOfBitsUsed = (2 + 2 + rpcdata1.text_length) * 8;
		SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bs, playerid, 2);
		bs.ptrData = &rpcdata2;
		bs.numberOfBitsUsed = (2 + 2 + rpcdata2.text_length) * 8;
		SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bs, playerid, 2);
		rpcdata2.textdrawid = td_jobmap_sort.rpcdata->textdrawid;
		rpcdata2.text_length = sprintf(rpcdata2.text, "%s", jobmap_sort_text[jobmapdata->sort]);
		bs.numberOfBitsUsed = (2 + 2 + rpcdata2.text_length) * 8;
		SAMP_SendRPCToPlayer(RPC_TextDrawSetString, &bs, playerid, 2);
		jobmap_update_selection_textdraws(playerid, jobmapdata);
		return NULL;
	case JOBMAP_ENTRY_PREV:
		jobmap_change_page(playerid, jobmapdata, -1);
		return NULL;
	case JOBMAP_ENTRY_NEXT:
		jobmap_change_page(playerid, jobmapdata, 1);
		return NULL;
	default:
		entry = (unsigned char) jobmapdata->selected_entry + jobmapdata->page * JOBMAP_NUM_ENTRIES_PERPAGE;
		return jobmapdata->entries[entry].airport;
	}
}

static
void jobmap_show(int playerid, unsigned int mission_type, struct MISSIONPOINT *msp)
{
	struct JOBMAP_DATA *jobmapdata = &jobmapdatas[playerid];
	struct JOBMAP_ENTRY *next_entry;
	float dx, dy;
	int i;

	if (jobmapdata->entries) {
		free(jobmapdata->entries);
	}
	jobmapdata->page = 0;
	jobmapdata->num_entries = 0;
	jobmapdata->entries = next_entry = malloc(sizeof(struct JOBMAP_ENTRY) * numairports);
	for (i = 0; i < numairports; i++) {
		if (airports[i].missiontypes & mission_type && msp->ap != &airports[i]) {
			dx = msp->pos.x - airports[i].pos.x;
			dy = msp->pos.y - airports[i].pos.y;
			next_entry->airport = airports + i;
			next_entry->distance = (int) sqrt(dx * dx + dy * dy);
			next_entry++;
			jobmapdata->num_entries++;
		}
	}
	if (jobmapdata->num_entries) {
		jobmapdata->selected_entry = 0;
	} else {
		jobmapdata->selected_entry = JOBMAP_ENTRY_SORT;
	}
	jobmapdata->max_page = (jobmapdata->num_entries - 1) / JOBMAP_NUM_ENTRIES_PERPAGE;
	if (jobmapdata->max_page) {
		td_jobmap_nextpage.rpcdata->font_color = COL_PANEL_ROW_NORMAL;
	} else {
		td_jobmap_nextpage.rpcdata->font_color = COL_PANEL_ROW_DISABLED;
	}
	td_jobmap_prevpage.rpcdata->font_color = COL_PANEL_ROW_DISABLED;
	textdraws_strcpy(&td_jobmap_sort, jobmap_sort_text[jobmapdata->sort]);
	td_jobmap_header.rpcdata->text_length = jobmap_format_header(td_jobmap_header.rpcdata->text, jobmapdata);
	jobmap_resort_entries(jobmapdata);
	jobmap_format_entries(
		td_jobmap_opts.rpcdata->text, &td_jobmap_opts.rpcdata->text_length,
		td_jobmap_optdists.rpcdata->text, &td_jobmap_optdists.rpcdata->text_length,
		jobmapdata
	);

	/*Include mission type name in header.*/
	for (i = 0;; i++) {
		if (i >= SETTING__NUM_MISSION_TYPES) {
			logprintf("failed to construct mission map title for msp id %d vehiclemodel %d class %d",
				msp->id, GetVehicleModel(GetPlayerVehicleID(playerid)), classid[playerid]);
			textdraws_strcpy(&td_jobmap_title, "Flights");
			break;
		}
		if ((mission_type >> i) & 1) {
			td_jobmap_title.rpcdata->text_length =
				sprintf(td_jobmap_title.rpcdata->text, "%s Flights", mission_type_names[i]);
			break;
		}
	}

#if NUM_MAP_TEXTDRAWS - 2 != 11
#error
#endif
	/*Showing all but the two selection textdraws.*/
	textdraws_show(playerid, 11, &td_jobmap_mapbg, &td_jobmap_bg,
		&td_jobmap_sort,
		&td_jobmap_prevpage, &td_jobmap_nextpage,
		&td_jobmap_keyhelp, &td_jobmap_title,
		&td_jobmap_header, &td_jobmap_header_right,
		&td_jobmap_opts, &td_jobmap_optdists);

	/*'from' textdraw properties*/
	strcpy(td_jobmap_from.rpcdata->text, msp->ap->code);
	td_jobmap_from.rpcdata->x = map_text_x_base + map_text_x_unit * (msp->pos.x + JOBMAP_HALFSIZE);
	td_jobmap_from.rpcdata->y = map_text_y_base + map_text_y_unit * (-msp->pos.y + JOBMAP_HALFSIZE);

#if NUM_MISSIONMAP_LOCATIONS_TEXTDRAWS - 1 != 8
#error
#endif
	/*Showing all but "to" textdraws.*/
	textdraws_show(playerid, 8,
		&td_jobmap_island_mainland, &td_jobmap_island_octa, &td_jobmap_island_cata,
		&td_jobmap_island_igzu, &td_jobmap_island_bnsa, &td_jobmap_island_dune,
		&td_jobmap_island_strs,
		&td_jobmap_from);

	jobmap_update_selection_textdraws(playerid, jobmapdata);

	kneeboard_hide(playerid, KNEEBOARD_HIDE_REASON_JOBMAP);
	panel_hide(playerid, PANEL_HIDE_REASON_JOBMAP);
}

static
void jobmap_dispose()
{
	free(td_jobmap_keyhelp.rpcdata);
	free(td_jobmap_title.rpcdata);
	free(td_jobmap_bg.rpcdata);
	free(td_jobmap_header.rpcdata);
	free(td_jobmap_header_right.rpcdata);
	free(td_jobmap_mapbg.rpcdata);
	free(td_jobmap_sort.rpcdata);
	free(td_jobmap_prevpage.rpcdata);
	free(td_jobmap_nextpage.rpcdata);
	free(td_jobmap_opts.rpcdata);
	free(td_jobmap_optdists.rpcdata);
	free(td_jobmap_selection_left.rpcdata);
	free(td_jobmap_selection_right.rpcdata);

	free(td_jobmap_island_mainland.rpcdata);
	free(td_jobmap_island_octa.rpcdata);
	free(td_jobmap_island_cata.rpcdata);
	free(td_jobmap_island_igzu.rpcdata);
	free(td_jobmap_island_bnsa.rpcdata);
	free(td_jobmap_island_dune.rpcdata);
	free(td_jobmap_island_strs.rpcdata);
	free(td_jobmap_from.rpcdata);
	free(td_jobmap_to.rpcdata);
}

/**
Sizes and positions the textdraws that are used for their box to indicate islands on the jobmap.
*/
static
void jobmap_set_island_textdraw_properties()
{
	float map_coords_fontheight_base, map_coords_fontheight_unit;
	float map_coords_y_base, map_coords_y_unit;
	float map_coords_x_base, map_coords_x_unit;
	float boxwidth_unit;
	float boxwidth_min, boxwidth_max;
	struct RPCDATA_ShowTextDraw *txtdraw;
	struct ZONE *zone;
	int zoneindex, maxzoneindex;
	int regionindex;
	int octa_region_found, cata_region_found, igzu_region_found, bnsa_region_found, dune_region_found, strs_region_found;
	float min_x, min_y, max_x, max_y;

	map_text_x_base = td_jobmap_from.rpcdata->x;
	map_text_x_unit = (td_jobmap_to.rpcdata->x - td_jobmap_from.rpcdata->x) / JOBMAP_SIZE;

	map_text_y_base = td_jobmap_from.rpcdata->y;
	map_text_y_unit = (td_jobmap_to.rpcdata->y - td_jobmap_from.rpcdata->y) / JOBMAP_SIZE;

	map_coords_x_base = td_jobmap_island_octa.rpcdata->x;
	map_coords_x_unit = (td_jobmap_island_cata.rpcdata->x - td_jobmap_island_octa.rpcdata->x) / JOBMAP_SIZE;

	map_coords_y_base = td_jobmap_island_octa.rpcdata->y;
	map_coords_y_unit = (td_jobmap_island_cata.rpcdata->y - td_jobmap_island_octa.rpcdata->y) / JOBMAP_SIZE;

	/*actual box width is the relation of x (font position) to box_width*/
	boxwidth_min = td_jobmap_island_octa.rpcdata->box_width - td_jobmap_island_octa.rpcdata->x;
	boxwidth_max = td_jobmap_island_mainland.rpcdata->box_width - td_jobmap_island_mainland.rpcdata->x;
	boxwidth_unit = (boxwidth_max - boxwidth_min) / JOBMAP_SIZE;

	map_coords_fontheight_base = td_jobmap_island_octa.rpcdata->font_height;
	map_coords_fontheight_unit = (td_jobmap_island_mainland.rpcdata->font_height - map_coords_fontheight_base) / JOBMAP_SIZE;

	td_jobmap_island_mainland.rpcdata->x = map_coords_x_base + map_coords_x_unit * (JOBMAP_HALFSIZE - 3000.0f);
	td_jobmap_island_mainland.rpcdata->box_width = td_jobmap_island_mainland.rpcdata->x + boxwidth_unit * 6000.0f;
	td_jobmap_island_mainland.rpcdata->y = map_coords_y_base + map_coords_y_unit * (JOBMAP_HALFSIZE - 3000.0f);
	td_jobmap_island_mainland.rpcdata->font_height = map_coords_fontheight_base + map_coords_fontheight_unit * 6000.0f;

	octa_region_found = 0;
	cata_region_found = 0;
	igzu_region_found = 0;
	bnsa_region_found = 0;
	dune_region_found = 0;
	strs_region_found = 0;
	for (regionindex = 0; regionindex < regioncount; regionindex++) {
		switch (regions[regionindex].zone.id) {
		case ZONE_OCTA:
			octa_region_found = 1;
			txtdraw = td_jobmap_island_octa.rpcdata;
			break;
		case ZONE_CATA:
			cata_region_found = 1;
			txtdraw = td_jobmap_island_cata.rpcdata;
			break;
		case ZONE_IGZU:
			igzu_region_found = 1;
			txtdraw = td_jobmap_island_igzu.rpcdata;
			break;
		case ZONE_BNSA:
			bnsa_region_found = 1;
			txtdraw = td_jobmap_island_bnsa.rpcdata;
			/*bnsa is too small, so using region coords*/
			min_x = regions[regionindex].zone.min_x;
			min_y = regions[regionindex].zone.min_y;
			max_x = regions[regionindex].zone.max_x;
			max_y = regions[regionindex].zone.max_y;
			goto have_coords;
		case ZONE_DUNE:
			dune_region_found = 1;
			txtdraw = td_jobmap_island_dune.rpcdata;
			break;
		case ZONE_STRS:
			strs_region_found = 1;
			txtdraw = td_jobmap_island_strs.rpcdata;
			break;
		default:
			continue;
		}
		zoneindex = regions[regionindex].from_zone_idx;
		maxzoneindex = regions[regionindex].to_zone_idx_exclusive;
		min_x = min_y = float_pinf;
		max_x = max_y = float_ninf;
		for (; zoneindex < maxzoneindex; zoneindex++) {
			zone = &zones[zoneindex];
			if (zone->min_x < min_x) {
				min_x = zone->min_x;
			}
			if (zone->min_y < min_y) {
				min_y = zone->min_y;
			}
			if (zone->max_x > max_x) {
				max_x = zone->max_x;
			}
			if (zone->max_y > max_y) {
				max_y = zone->max_y;
			}
		}
have_coords:
		assert(min_x < max_x);
		assert(min_y < max_y);

		txtdraw->x = map_coords_x_base + map_coords_x_unit * (40000.0f + min_x);
		txtdraw->box_width = txtdraw->x + boxwidth_unit * (max_x - min_x);
		txtdraw->y = map_coords_y_base + map_coords_y_unit * (40000.0f + -max_y);
		txtdraw->font_height = map_coords_fontheight_base + map_coords_fontheight_unit * (max_y - min_y);
	}
	assert(octa_region_found);
	assert(cata_region_found);
	assert(igzu_region_found);
	assert(bnsa_region_found);
	assert(dune_region_found);
	assert(strs_region_found);
}

static
void jobmap_init()
{
#if NUM_MAP_TEXTDRAWS != 13
#error
#endif
	textdraws_load_from_file("jobmap", TEXTDRAW_MISSIONMAP_BASE, NUM_MAP_TEXTDRAWS,
		/*remember to free their rpcdata in missions_dispose*/
		&td_jobmap_keyhelp, &td_jobmap_title, &td_jobmap_bg,
		&td_jobmap_header, &td_jobmap_header_right,
		&td_jobmap_mapbg, &td_jobmap_sort,
		&td_jobmap_prevpage, &td_jobmap_nextpage,
		&td_jobmap_opts, &td_jobmap_optdists,
		&td_jobmap_selection_left, &td_jobmap_selection_right);
	textdraws_set_textbox_properties(td_jobmap_keyhelp.rpcdata);
	map_list_entry_line_height =
		(td_jobmap_selection_right.rpcdata->y - td_jobmap_selection_left.rpcdata->y) / (JOBMAP_NUM_ENTRIES_PERPAGE - 1);
	strcpy(td_jobmap_header.rpcdata->text, "_");

#if NUM_MISSIONMAP_LOCATIONS_TEXTDRAWS_INFILE != 5
#error
#endif
	textdraws_load_from_file("jobmap", TEXTDRAW_MISSIONMAP_LOCATIONS_BASE_FROMFILE, NUM_MISSIONMAP_LOCATIONS_TEXTDRAWS_INFILE,
		/*remember to free their rpcdata in missions_dispose*/
		&td_jobmap_island_cata, &td_jobmap_island_octa,
		&td_jobmap_island_mainland, &td_jobmap_from, &td_jobmap_to);
	textdraws_initialize_from(&td_jobmap_island_cata, &td_jobmap_island_igzu, TEXTDRAW_MISSIONMAP_ISLAND_IGZU);
	textdraws_initialize_from(&td_jobmap_island_cata, &td_jobmap_island_bnsa, TEXTDRAW_MISSIONMAP_ISLAND_BNSA);
	textdraws_initialize_from(&td_jobmap_island_cata, &td_jobmap_island_dune, TEXTDRAW_MISSIONMAP_ISLAND_DUNE);
	textdraws_initialize_from(&td_jobmap_island_cata, &td_jobmap_island_strs, TEXTDRAW_MISSIONMAP_ISLAND_STRS);

	jobmap_set_island_textdraw_properties();
}

static
void jobmap_on_player_connect(int playerid)
{
	jobmapdatas[playerid].sort = JOBMAP_SORT_NAME_ASCENDING;
	jobmapdatas[playerid].entries = NULL;
}

static
void jobmap_on_player_disconnect(int playerid)
{
	if (jobmapdatas[playerid].entries) {
		free(jobmapdatas[playerid].entries);
		jobmapdatas[playerid].entries = NULL;
	}
}
