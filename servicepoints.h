
/* vim: set filetype=c ts=8 noexpandtab: */

/**
Free memory
*/
void svp_dispose();
/**
Load servicepoints from the db.
*/
void svp_init();
/**
Reset servicepoint map icon streaming for new player.
*/
void svp_on_player_connect(int playerid);
/**
Updates the service point mapicons (and 3D text) for given playerid.

@param x x-position of the player
@param y y-position of the player
*/
void svp_update_mapicons(int playerid, float x, float y);
