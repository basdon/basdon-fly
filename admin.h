
/* vim: set filetype=c ts=8 noexpandtab: */

/**
Dialog response handler for airport assignment //vehinfo sub-dialog
*/
void admin_dlg_vehinfo_assign_response(int pid, int res, int listitem);
/**
Dialog response handler for base //vehinfo dialog
*/
void admin_dlg_vehinfo_response(int pid, int res, int listitem);
/**
The //getcar cmd teleports a vehicle to the player's location.
*/
int cmd_admin_getcar(CMDPARAMS);
/**
The //respawn cmd respawns the vehicle the player is in.
*/
int cmd_admin_respawn(CMDPARAMS);
/**
The //rr commands respawns all unoccupied vehicles in a close range.
*/
int cmd_admin_rr(CMDPARAMS);
/**
The //vehinfo cmd shows vehicle info

Also allows editing enabled state and linked airport.
*/
int cmd_admin_vehinfo(CMDPARAMS);