
// vim: set filetype=c ts=8 noexpandtab:

#namespace "basdon.c"

//@summary Plugin callback for {@link B_OnDialogResponse}
//@returns {@code 0} if further processing should be cancelled
native B_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])

//@summary Plugin callback for {@link OnGameModeExit}
//@remarks call before closing db
native B_OnGameModeExit()

//@summary Plugin callback for {@link OnGameModeInit}
//@remarks call after db has been initialized
native B_OnGameModeInit()

//@summary Plugin callback for mysql callback
native B_OnMysqlResponse(function, data)

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

#namespace "login.c"

//@summary Format query to add failed login
//@param playerid player id
//@param bufq1 buffer for query to insert failed login
//@param bufq2 buffer for query to update lastfal column
//@returns {@code 0} on failure
//@remarks user id and ip must be set to succeed!
native Login_FormatAddFailedLogin(playerid, bufq1[], bufq2[])

//@summary Format query to change password
//@param userid user id
//@param password hashed password to set
//@param buf the buffer to store the query in
native Login_FormatChangePassword(userid, password[], buf[])

//@summary Formats text to show in change password box {@b during gameplay}
//@param buf buffer to store result text in
//@param pwmismatch whether to show the password mismatch error msg (optional={@code 0})
//@param step current step ({@code 0}=current pw, {@code 1}=password, {@code 2}=confirm password)
native Login_FormatChangePasswordBox(buf[], pwmismatch=0, step)

//@summary Formats query to check if user exists
//@param playerid player
//@param buf buffer to store query in
//@returns {@code 0} if something failed and query could not be made
native Login_FormatCheckUserExist(playerid, buf[]);

//@summary Formats query to create a game session
//@param playerid player
//@param buf buffer to store query in
//@returns {@code 0} if something failed and query could not be made
native Login_FormatCreateSession(playerid, buf[])

//@summary Formats query to create a user
//@param playerid player
//@param buf buffer to store query in
//@param password password for user (hashed or empty for guest account)
//@param group group for the new user
//@returns {@code 0} if something failed (user has no data)
native Login_FormatCreateUser(playerid, buf[], password[], group)

//@summary Formats query to create session, and updates user last seen time
//@param playerid player
//@param buf buffer to store queries in
//@remarks buffer will have two querys: on {@code buf[1]} and {@code buf[buf[0]]}
native Login_FormatCreateUserSession(playerid, buf[])

//@summary Formats text to show in register dialog box {@b in a guest session}
//@param playerid player to show for (will show current name)
//@param buf buffer to store result text in
//@param invalid_name_error whether to show the invalid name error msg (optional={@code 0})
//@param pwmismatch whether to show the password mismatch error msg (optional={@code 0})
//@param step current step ({@code 0}=choose name, {@code 1}=password, {@code 2}=confirm password)
//@seealso Login_FormatOnJoinRegisterBox
native Login_FormatGuestRegisterBox(playerid, buf[], invalid_name_error=0, pwmismatch=0, step)

//@summary Formats query to load account data
//@param userid user id
//@param buf buffer to store query in
native Login_FormatLoadAccountData(userid, buf[]);

//@summary Formats text to show in register dialog box {@b on join}
//@param buf buffer to store result text in
//@param pwmismatch whether to show the password mismatch error msg (optional={@code 0})
//@param step current step ({@code 0}=password, {@code 1}=confirm password)
//@seealso Login_FormatGuestRegisterBox
native Login_FormatOnJoinRegisterBox(buf[], pwmismatch=0, step)

//@summary Formats query to save a player's name
//@param playerid player
//@param buf buffer to store query in
//@returns {@code 0} on failure
native Login_FormatSavePlayerName(playerid, buf[])

//@summary Formats query to update a user's last in-game failed login notice time
//@param playerid player
//@param lastfal timestamp of the last failed login that the player has now seen in-game
//@param buf144 buffer to store query in
//@returns {@code 0} on failure (no data for playerid, shouldn't happen)
native Login_FormatUpdateFalng(playerid, lastfal, buf144[])

//@summary Formats query to upgrade a guest account to a real account
//@param playerid player
//@param password hashed password
//@param buf buffer to store queries in
//@returns {@code 0} on failure (but should not happen if login is inited for player)
native Login_FormatUpgradeGuestAcc(playerid, password[], buf[])

//@summary Frees the hashed password stored for player
//@param playerid player to free stored password for
//@seealso Login_GetPassword
//@seealso Login_UsePassword
native Login_FreePassword(playerid)

//@summary Retrieves the hashed password
//@param playerid player
//@param buf buffer to put password in
//@seealso Login_UsePassword
//@seealso Login_FreePassword
//@returns {@code 0} if there's no password stored for player
native Login_GetPassword(playerid, buf[]);

//@summary Free memory that was used to confirm a player's password
//@param playerid the player for which the data was saved
//@remarks Has no effect if the data was already reset
//@seealso Login_PasswordConfirmStore
//@seealso Login_PasswordConfirmValidate
native Login_PasswordConfirmFree(playerid)

//@summary Store password hash to confirm it later using {@link Login_PasswordConfirmValidate}
//@param playerid the player for which to store the hash
//@param pwhash the hash to store
//@seealso Login_PasswordConfirmFree
//@seealso Login_PasswordConfirmValidate
native Login_PasswordConfirmStore(playerid, pwhash[])

//@summary Check if {@param pwhash} equals the previously saved hash
//@param playerid the player for which the data was saved
//@param pwhash the hash to check
//@returns {@code 0} if the hash doesn't match the hash previously saved with {@link Login_PasswordConfirmStore}
//@remarks The saved data gets reset after the check, so no need to call {@link Login_PasswordConfirmFree}
//@seealso Login_PasswordConfirmStore
//@seealso Login_PasswordConfirmFree
native Login_PasswordConfirmValidate(playerid, pwhash[])

//@summary Stores the hashed password so it can be checked against later
//@param playerid player
//@param buf buffer containing password
//@seealso Login_FreePassword
//@seealso Login_GetPassword
native Login_UsePassword(playerid, buf[]);

#namespace "playerdata.c"

//@summary Clears login data for a player
//@param playerid player to clear login data for
native PlayerData_Clear(playerid)

//@summary Formats query to update player's basic data
//@param userid user id of the player to update stuff for
//@param score score of the player to store
//@param money money of the player to store
//@param dis total distance flown (odo)
//@param flighttime total seconds spent flying
//@param prefs player's preferences
//@param buf buffer to store query in
native PlayerData_FormatUpdateQuery(userid, score, money, Float:dis, flighttime, prefs, buf[])

//@summary Updates the player's user ID
//@param playerid player
//@param id userid of the player
//@seealso PlayerData_Clear
//@returns {@code 0} if there was no player data saved for {@param playerid}
native PlayerData_SetUserId(playerid, id)

//@summary Update the player's group
//@param playerid player
//@param group new groups mask of the player (see {@code GROUP_*} constants)
//@returns {@code 0} if there was no player data saved for {@param playerid}
native PlayerData_UpdateGroup(playerid, group)

//@summary Updates the stored player's name on the plugin side
//@param playerid the playerid
//@param name new name
//@param namelen length of {@param name}, excluding zero term
//@seealso PlayerData_SetUserId
//@seealso PlayerData_Clear
//@returns {@code 0} if there was no player data saved for {@param playerid}
native PlayerData_UpdateName(playerid, name[], namelen)

#namespace "vehicles.c"

//@summary Adds a vehicle in the db vehicle table
//@param dbid id of the vehicle in db
//@param model model of the vehicle
//@param owneruserid user id of the owning player
//@param x saved x position
//@param y saved y position
//@param z saved z position
//@param r saved z rotation
//@param col1 first color
//@param col2 second color
//@param odo odometer value
//@param ownername name of the owner
//@returns the position of the vehicle in the vehicle table
//@remarks table will be resized and reallocated if it's already full
//@seealso Veh_Destroy
//@seealso Veh_Init
//@seealso Veh_UpdateSlot
native Veh_Add(dbid, model, owneruserid, Float:x, Float:y, Float:z, Float:r, col1, col2, odo, ownername[])

//@summary Collects all vehicles from the table that are owned by a player
//@param userid user id of the player of whom to collect all vehicles
//@param buf buffer to put in the vehicle data
//@returns the amount of vehicles that were placed in {@param buf}
//@remarks vehicles are placed in buf in following order: {@code col2, col1, r, z, y, x, model, dbid}
native Veh_CollectPlayerVehicles(userid, buf[])

//@summary Collects all vehicle owned by player that are spawned
//@param userid user id of the player of whom to collect spawned vehicles
//@param buf buffer to put the vehicle ids in
//@returns the amount of vehicles that were placed in {@param buf}
native Veh_CollectSpawnedVehicles(userid, buf[])

//@summary Destroys the db vehicle table and service points table and frees used memory
//@seealso Veh_Add
//@seealso Veh_Init
native Veh_Destroy();

//@summary Inits the db vehicle table
//@param dbvehiclecount initial size of the table
//@seealso Veh_Add
//@seealso Veh_Destroy
native Veh_Init(dbvehiclecount)

//@summary Clears data when a player disconnects
//@param playerid the player that disconnected
native Veh_OnPlayerDisconnect(playerid)

//@summary Let the plugin know a vehicle was created or destroyed
//@param vehicleid the id of the vehicle in SA-MP
//@param dbid id of the vehicle in the vehicle table (returned by {@link Veh_Add}), or {@code -1} if slot is now free
//@seealso Veh_Add
native Veh_UpdateSlot(vehicleid, dbid)
