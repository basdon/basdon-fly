/**
Money per player.
TODO: check if we can use playerpool->playerMoney (need to check if client messages affects its value)
*/
static int playermoney[MAX_PLAYERS];

/**
Get amount of money player has.
*/
static
int money_get(int playerid)
{
	return playermoney[playerid];
}

static int money_take(int, int);

/**
Gives money to a player.

Will not actually give anything if it would cause an overflow.

@param amount amount to give (negative will take money)
@return 0 if it would cause overflow
*/
static
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
	GivePlayerMoneyRaw(playerid, amount);
	return 1;
}

/**
Take money from a player.

Will not actually give anything if it would cause an underflow.

@param amount amount to take (negative will give money)
@return 0 if it would cause underflow
*/
static
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
	GivePlayerMoneyRaw(playerid, -amount);
	return 1;
}

/**
Used to set the player their money, to combat random -$100 on spawn.
*/
static
void money_on_player_spawn(int playerid)
{
	/*spawning might take $100 randomly, so reset it here just in case*/
	SetPlayerMoneyRaw(playerid, playermoney[playerid]);
}

/**
Set a player's money.
*/
static
void money_set(int playerid, int amount)
{
	SetPlayerMoneyRaw(playerid, playermoney[playerid] = amount);
}
