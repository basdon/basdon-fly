
// vim: set filetype=c ts=8 noexpandtab:

#namespace "airports.c"

//@summary Inits the airport table
//@param amount amount of airports that will be created using {@link APT_Add}
//@seealso APT_Add
//@seealso APT_Destroy
//@remarks undefined behavior when less than {@param amount} APT_Add calls are made afterwards
native APT_Init(amount)

//@summary Add an airport to the airports table
//@param index the index in the table to add this airport
//@param code airport's code, must not be longer than {@code 4} + zero term
//@param enabled {@code 0} if the airport should not be usable
//@param name airport's name, must not be longer than {@code MAX_AIRPORT_NAME} + zero term
//@param beacon airport's beacon, must not be longer than {@code 4} + zero term
//@param x beacon's x position
//@param y beacon's y position
//@param z beacon's z position
//@seealso APT_Init
//@seealso APT_AddRunway
native APT_Add(index, code[], enabled, name[], beacon[], Float:x, Float:y, Float:z)

//@summary Add a runway that belongs to an airport in the airports table
//@param aptindex the index of the airport in the airport table this runway belongs to
//@param specifier either {@code 'L'}, {@code 'C'}, {@code 'R'} or {@code 0}
//@param heading heading of the runway
//@param x beacon's x position
//@param y beacon's y position
//@param z beacon's z position
//@param nav {@code 1} if the runway has navigation (VOR and ILS) abilities
//@seealso APT_Add
native APT_AddRunway(aptindex, specifier, Float:heading, Float:x, Float:y, Float:z, nav)

//@summary Clear the airport table and free the used memory
//@remarks Also clears mission points (which exists inside the airport table)
//@seealso APT_Init
native APT_Destroy()

//@summary Format airport list in tablist form, sorted by airport distance to given ({@param x},{@param y}) coordinates
//@param playerid player that wants a list (used to store order for follow-up)
//@param x x position of player
//@param y y position of player
//@param buf the buffer to store the resulting list in (use {@code buf4096})
//@remarks Despite being named 'nearest', it adds all airports, but sorted on distance.
//@remarks Shown list order gets saved for follow-up, use {@link APT_MapIndexFromListDialog} to free memory.
native APT_FormatNearestList(playerid, Float:x, Float:y, buf[])

//@summary Format beacon list in msgbox form
//@param buf the buffer to store the resulting list in (use {@code buf4096})
native APT_FormatBeaconList(buf[])

//@summary Maps the clicked listitem (from {@link APT_FormatNearestList} to airport index.
//@param playerid the playerid that clicked
//@param listitem the listitem that the player clicked (optional={@code 0}
//@remarks Call this even when it's not needed, it frees memory.
native APT_MapIndexFromListDialog(playerid, listitem=0)

//@summary Format an airport's info for a msgbox
//@param aptidx index of the airport
//@param buf the buffer to store the resulting list in (use {@code buf4096})
//@returns {@code 0} if {@param aptidx} is invalid
native APT_FormatInfo(aptidx, buf[])

//@summary Format an airport's code and name for use in dialog titles etc
//@param aptidx index of the airport
//@param buf the buffer to store the resulting string in (use {@code buf64})
//@returns {@code 0} if {@param aptidx} is invalid
native APT_FormatCodeAndName(aptidx, buf[])

#namespace "anticheat.c"

//@summary Formats query to insert log into db
//@param playerid player that triggered log
//@param loggedstatus logged in status of the player
//@param message message to put in log (not sqli safe)
//@param buf the buffer where the query will be written into
native Ac_FormatLog(playerid, loggedstatus, const message[], buf[])

#namespace "basdon.c"

//@summary Validate the script and plugin
//@param maxplayers pass {@code MAX_PLAYERS}, to check if the plugin has the same value for it
//@returns {@code 0} if something is off
//@remarks plugin also checks if it can find the {@link getrandom} public function
native Validate(maxplayers)

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

//@summary Gets hash of command in entered command text (excluding params)
//@param cmdtext command text to get hash of
//@remarks End delimiter for calculation is either a zero terminator, or anything below {@code ' '}
native Command_Hash(cmdtext[])

//@summary Checks if the entered cmd is {@param cmd}
//@param cmdtext cmdtext entered by player
//@param cmd the command that should match (all lowercase, leading slash)
//@param idx will contain the index where the params start in {@param cmdtext}
//@remarks don't exceed cmd length of 49
//@remarks {@param idx} may be changed even if this returns {@code 0}
//@remarks commands can't have spaces in them for obvious reasons
native Command_Is(cmdtext[], const cmd[], &idx)

#namespace "dialog.c"

//@summary Queue a dialog to show later
//@param playerid see {@link ShowPlayerDialog}
//@param dialogid see {@link ShowPlayerDialog}
//@param style see {@link ShowPlayerDialog}
//@param caption see {@link ShowPlayerDialog}
//@param info see {@link ShowPlayerDialog}
//@param button1 see {@link ShowPlayerDialog}
//@param button2 see {@link ShowPlayerDialog}
//@param transactionid the transactionid for the dialog
//@returns info see {@link ShowPlayerDialog}
//@remarks info see {@link ShowPlayerDialog}
//@seealso HasDialogsInQueue
//@seealso DropDialogQueue
//@seealso PopDialogQueue
native Dialog_Queue(playerid, dialogid, style, caption[], info[], button1[], button2[], transactionid)

//@summary Drops the dialog queue for a player
//@param playerid the playerid to drop the queue for
//@seealso HasDialogsInQueue
//@seealso QueueDialog
//@seealso PopDialogQueue
native Dialog_DropQueue(playerid)

//@summary Check if there are dialogs queued for a player
//@param playerid the playerid to check the queue for
//@returns {@code 0} if the queue is empty
//@seealso DropDialogQueue
//@seealso QueueDialog
//@seealso PopDialogQueue
native Dialog_HasInQueue(playerid)

//@summary Pop the first queued dialog for a player
//@param playerid the playerid to pop for
//@param dialogid variable to store dialogid in, passed by ref
//@param style variable to store style in, passed by ref
//@param caption variable to store caption in, MUST be of at least {@code 64} len
//@param info variable to store info in, MUST be of at least {@code 4096} len
//@param button1 variable to store button1 in, MUST be of at least {@code 32} len
//@param button2 variable to store button2 in, MUST be of at least {@code 32} len
//@param transactionid variable to store transactionid in, passed by ref
//@returns {@code 0} if there was no dialog to pop
//@seealso DropDialogQueue
//@seealso QueueDialog
//@seealso HasDialogsInQueue
native Dialog_PopQueue(playerid, &dialogid, &style, caption[], info[], button1[], button2[], &transactionid)

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
//@param buf the buffer to store the query in
//@returns {@code 0} on failure
//@remarks user id and ip must be set to succeed!
native Login_FormatAddFailedLogin(playerid, buf[])

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

#namespace "missions.c"

//@summary Adds a mission point
//@param aptindex index of the airport in the airports table this mission point belongs to
//@param id the id of the mission point
//@param x x coordinate
//@param y y coordinate
//@param z z coordinate
//@param type type of missions this mission point is for
native Missions_AddPoint(aptindex, id, Float:x, Float:y, Float:z, type)

//@summary Creates a random mission for a player
//@param playerid player to create a random mission for
//@param x current x coordinate of player
//@param y current y coordinate of player
//@param z current z coordinate of player
//@param vehicleid id of the vehicle player is in
//@param vv vehicle reincarnation value (see {@link vv})
//@param vehiclehp the safe health of {@param vehicleid}
//@param msg buffer used to put error msg in (only to be sent when returned {@code 0})
//@param querybuf buffer to store queries in (only to be executed when returned {@code 1}), see remarks
//@returns {@code 0} if a mission could not be started, in that case warn msg {@param msg} should be sent to player and no queries executed
//@remarks queries in {@param querybuf} are at positions {@code 0}, {@code 200}, {@code 400}
//@remarks next step is to execute the queries and call {@link Missions_Start} after query@{@code buf[400]} returned last inserted id
native Missions_Create(playerid, Float:x, Float:y, Float:z, vehicleid, vv, Float:vehiclehp, msg[], querybuf[])

//@summary Ends the active mission for given player (when applicable)
//@param playerid the playerid to cancel their mission for
//@param reason the reason, see {@code MISSION_CANCELREASON_} constants
//@param buf buffer for flg update query, only to be executed if this functions returns non-zero
//@returns {@code 0} if no mission was active for player
native Missions_EndUnfinished(playerid, reason, buf[])

//@summary Try to advance any active mission for player when they entered a race checkpoint
//@param playerid player that entered a checkpoint
//@param vehicleid vehicle id the player is in (may be {@code 0})
//@param vv vehicle reincarnation value (see {@link vv})
//@param x x velocity of the vehicle
//@param y y velocity of the vehicle
//@param z z velocity of the vehicle
//@param errmsg buffer to store errormessage in to be sent if returnvalue is {@code MISSION_ENTERCHECKPOINTRES_ERR}
//@returns {@code 0} if nothing should happen or one of <ul>\
	<li>{@code MISSION_ENTERCHECKPOINTRES_LOAD} if player is now loading cargo</li>\
	<li>{@code MISSION_ENTERCHECKPOINTRES_UNLOAD} if player is now unloading cargo</li>\
	<li>{@code MISSION_ENTERCHECKPOINTRES_ERR} if vehicle does not match for player's mission, send {@param errmsg} to player</li></ul>
//@remarks also checks if {@param vehicleid} is valid
native Missions_EnterCheckpoint(playerid, vehicleid, vv, Float:x, Float:y, Float:z, errmsg[])

//@summary Compute some mission point stuff, to be done after all points have been added
native Missions_FinalizeAddPoints()

//@summary Get mission state of a player
//@param player to get the mission state of
//@returns {@code -1} if not in a mission, or one of the flight-statuses.txt constants
native Missions_GetState(playerid)

//@summary Let the mission logic know a vehicle was repaired, to know the total damage inflicted during flight
//@param playerid the player that repaired the vehicle, this might not be the driver but the mission script can take it
//@param vehicleid the vehicle that was repaired
//@param oldhp old hp of the vehicle
//@param newhp new hp of the vehicle
native Missions_OnVehicleRepaired(playerid, vehicleid, Float:oldhp, Float:newhp)

//@summary Let mission logic know the weather has been changed, to give dangerous weather bonuses when appropriate
//@param newweatherid the new weather
native Missions_OnWeatherChanged(newweatherid)

//@summary Advances mission stage after load, gets coordinates for unload point and update queries
//@param playerid the player that has finished loading
//@param x will contain x coordinate of unloading point on return
//@param y will contain y coordinate of unloading point on return
//@param z will contain z coordinate of unloading point on return
//@param buf will contain update query to execute
//@returns {@code 0} if something is wrong and nothing should happen
native Missions_PostLoad(playerid, &Float:x, &Float:y, &Float:z, buf[])

//@summary Starts a mission for a player after the mission id has been retrieved
//@param playerid the player to start the mission for
//@param missionid the mission id
//@param x will contain loading point x coordinate on return
//@param y will contain loading point y coordinate on return
//@param z will contain loading point z coordinate on return
//@param msg buffer to store mission start msg in, to be sent to the player
//@returns {@code 0} if there's no mission associated with the given player
native Missions_Start(playerid, missionid, &Float:x, &Float:y, &Float:z, msg[])

#namespace "nav.c"

//@summary Enables ADF navigation for a vehicle
//@param vehicleid vehicle to enable ADF for
//@param cmdtext beacon to navigate to (may have whitespace in front)
/// <returns>
///   <ul>
///     <li><b><c>RESULT_ADF_OFF</c></b> - ADF is now disabled</li>
///     <li><b><c>RESULT_ADF_ON</c></b> - ADF is now enabled</li>
///     <li><b><c>RESULT_ADF_ERR</c></b> - error, message stored in <paramref name="buf64"/></li>
///   </ul>
/// </returns>
//@seealso Nav_EnableVOR
//@seealso Nav_ToggleILS
native Nav_EnableADF(vehicleid, cmdtext[], buf64[])

//@summary Enables VOR navigation for a vehicle
//@param vehicleid vehicle to enable VOR for
//@param cmdtext beacon+runway to navigate to (optionally prepended by whitespace and white space between beacon and runway)
//@param buf64 buffer to store custom error message in
/// <returns>
///   <ul>
///     <li><b><c>RESULT_VOR_OFF</c></b> - VOR is now disabled</li>
///     <li><b><c>RESULT_VOR_ON</c></b> - VOR is now enabled</li>
///     <li><b><c>RESULT_VOR_ERR</c></b> - error, message stored in <paramref name="buf64"/></li>
///   </ul>
/// </returns>
//@seealso Nav_EnableADF
//@seealso Nav_ToggleILS
native Nav_EnableVOR(vehicleid, cmdtext[], buf64[])

//@summary get the active nav in the specified vehicle
//@param vehicleid the vehicle to query its nav system
//@returns one of {@code NAV_NONE}, {@code NAV_ADF}, {@code NAV_VOR}, {@code NAV_ILS}
//@remarks {@code NAV_ILS &amp; NAV_VOR == NAV_VOR}
native Nav_GetActiveNavType(vehicleid)

//@summary Update nav data for player
//@param playerid player to update nav for
//@param vehicleid vehicle of player
//@param bufdist buffer for distance
//@param bufalt buffer for altitude
//@param bufcrs buffer for course
//@param bufils buffer for ILS text
//@param vorvalue variable to store vor offset in, will be {@code [320.0f-85.0f,320.0f+85.0f]} or {@code > 640.0f} if not active.
//@returns {@code 0} if nothing needs to be updated, otherwise the first byte of each component will be {@code 0} if no update needed
native Nav_Format(playerid, vehicleid, bufdist[], bufalt[], bufcrs[], bufils[], &Float:vorvalue)

//@summary Resets all nav for a vehicle
//@param vehicleid vehicle to reset nav for
//@returns {@code 0} if nav was not enabled for vehicle
native Nav_Reset(vehicleid)

//@summary Reset panel value indicators cache for a player
//@param playerid player to reset cache for
native Nav_ResetCache(playerid)

//@summary Toggles ILS navigation for a vehicle
//@param vehicleid vehicle to enable ILS for
/// <returns>
///   <ul>
///     <li><b><c>RESULT_ILS_OFF</c></b> - ILS is now disabled</li>
///     <li><b><c>RESULT_ILS_ON</c></b> - ILS is now enabled</li>
///     <li><b><c>RESULT_ILS_NOVOR</c></b> - VOR is not enabled</li>
///     <li><b><c>RESULT_ILS_NOILS</c></b> - no ILS for current VOR runway</li>
///   </ul>
/// </returns>
//@remarks ILS can only be enabled if VOR is enabled on a runway that has VOR+ILS capabilities
//@seealso Nav_EnableADF
//@seealso Nav_EnableVOR
native Nav_ToggleILS(vehicleid)

//@summary Update nav for vehicle
//@param vehicleid vehicle to update nav for
//@param x x position of aircraft
//@param y y position of aircraft
//@param z z position of aircraft
//@param heading heading of aircraft
//@returns {@code 0} if no nav is set for this vehicle
native Nav_Update(vehicleid, Float:x, Float:y, Float:z, Float:heading)

#namespace "panel.c"

//@summary Formats text for altitude part of panel
//@param playerid the player to update the altitude for
//@param altitude the altitude as int
//@param buf4 buffer for the altitude text, must be of length {@code 4}
//@param buf13 buffer for the altitude small meter text, must be of length {@code 13}
//@param buf44 buffer for the altitude large meter text, must be of length {@code 44}
//@returns {@code 1} if an update is needed
//@remarks {@code buf13[0]} will be set to {@code 0} if the small meter doesn't need to be updated
//@remarks {@code buf44[0]} will be set to {@code 0} if the large meter doesn't need to be updated
native Panel_FormatAltitude(playerid, altitude, buf4[], buf13[], buf44[])

//@summary Formats text for heading part of panel
//@param playerid the player to update the heading for
//@param heading the heading as int
//@param buf4 buffer for the speed text, must be of length {@code 4}
//@param buf44 buffer for the speed large meter text, must be of length {@code 44}
//@returns {@code 1} if an update is needed
native Panel_FormatHeading(playerid, heading, buf4[], buf44[])

//@summary Formats text for speed part of panel
//@param playerid the player to update the speed for
//@param speed the speed as int
//@param buf4 buffer for the speed text, must be of length {@code 4}
//@param buf13 buffer for the speed small meter text, must be of length {@code 13}
//@param buf44 buffer for the speed large meter text, must be of length {@code 44}
//@returns {@code 1} if an update is needed
//@remarks {@code buf13[0]} will be set to {@code 0} if the small meter doesn't need to be updated
//@remarks {@code buf44[0]} will be set to {@code 0} if the large meter doesn't need to be updated
//@remarks If the small meter doesn't need to be updated, the large meter won't need an update either
native Panel_FormatSpeed(playerid, speed, buf4[], buf13[], buf44[])

//@summary Reset panel caches for player
//@param playerid the player to reset panel caches for
native Panel_ResetCaches(playerid)

#namespace "playerdata.c"

//@summary Clears login data for a player
//@param playerid player to clear login data for
//@seealso PlayerData_Init
native PlayerData_Clear(playerid)

//@summary Stores a player's data
//@param playerid the playerid
//@param ip player's ip
//@param name player's name
//@param namelen length of {@param name}, excluding zero term
//@seealso PlayerData_UpdateName
//@seealso PlayerData_SetUserId
//@seealso PlayerData_Clear
native PlayerData_Init(playerid, ip[], name[], namelen)

//@summary Formats query to update player's basic data
//@param userid user id of the player to update stuff for
//@param score score of the player to store
//@param money money of the player to store
//@param buf buffer to store query in
native PlayerData_FormatUpdateQuery(userid, score, money, buf[])

//@summary Updates the player's user ID
//@param playerid player
//@param id userid of the player
//@seealso PlayerData_Init
//@seealso PlayerData_Clear
//@returns {@code 0} if there was no player data saved for {@param playerid}
native PlayerData_SetUserId(playerid, id)

//@summary Updates the stored player's name on the plugin side
//@param playerid the playerid
//@param name new name
//@param namelen length of {@param name}, excluding zero term
//@seealso PlayerData_Init
//@seealso PlayerData_SetUserId
//@seealso PlayerData_Clear
//@returns {@code 0} if there was no player data saved for {@param playerid}
native PlayerData_UpdateName(playerid, name[], namelen)

#namespace "playertime.c"

//@summary Formats 0-3 queries to update a user's playtimes in the db
//@param userid the userid of the player, may be {@code -1}
//@param sessionid the sessionid of the player's current session, may be {@code -1}
//@param playtimetoadd how much actual (non-afk) play time to add (is ignored when {@param isdisconnect} is {@code 1})
//@param isdisconnect should be {@code 1} when updating the times during player disconnect
//@param buf buffer to store queries in
//@returns {@code 0} if userid is {@code -1}, meaning no queries must be executed!
//@remarks {@code buf[2]} will have a query to update the player's online times
//@remarks {@code buf[0]} is either {@code 0} when {@param sessionid} was {@code -1}, or contains the\
		position of the query to update the session's endtime in {@param buf}
//@remarks {@code buf[1]} is either {@code 0} when {@param sessionid} was {@code -1} or {@code isdisconnect} is {@code 0},\
		or contains the position of the query to update a player's online time (accurate) in {@param buf}
native Playtime_FormatUpdateTimes(userid, sessionid, playtimetoadd, isdisconnect, buf[])

#namespace "timecyc.c"

//@summary Gets the current weather message
//@param buf buffer to store message in (should be {@code buf144})
//@remarks {@link Timecyc_GetNextWeatherMsg} MUST be called at least once before invoking this function
native Timecyc_GetCurrentWeatherMsg(buf[]);

//@summary Gets the next weather and the message to broadcast and query string to execute for weather stats
//@param nextweatherindex index of next weather, MUST be from {@code 0} to {@code NEXT_WEATHER_POSSIBILITIES} (exclusive)
//@param bufmsg buffer to store message in (should be {@code buf144})
//@param bufquery buffer to store query in to save weather statistics (should be {@code buf4096})
//@returns the weather ID to set
//@remarks {@code bufquery[0]} will be {@code 0} if no query should be executed
//@remarks Also sets the current weather in the plugin for use in {@link Timecyc_GetCurrentWeatherMsg}
native Timecyc_GetNextWeatherMsgQuery(nextweatherindex, bufmsg[], bufquery[])

#namespace "various.c"

//@summary Url encodes given string.
//@param data string to encode
//@param len amount of characters in {@param data}
//@param output buffer to store result in (should be {@code len * 3} of size)
//@returns Amount of charactes written (excluding terminating zero byte).
native Urlencode(const data[], len, output[])

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
//@param ownername name of the owner
//@returns the position of the vehicle in the vehicle table
//@remarks table will be resized and reallocated if it's already full
//@seealso Veh_Destroy
//@seealso Veh_Init
//@seealso Veh_UpdateSlot
native Veh_Add(dbid, model, owneruserid, Float:x, Float:y, Float:z, Float:r, col1, col2, ownername[])

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
native Veh_CollectSpawnedVehicles(playerid, buf[])

//@summary Destroys the db vehicle table and frees used memory
//@seealso Veh_Add
//@seealso Veh_Init
native Veh_Destroy();

//@summary Inits the db vehicle table
//@param dbvehiclecount initial size of the table
//@seealso Veh_Add
//@seealso Veh_Destroy
native Veh_Init(dbvehiclecount)

//@summary Checks if a player is allowed to be in a vehicle
//@param userid user id of the player to check
//@param vehicleid vehicle
//@param buf[] if not allowed, errormessage will be placed in this buffer
//@returns {@code 0} if that user is not allowed in the given vehicle (to send to player when trying to enter)
native Veh_IsPlayerAllowedInVehicle(userid, vehicleid, buf[])

//@summary Check if there is a label on given vehicle for given player, {@b and unregister it}
//@param vehicleid vehicle on which a label might be
//@param playerid player for which the label would have been created
//@param labelid the label id will be put in this variable if this returns positive
//@returns {@code 1} if there is a label to delete, its id will be put in {@param labelid}
native Veh_GetLabelToDelete(vehicleid, playerid, &PlayerText3D:labelid)

//@summary Clears data when a player disconnects
//@param playerid the player that disconnected
native Veh_OnPlayerDisconnect(playerid)

//@summary Let the plugin know a label was created on a vehicle for a player
//@param vehicleid the vehicle the label is attached to
//@param playerid the player the label was made for
//@param labelid the label id assigned to the newly created label
native Veh_RegisterLabel(vehicleid, playerid, PlayerText3D:labelid)

//@summary Check if a label should be created on a vehicle for a player
//@param vehicleid vehicle on which the label would be attached
//@param playerid player for which the label would show
//@param buf buffer to store the label text in, if this returns positive
//@returns {@code 1} if a label should be made, with given text in {@param buf}
native Veh_ShouldCreateLabel(vehicleid, playerid, buf[])

//@summary Let the plugin know a vehicle was created or destroyed
//@param vehicleid the id of the vehicle in SA-MP
//@param dbid id of the vehicle in the vehicle table (returned by {@link Veh_Add}), or {@code -1} if slot is now free
//@seealso Veh_Add
native Veh_UpdateSlot(vehicleid, dbid)

#namespace "zones.c"

//@summary Insert the zone string for player in {@param buf}
//@param playerid the playerid of which to get the zone
//@param buf a buffer
native Zones_FormatForPlayer(playerid, buf[])

//@summary Inserts the formatted '/loc' text for a player
//@param playerid the playerid whose '/loc' is needed
//@param buf a buffer
//@param z z position of player
//@param model model of vehicle the player is in or {@code 0}
//@param vx x-velocity of vehicle the player is in
//@param vy y-velocity of vehicle the player is in
//@param vz z-velocity of vehicle the player is in
native Zones_FormatLoc(playerid, buf[], Float:z, model, Float:vx, Float:vy, Float:vz)

//@summary Invalidate cached zone for player
//@param playerid player to invalidate cache for
native Zones_InvalidateForPlayer(playerid)

//@summary Check if the player's zone has changed.
//@param playerid player to check
//@param x x position of player
//@param y y position of player
//@param z z position of player
//@returns {@code 0} if the player's zone did not change
native Zones_UpdateForPlayer(playerid, Float:x, Float:y, Float:z)

