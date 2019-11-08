
/* vim: set filetype=c ts=8 noexpandtab: */

/**
The /adf cmd.

Syntax: /adf [beacon]
Example: /adf ls
When no beacon, disable nav
*/
int nav_cmd_adf(CMDPARAMS);
/**
The /ils command

Syntax: /ils
Only toggles ils when VOR is already active and the runway has ILS capabilities.
*/
int nav_cmd_ils(CMDPARAMS);
/**
The /vor cmd.

Syntax: /vor [beacon][runway]
Example: /vor ls09l
Optional whitespace between beacon and runway
When no beacon, disable nav
When no or invalid runway, print a list of valid runways
*/
int nav_cmd_vor(CMDPARAMS);
/**
Disables navigation for given vehicle.

Ensures the VOR textdraws are hidden.
*/
void nav_disable(AMX*, int vehicleid);
/**
Enables navigation for given vehicle.

Ensures the VOR textdraws are shown/hidden.
Resets nav cache for all players in vehicle.

@param adf vec3 to ADF towards, may be NULL
@param vor runway to VOR towards, may be NULL
*/
void nav_enable(AMX*, int vehicleid, struct vec3 *adf, struct RUNWAY *vor);
/**
May return NAV_NONE, NAV_ADF, NAV_VOR or NAV_VOR|NAV_ILS
*/
int nav_get_active_type(int vehicleid);
void nav_init();
/**
Set given vehicle's navigation to given airport.

Navigation target is decided by the runway or heliport (decision based on
given vehiclemodel) that is the closest to the vehicle.
When no target is found, airport's ADF beacon will be used.
*/
void nav_navigate_to_airport(AMX*, int vid, int vmodel, struct AIRPORT *ap);
/**
Reset cache so textdraws will be updated next timer tick.
*/
void nav_reset_cache(int playerid);
/**
Resets (disables) navigation for given vehicle. Does not hide textdraws.
*/
void nav_reset_for_vehicle(int vehicleid);
/**
Update nav values to be displayed.

Does not display data, see nav_update_textdraws for that.
*/
void nav_update(AMX *amx, int vehicleid, struct vec3 *pos, float heading);
/**
Update nav related textdraws in panel for player if needed.

Does not actually update the nav data (see nav_update for that), just updates
the textdraws.
*/
void nav_update_textdraws(
	AMX*, int playerid, int vehicleid,
	int *ptxt_adf_dis_base, int *ptxt_adf_alt_base, int *ptxt_adf_crs_base,
	int *ptxt_vor_base, int *ptxt_ils_base);