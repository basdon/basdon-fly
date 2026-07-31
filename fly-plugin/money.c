/**
Get amount of money player has.
*/
static
int money_get(int playerid)
{
	TRACE;

	return playerpool->playerMoney[playerid];
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
	TRACE;

	if (amount < 0) {
		return money_take(playerid, -amount);
	}
	/*prevent overflow*/
	if (playerpool->playerMoney[playerid] + amount < playerpool->playerMoney[playerid]) {
		return 0;
	}
	playerpool->playerMoney[playerid] += amount;
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
	TRACE;

	if (amount < 0) {
		return money_give(playerid, -amount);
	}
	/*prevent underflow*/
	if (playerpool->playerMoney[playerid] - amount > playerpool->playerMoney[playerid]) {
		return 0;
	}
	playerpool->playerMoney[playerid] -= amount;
	GivePlayerMoneyRaw(playerid, -amount);
	return 1;
}

/**
Set a player's money.
*/
static
void money_set(int playerid, int amount)
{
	TRACE;

	SetPlayerMoneyRaw(playerid, playerpool->playerMoney[playerid] = amount);
}
