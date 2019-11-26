
// vim: set filetype=c ts=8 noexpandtab:

#namespace "basdon.c"

//@summary Plugin callback for mysql/bcrypt callback
native B_OnCallbackHit(function, data)

//@summary Plugin callback for {@link B_OnDialogResponse}
//@returns {@code 0} if further processing should be cancelled
native B_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])

//@summary Plugin callback for {@link OnGameModeExit}
//@remarks call before closing db
native B_OnGameModeExit()

//@summary Plugin callback for {@link OnGameModeInit}
//@remarks call after db has been initialized
native B_OnGameModeInit()

//@summary Plugin callback for {@link OnPlayerCommandText}
//@remarks will call {@link OnPlayerCommandTextHash}
native B_OnPlayerCommandText(playerid, cmdtext[])

//@summary Plugin callback for {@link OnPlayerConnect}
native B_OnPlayerConnect(playerid)

//@summary Plugin callback for {@link OnPlayerDeath}
native B_OnPlayerDeath(playerid, killerid, reason)

//@summary Plugin callback for {@link OnPlayerDisconnect}
native B_OnPlayerDisconnect(playerid, reason)

//@summary Plugin callback for {@link OnPlayerEnterRaceCP}
native B_OnPlayerEnterRaceCP(playerid)

//@summary Plugin callback for {@link OnPlayerEnterVehicle}
native B_OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)

//@summary Plugin callback for {@link OnPlayerKeyStateChange}
//@remarks parameters {@code oldkeys} and {@code newkeys} are swapped
native B_OnPlayerKeyStateChange(playerid, oldkeys, newkeys)

//@summary Plugin callback for {@link OnPlayerRequestClass}
native B_OnPlayerRequestClass(playerid, classid)

//@summary Plugin callback for {@link OnPlayerRequestSpawn}
native B_OnPlayerRequestSpawn(playerid)

//@summary Plugin callback for {@link OnPlayerSpawn}
native B_OnPlayerSpawn(playerid)

//@summary Plugin callback for {@link OnPlayerStateChange}
native B_OnPlayerStateChange(playerid, oldstate, newstate)

//@summary Plugin callback for {@link OnPlayerUpdate}
native B_OnPlayerUpdate(playerid)

//@summary Plugin callback for {@link OnPlayerText}
native B_OnPlayerText(playerid, text[])

//@summary Plugin callback for {@link SSocket_OnRecv}
native B_OnRecv(ssocket:handle, data[], len)

//@summary Plugin callback for {@link OnVehicleSpawn}
//@remarks {@paramref vehicleid} is passed-by-reference because the vehicle \
           might be recreated, in which case the id will be changed
native B_OnVehicleSpawn(&vehicleid)

//@summary Plugin callback for {@link OnVehicleStreamIn}
native B_OnVehicleStreamIn(vehicleid, forplayerid)

//@summary Plugin callback for {@link OnVehicleStreamOut}
native B_OnVehicleStreamOut(vehicleid, forplayerid)

//@summary Validate the script and plugin
//@param maxplayers pass {@code MAX_PLAYERS}, to check if the plugin has the same value for it
//@param buf4096 the 4096 sized buffer that's on the heap
//@param buf144 the 144 sized buffer that's on the heap
//@param buf64 the 64 sized buffer that's on the heap
//@param buf32 the 32 sized buffer that's on the heap
//@param buf32_1 the 2nd 32 sized buffer that's on the heap
//@param emptystring empty buffer on the heap
//@param underscorestring buffer containing simply {@code "_"} on the heap
//@returns {@code 0} if something is off
//@remarks plugin also checks if it can find the {@link getrandom} public function
native B_Validate(maxplayers, buf4096[], buf144[], buf64[], buf32[], buf32_1[], emptystring[], underscorestring[])

#namespace "commands.c"

//@summary Scan the next part of {@param cmdtext} for an int
//@param cmdtext command text
//@param idx start location of the param in {@param cmdtext}, will be increased to next param's position if successful
//@param value reference to store value in
//@returns {@code 0} if there's no next int
//@remarks the value in {@param value} will be modified, even if this returns {@code 0}
//@remarks {@param idx} will not be increased if this returns {@code 0}
native Command_GetIntParam(cmdtext[], &idx, &value)

//@summary Scan the next part of {@param cmdtext} for a player reference
//@param cmdtext command text
//@param idx start location of the param in {@param cmdtext}, will be increased to next param's position if successful
//@param player variable to store target player id in (will be {@code INVALID_PLAYER_ID} if player not online)
//@returns {@code 0} if there's no next player param (whitespace followed by active id or part of player name)
//@remarks the value in {@param player} will be modified, even if this returns {@code 0}
//@remarks {@param idx} will not be increased if this returns {@code 0}
native Command_GetPlayerParam(cmdtext[], &idx, &player)

//@summary Scan the next part of {@param cmdtext} for a string
//@param cmdtext command text
//@param idx start location of the param in {@param cmdtext}, will be increased to next param's position if successful
//@param buf buffer to store string in
//@returns {@code 0} if there's no next string
//@remarks {@param idx} will not be increased if this returns {@code 0}
native Command_GetStringParam(cmdtext[], &idx, buf[])

//@summary Checks if the entered cmd is {@param cmd}
//@param cmdtext cmdtext entered by player
//@param cmd the command that should match (all lowercase, leading slash)
//@param idx will contain the index where the params start in {@param cmdtext}
//@remarks don't exceed cmd length of 49
//@remarks {@param idx} may be changed even if this returns {@code 0}
//@remarks commands can't have spaces in them for obvious reasons
native Command_Is(cmdtext[], const cmd[], &idx)

#namespace "game_sa.c"

//@summary Checks if a vehicle with specified model is an air vehicle
//@param model the model of the vehicle to check, {@b MUST} be either a valid model or {@code 0}
//@returns {@code 0} if the model given is not an air vehicle
native Game_IsAirVehicle(model)

//@summary Checks if a vehicle with specified model is a heli
//@param model the model of the vehicle to check, {@b MUST} be either a valid model or {@code 0}
//@returns {@code 0} if the model given is not a heli
native Game_IsHelicopter(model)

//@summary Checks if a vehicle with specified model is a plane
//@param model the model of the vehicle to check, {@b MUST} be either a valid model or {@code 0}
//@returns {@code 0} if the model given is not a plane
native Game_IsPlane(model)

