
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "money.h"

/**
Money per player.
*/
static int playermoney[MAX_PLAYERS];

int money_give(int playerid, int amount)
{
	if (amount < 0) {
		return money_take(playerid, -amount);
	}
	/*prevent overflow*/
	if (playermoney[playerid] + amount < playermoney[playerid]) {
		return 0;
	}
	playermoney[playerid] += amount;
	NC_PARS(2);
	nc_params[1] = playerid;
	nc_params[2] = amount;
	NC(n_GivePlayerMoney_);
	return 1;
}

int money_take(int playerid, int amount)
{
	if (amount < 0) {
		return money_give(playerid, -amount);
	}
	/*prevent underflow*/
	if (playermoney[playerid] - amount > playermoney[playerid]) {
		return 0;
	}
	playermoney[playerid] -= amount;
	NC_PARS(2);
	nc_params[1] = playerid;
	nc_params[2] = -amount;
	NC(n_GivePlayerMoney_);
	return 1;
}

void money_set(int playerid, int amount)
{
	NC_PARS(1);
	nc_params[1] = playerid;
	NC(n_ResetPlayerMoney_);
	NC_PARS(2);
	nc_params[2] = amount;
	NC(n_GivePlayerMoney_);
	playermoney[playerid] = amount;
}
