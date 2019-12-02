
/* vim: set filetype=c ts=8 noexpandtab: */

/**
Get amount of money player has.
*/
int money_get(int playerid);
/**
Gives money to a player.

Will not actually give anything if it would cause an overflow.

@param amount amount to give (negative will take money)
@return 0 if it would cause overflow
*/
int money_give(int playerid, int amount);
/**
Used to set the player their money, to combat random -$100 on spawn.
*/
void money_on_player_spawn(int playerid);
/**
Take money from a player.

Will not actually give anything if it would cause an underflow.

@param amount amount to take (negative will give money)
@return 0 if it would cause underflow
*/
int money_take(int playerid, int amount);
/**
Set a player's money.
*/
void money_set(int playerid, int amount);
