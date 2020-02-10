
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "kneeboard.h"
#include <string.h>

/**
Global textdraw for the header.
*/
int gtxt_header;
/**
Player textdraw for the distance (this one also does the background).
*/
int ptxt_distance[MAX_PLAYERS];
/**
Player textdraw for the body.
*/
int ptxt_info[MAX_PLAYERS];

void kneeboard_create_global_text()
{
	B144("~w~Kneeboard");
	NC_PARS(3);
	nc_paramf[1] = 532.0f;
	nc_paramf[2] = 230.0f;
	nc_params[3] = buf144a;
	gtxt_header = nc_params[1] = NC(n_TextDrawCreate);
	nc_paramf[2] = 0.5f;
	nc_paramf[3] = 1.7f;
	NC(n_TextDrawLetterSize);
	NC_PARS(2);
	nc_params[2] = 1;
	NC(n_TextDrawSetOutline);
	nc_params[2] = 0;
	NC(n_TextDrawFont);
}

void kneeboard_create_player_text(int playerid)
{
	B144("_");

	NC_PARS(4);
	nc_params[1] = playerid;
	nc_paramf[2] = 525.0f;
	nc_paramf[3] = 240.0f;
	nc_params[4] = buf144a;
	ptxt_distance[playerid] = nc_params[2] = NC(n_CreatePlayerTextDraw);
	nc_paramf[3] = 0.25f;
	nc_paramf[4] = 1.0f;
	NC(n_PlayerTextDrawLetterSize);
	nc_paramf[3] = 615.0f;
	nc_paramf[4] = 100.0f;
	NC(n_PlayerTextDrawLetterSize);
	NC_PARS(3);
	nc_params[3] = 1;
	NC(n_PlayerTextDrawFont);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawUseBox);
	nc_params[3] = 0x00000077; /*should be same as PANEL_BG*/
	NC(n_PlayerTextDrawBoxColor);

	NC_PARS(4);
	nc_params[1] = playerid;
	nc_paramf[2] = 525.0f;
	nc_paramf[3] = 250.0f;
	nc_params[4] = buf144a;
	ptxt_distance[playerid] = nc_params[2] = NC(n_CreatePlayerTextDraw);
	nc_paramf[3] = 0.25f;
	nc_paramf[4] = 1.0f;
	NC(n_PlayerTextDrawLetterSize);
	nc_paramf[3] = 615.0f;
	nc_paramf[4] = 100.0f;
	NC(n_PlayerTextDrawLetterSize);
	NC_PARS(3);
	nc_params[3] = 1;
	NC(n_PlayerTextDrawFont);
	NC(n_PlayerTextDrawSetShadow);
	NC(n_PlayerTextDrawUseBox);
	nc_params[3] = 0;
	NC(n_PlayerTextDrawBoxColor);
}
