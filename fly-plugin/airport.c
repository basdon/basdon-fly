/**
Clears all data and frees all allocated memory.
*/
static
void airports_destroy()
{
	struct AIRPORT *ap = airports;

	while (numairports) {
		numairports--;
		free(ap->runways);
		ap++;
	}
        free(airports);
	airports = NULL;
}

#ifdef AIRPORT_PRINT_STATS
static
void airports_print_stats()
{
	struct AIRPORT *ap;
	struct RUNWAY *rnw;
	struct MISSIONPOINT *msp;
	int i, j;

	ap = airports;
	for (i = 0; i < numairports; i++) {
		printf("airport %d: %s code %s\n", ap->id, ap->name, ap->code);
		printf(" runwaycount: %d\n", ap->runwaysend - ap->runways);
		rnw = ap->runways;
		while (rnw != ap->runwaysend) {
			printf("  runway %s type %d nav %d\n", rnw->id, rnw->type, rnw->nav);
			rnw++;
		}
		printf(" missionpoints: %d types: %d\n", ap->num_missionpts, ap->missiontypes);
		msp = ap->missionpoints;
		for (j = 0; j < ap->num_missionpts; j++) {
			printf("  missionpoint %d: %s type %d point_type %d\n", msp->id, msp->name, msp->type, msp->point_type);
			msp++;
		}
		ap++;
	}
}
#endif

/**
Loads airports/runways/missionpoints (and init other things).
*/
static
void airports_init()
{
	struct AIRPORT *ap;
	struct RUNWAY *rnw, *prev_rnw;
	struct MISSIONPOINT *msp;
	float dx, dy;
	int cacheid, rowcount, lastap, *field = nc_params + 2;
	int i, j, airportid;

	/*load airports*/
	atoc(buf144, "SELECT i,c,n,e,x,y,z,flags FROM apt ORDER BY i ASC", 144);
	cacheid = NC_mysql_query(buf144a);
	numairports = NC_cache_get_row_count();
	if (!numairports) {
		assert(((void) "no airports", 0));
	} else if (numairports > MAX_AIRPORTS) {
		assert(((void) "too many airports", 0));
	}
	airports = malloc(sizeof(struct AIRPORT) * numairports);
	nc_params[3] = buf144a;
	for (i = 0; i < numairports; i++) {
		ap = &airports[i];
		ap->runways = ap->runwaysend = NULL;
		ap->missionpoints = NULL;
		ap->num_missionpts = 0;
		ap->missiontypes = 0;
		NC_PARS(3);
		nc_params[1] = i;
		ap->id = (*field = 0, NC(n_cache_get_field_i));
		*field = 1; NC(n_cache_get_field_s);
		ctoa(ap->code, buf144, sizeof(ap->code));
		*field = 2; NC(n_cache_get_field_s);
		ctoa(ap->name, buf144, sizeof(ap->name));
		NC_PARS(2);
		ap->enabled = (char) (*field = 3, NC(n_cache_get_field_i));
		ap->pos.x = (*field = 4, NCF(n_cache_get_field_f));
		ap->pos.y = (*field = 5, NCF(n_cache_get_field_f));
		ap->pos.z = (*field = 6, NCF(n_cache_get_field_f));
		ap->flags = (*field = 7, NC(n_cache_get_field_i));
	}
	NC_cache_delete(cacheid);

	/*TODO make this into one big array aswell?*/
	/*load runways*/
	B144("SELECT a,x,y,z,n,type,h,s,i FROM rnw ORDER BY a ASC, i ASC");
	cacheid = NC_mysql_query(buf144a);
	rowcount = NC_cache_get_row_count();
	if (!rowcount) {
		assert(((void) "no runways", 0));
	}
	rowcount--;
	while (rowcount >= 0) {
		lastap = NC_cache_get_field_int(rowcount, 0);
		ap = airports + lastap;

		/*look 'ahead' to see how many runways there are*/
		i = rowcount - 1;
		while (i-- > 0) {
			nc_params[1] = i;
			if (NC(n_cache_get_field_i) != lastap) {
				break;
			}
		}

		/*gottem*/
		i = rowcount - i;
		ap->runways = rnw = malloc(sizeof(struct RUNWAY) * i);
		ap->runwaysend = ap->runways + i;
		nc_params[1] = rowcount;
		nc_params[3] = buf32a;
		prev_rnw = NULL;
		while (i--) {
			NC_PARS(2);
			nc_params[1] = rowcount;
			rnw->pos.x = (*field = 1, NCF(n_cache_get_field_f));
			rnw->pos.y = (*field = 2, NCF(n_cache_get_field_f));
			rnw->pos.z = (*field = 3, NCF(n_cache_get_field_f));
			rnw->nav = (*field = 4, NC(n_cache_get_field_i));
			rnw->type = (*field = 5, NC(n_cache_get_field_i));
			rnw->heading = (*field = 6, NCF(n_cache_get_field_f));
			rnw->headingr = (360.0f - rnw->heading + 90.0f) * DEG_TO_RAD;
			rnw->other_end = NULL;
			rnw->length = 0;
			rnw->identifier = (*field = 8, NC(n_cache_get_field_i));
			NC_PARS(3);
			*field = 7; NC(n_cache_get_field_s);
			sprintf(rnw->id, "%02.0f", (float) ceil(rnw->heading / 10.0f));
			rnw->id[2] = (char) buf32[0];
			rnw->id[3] = 0;
			if (prev_rnw && prev_rnw->identifier == rnw->identifier) {
				prev_rnw->other_end = rnw;
				rnw->other_end = prev_rnw;
				dx = rnw->pos.x - prev_rnw->pos.x;
				dy = rnw->pos.y - prev_rnw->pos.y;
				rnw->length = prev_rnw->length = (unsigned short) sqrt(dx * dx + dy * dy);
			}
			prev_rnw = rnw;
			rnw++;
			rowcount--;
		}
	}
	NC_cache_delete(cacheid);

	/*load missionpoints*/
	/*They _HAVE_ to be ordered by airport.*/
	atoc(buf144, "SELECT a,i,x,y,z,t,name FROM msp ORDER BY a ASC,i ASC", 144);
	cacheid = NC_mysql_query(buf144a);
	nummissionpoints = NC_cache_get_row_count();
	if (!nummissionpoints) {
		assert(((void) "no missionpoints", 0));
	}
	missionpoints = malloc(sizeof(struct MISSIONPOINT) * nummissionpoints);
	ap = NULL;
	nc_params[3] = buf32a;
	for (i = 0; i < nummissionpoints; i++) {
		msp = &missionpoints[i];
		NC_PARS(2);
		nc_params[1] = i;
		airportid = (*field = 0, NC(n_cache_get_field_i));
		if (!ap || ap->id != airportid) {
			for (j = 0; j < numairports; j++) {
				if (airports[j].id == airportid) {
					ap = &airports[j];
					goto have_airport;
				}
			}
			/*This should never happen, as per database scheme.*/
			assert(((void) "failed to link msp to ap", 0));
have_airport:
			ap->missionpoints = msp;
		}
		ap->num_missionpts++;

		if (ap->num_missionpts > MAX_MISSIONPOINTS_PER_AIRPORT) {
			assert(((void) "too many missionpoints", 0));
		}

		msp->ap = ap;
		msp->id = (unsigned short) (*field = 1, NC(n_cache_get_field_i));
		msp->pos.x = (*field = 2, NCF(n_cache_get_field_f));
		msp->pos.y = (*field = 3, NCF(n_cache_get_field_f));
		msp->pos.z = (*field = 4, NCF(n_cache_get_field_f));
		msp->type = (*field = 5, NC(n_cache_get_field_i));
		ap->missiontypes |= msp->type;
		if (msp->type & PASSENGER_MISSIONTYPES) {
			msp->point_type = MISSION_POINT_PASSENGERS;
			if (msp->type & ~PASSENGER_MISSIONTYPES) {
mixed_missionpoints:
				logprintf("mixed missionpoint types msp id %d ap %s", msp->id, ap->name);
				assert(((void) "mixed missionpoint types", 0));
			}
		} else if (msp->type & CARGO_MISSIONTYPES) {
			msp->point_type = MISSION_POINT_CARGO;
			if (msp->type & ~CARGO_MISSIONTYPES) {
				goto mixed_missionpoints;
			}
		} else {
			msp->point_type = MISSION_POINT_SPECIAL;
		}
		NC_PARS(3);
		*field = 6; NC(n_cache_get_field_s);
		ctoa(msp->name, buf32, MAX_MSP_NAME + 1);
	}
	NC_cache_delete(cacheid);

#ifdef AIRPORT_PRINT_STATS
	airports_print_stats();
#endif
}

#pragma pack(push,1)
/**To be used when the 'nearest' dialog is shown, so dialog response can be mapped to the data that was shown.*/
struct NEAREST_AIRPORT_DATA {
	int num_nearest;
	/**Actually arbitrary size.*/
	struct AIRPORT *nearest[1];
};
#pragma pack(pop)

/**
Call when getting a response from DIALOG_AIRPORT_NEAREST, to show info dialog.
*/
static
void cb_airport_nearest_dialog_response(int playerid, struct DIALOG_RESPONSE response)
{
	register struct NEAREST_AIRPORT_DATA *data;
	struct DIALOG_INFO dialog;
	struct AIRPORT *ap;
	struct RUNWAY *rnw;
	int helipads;
	char *info;
	char hasrunways;

	data = response.data;
	if (response.aborted || !response.response ||
		response.listitem < 0 || response.listitem >= data->num_nearest)
	{
		goto freereturn;
	}
	ap = data->nearest[response.listitem];

	dialog_init_info(&dialog);
	sprintf(dialog.caption, "%s - %s", ap->code, ap->name);
	info = dialog.info;
	info += sprintf(info, "Elevation:\t%.0f FT", ap->pos.z);
	rnw = ap->runways;
	hasrunways = 0;
	helipads = 0;
	while (rnw != ap->runwaysend) {
		if (rnw->type == RUNWAY_TYPE_HELIPAD) {
			helipads++;
		} else {
			if (!hasrunways) {
				hasrunways = 1;
				info += sprintf(info, "\nRunways:\t%s (%dm)", rnw->id, rnw->length);
			} else {
				info += sprintf(info, "\n\t\t%s (%dm)", rnw->id, rnw->length);
			}
			if (rnw->nav == (NAV_VOR | NAV_ILS)) {
				info += sprintf(info, " (VOR+ILS)");
			} else if (rnw->nav) {
				info += sprintf(info, " (VOR)");
			}
		}
		rnw++;
	}
	info += sprintf(info, "\nHelipads:\t%d", helipads);

	dialog.transactionid = DLG_TID_AIRPORT_NEAREST_DETAIL;
	dialog.button1 = "Close";
	dialog_show(playerid, &dialog);

freereturn:
	free(response.data);
}

/**
Structure used for sorting airports by distance for the /nearest list dialog.
*/
struct APREF {
	float distance;
	struct AIRPORT *ap;
};

/**
Sort function used to sort airports by distance for the /nearest list dialog.
*/
static
int sortaprefs(const void *_a, const void *_b)
{
	struct APREF *a = (struct APREF*) _a, *b = (struct APREF*) _b;
	return (int) (10.0f * (a->distance - b->distance));
}

static
int airport_show_nearest_dialog(int playerid)
{
	register struct AIRPORT *ap;
	struct NEAREST_AIRPORT_DATA *data;
	struct DIALOG_INFO dialog;
	int i;
	int num;
	struct vec3 playerpos;
	float dx, dy;
	struct APREF *aps;
	char *info;

	GetPlayerPos(playerid, &playerpos);
	/**Kinda stupid this extra checks is needed to exclude disabled airports.
	TODO Probably happens on a lot of places, separate disabled/enabled airports at load thx.*/
	aps = alloca(sizeof(struct APREF) * numairports);
	num = i = 0;
	while (i < numairports) {
		if (airports[i].enabled) {
			dx = airports[i].pos.x - playerpos.x;
			dy = airports[i].pos.y - playerpos.y;
			aps[num].distance = (float) sqrt(dx * dx + dy * dy);
			aps[num].ap = &airports[i];
			num++;
		}
		i++;
	}
	if (num == 0) {
		SendClientMessage(playerid, COL_WARN, WARN"No airports!");
		return 1;
	}
	qsort(aps, num, sizeof(struct APREF), sortaprefs);
	/**This is one ptr too much, but that's fine. Don't feel like putting that extra -1.*/
	data = malloc(sizeof(struct NEAREST_AIRPORT_DATA) + sizeof(struct AIRPORT*) * num);
	dialog.handler.data = data;
	data->num_nearest = num;

	dialog_init_info(&dialog);
	info = dialog.info;
	for (i = 0; i < num; i++) {
		if (aps[i].distance < 1000.0f) {
			info += sprintf(info, "%.0f", aps[i].distance);
		} else {
			info += sprintf(info, "%.1fK", aps[i].distance / 1000.0f);
		}
		ap = aps[i].ap;
		data->nearest[i] = ap;
		info += sprintf(info, "\t%s\t[%s]\n", ap->name, ap->code);
	}

	dialog.transactionid = DLG_TID_AIRPORT_NEAREST;
	dialog.style = DIALOG_STYLE_TABLIST;
	dialog.handler.options = DLG_OPT_NOTIFY_ABORTED;
	dialog.handler.callback = cb_airport_nearest_dialog_response;
	dialog.handler.data = data;
	dialog.caption = "Nearest airports";
	dialog.button1 = "Info";
	dialog.button2 = "Close";
	dialog_show(playerid, &dialog);
	return 1;
}
