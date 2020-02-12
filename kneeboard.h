
/* vim: set filetype=c ts=8 noexpandtab: */

/**
Player textdraw for the header.

This could technically be a global textdraw, but then it draws behind the
player textdraws, which is not wanted.
*/
extern int kneeboard_ptxt_header[MAX_PLAYERS];
/**
Player textdraw for the distance (this one also does the background).
*/
extern int kneeboard_ptxt_distance[MAX_PLAYERS];
/**
Player textdraw for the body.
*/
extern int kneeboard_ptxt_info[MAX_PLAYERS];

/**
Creates the player textdraws for the kneeboard panel.
*/
void kneeboard_create_player_text(int playerid);
/**
Resets and shows the kneeboard for given player.
*/
void kneeboard_reset_show(int playerid);
/**
Updates the distance indicator in the kneeboard for given player.

Should be called by the panel.
*/
void kneeboard_update_distance(int playerid, struct vec3 *playerpos);
