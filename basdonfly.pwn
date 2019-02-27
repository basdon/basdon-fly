
// vim: set filetype=c ts=8 noexpandtab:

//@summary Validate the script and plugin {@code MAX_PLAYERS} has the same value
//@param maxplayers should be {@code MAX_PLAYERS}
//@returns {@code 0} if the values don't match
native ValidateMaxPlayers(maxplayers)

//@summary Url encodes given string.
//@param data string to encode
//@param len amount of characters in {@param data}
//@param output buffer to store result in (should be {@code len * 3} of size)
//@returns Amount of charactes written (excluding terminating zero byte).
native Urlencode(const data[], len, output[])

//@summary Reset panel caches for player
//@param playerid the player to reset panel caches for
native Panel_ResetCaches(playerid)

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

//@summary Formats text for heading part of panel
//@param playerid the player to update the heading for
//@param heading the heading as int
//@param buf4 buffer for the speed text, must be of length {@code 4}
//@param buf44 buffer for the speed large meter text, must be of length {@code 44}
//@returns {@code 1} if an update is needed
native Panel_FormatHeading(playerid, heading, buf4[], buf44[])

//@summary Checks if a vehicle with specified model is an air vehicle
//@param model the model of the vehicle to check, {@b MUST} be either a valid model or {@code 0}
//@returns {@code 0} if the model given is not an air vehicle
native IsAirVehicle(model)

//@summary Checks if a vehicle with specified model is a heli
//@param model the model of the vehicle to check, {@b MUST} be either a valid model or {@code 0}
//@returns {@code 0} if the model given is not a heli
native IsHelicopter(model)

//@summary Checks if a vehicle with specified model is a plane
//@param model the model of the vehicle to check, {@b MUST} be either a valid model or {@code 0}
//@returns {@code 0} if the model given is not a plane
native IsPlane(model)

//@summary Store password hash to confirm it later using {@link ValidatePasswordConfirmData}
//@param playerid the player for which to store the hash
//@param pwhash the hash to store
//@remarks This will cause a memory leak if {@link ResetPasswordConfirmData} or {@link ValidatePasswordConfirmData} is not called before this is called again for the same {@param playerid}.
//@seealso ResetPasswordConfirmData
//@seealso ValidatePasswordConfirmData
native SetPasswordConfirmData(playerid, pwhash[])

//@summary Check if {@param pwhash} equals the previously saved hash
//@param playerid the player for which the data was saved
//@param pwhash the hash to check
//@returns {@code 0} if the hash doesn't match the hash previously saved with {@link SetPasswordConfirmData}
//@remarks The saved data gets reset after the check, so no need to call {@link ResetPasswordConfirmData}
//@seealso SetPasswordConfirmData
//@seealso ResetPasswordConfirmData
native ValidatePasswordConfirmData(playerid, pwhash[])

//@summary Free memory that was used to confirm a player's password
//@param playerid the player for which the data was saved
//@remarks Has no effect if the data was already reset
//@seealso SetPasswordConfirmData
//@seealso ValidatePasswordConfirmData
native ResetPasswordConfirmData(playerid)

//@summary Gets hash of command in entered command text (excluding params)
//@param cmdtext command text to get hash of
//@remarks End delimiter for calculation is either a zero terminator, or anything below {@code ' '}
native CommandHash(cmdtext[])

//@summary Checks if the entered cmd is {@param cmd}
//@param cmdtext cmdtext entered by player
//@param cmd the command that should match (all lowercase, leading slash)
//@param idx will contain the index where the params start in {@param cmdtext}
//@remarks don't exceed cmd length of 49
//@remarks {@param idx} may be changed even if this returns {@code 0}
//@remarks commands can't have spaces in them for obvious reasons
native IsCommand(cmdtext[], const cmd[], &idx)

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
native QueueDialog(playerid, dialogid, style, caption[], info[], button1[], button2[], transactionid)

//@summary Drops the dialog queue for a player
//@param playerid the playerid to drop the queue for
//@seealso HasDialogsInQueue
//@seealso QueueDialog
//@seealso PopDialogQueue
native DropDialogQueue(playerid)

//@summary Check if there are dialogs queued for a player
//@param playerid the playerid to check the queue for
//@returns {@code 0} if the queue is empty
//@seealso DropDialogQueue
//@seealso QueueDialog
//@seealso PopDialogQueue
native HasDialogsInQueue(playerid)

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
native PopDialogQueue(playerid, &dialogid, &style, caption[], info[], button1[], button2[], &transactionid)

//@summary Stores a player's data
//@param playerid the playerid
//@param ip player's ip
//@param name player's name
//@param namelen length of {@param name}, excluding zero term
//@seealso PlayerData_UpdateName
//@seealso PlayerData_SetUserId
//@seealso PlayerData_Clear
native PlayerData_Init(playerid, ip[], name[], namelen)

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

//@summary Clears login data for a player
//@param playerid player to clear login data for
//@seealso PlayerData_Init
native PlayerData_Clear(playerid)

//@summary Format data to send to register api
//@param playerid the player for who the call is
//@param inputtext password as entered by player
//@param buf the buffer to store the resulting post data in (use {@code buf4096})
//@returns {@code 0} on failure (no login data set for {@param playerid})
native FormatLoginApiRegister(playerid, inputtext[], buf[])

//@summary Format data to send to guest register api
//@param playerid the player for who the call is
//@param userid userid of the player
//@param inputtext password as entered by player
//@param buf the buffer to store the resulting post data in (use {@code buf4096})
//@returns {@code 0} on failure (no login data set for {@param playerid})
native FormatLoginApiGuestRegister(playerid, userid, inputtext[], buf[])

//@summary Format data to send to check/change passowrd api (they expect the same data)
//@param userid userid of the player
//@param inputtext password as entered by player
//@param buf the buffer to store the resulting post data in (use {@code buf4096})
//@returns {@code 0} on failure (no login data set for {@param playerid})
native FormatLoginApiCheckChangePass(userid, inputtext[], buf[])

//@summary Format data to send to user exists/guest api (they expect the same data)
//@param playerid playerid of the player
//@param buf the buffer to store the resulting post data in (use {@code buf4096})
//@returns {@code 0} on failure (no login data set for {@param playerid})
native FormatLoginApiUserExistsGuest(playerid, buf[])

//@summary Formats query to check if user exists
//@param playerid player
//@param buf buffer to store query in
//@returns {@code 0} if something failed and query could not be made
native Login_FormatCheckUserExist(playerid, buf[]);

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

//@summary Formats query to upgrade a guest account to a real account
//@param playerid player
//@param password hashed password
//@param buf buffer to store queries in
//@returns {@code 0} on failure (but should not happen if login is inited for player)
native Login_FormatUpgradeGuestAcc(playerid, password[], buf[])

//@summary Stores the hashed password so it can be checked against later
//@param playerid player
//@param buf buffer containing password
native Login_UsePassword(playerid, buf[]);

//@summary Retrieves the hashed password
//@param playerid player
//@param buf buffer to put password in
native Login_GetPassword(playerid, buf[]);

//@summary Inits the airport table
//@param amount amount of airports that will be created using {@link APT_Add}
//@seealso APT_Add
//@seealso APT_Destroy
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
//@seealso APT_Init
native APT_Destroy()

//@summary Format airport list in tablist form, sorted by airport distance to given ({@param x},{@param y}) coordinates
//@param playerid player that wants a list (used to store order for follow-up)
//@param x x position of player
//@param y y position of player
//@param buf the buffer to store the resulting list in (use {@code buf4096})
//@remarks Despite being named 'nearest', it adds all airports, but sorted on distance.
//@remarks Shown list order gets saved for follow-up, use {@link APT_ClearFlowCache} to free memory.
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

//@summary Check if the player's zone has changed.
//@param playerid player to check
//@param x x position of player
//@param y y position of player
//@param z z position of player
//@returns {@code 0} if the player's zone did not change
native Zones_UpdateForPlayer(playerid, Float:x, Float:y, Float:z)

//@summary Invalidate cached zone for player
//@param playerid player to invalidate cache for
native Zones_InvalidateForPlayer(playerid)

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

//@summary Scan the next part of {@param cmdtext} for a player reference
//@param cmdtext command text
//@param idx start location of the param in {@param cmdtext}, will be increased to next param's position if successful
//@param player variable to store target player id in (will be {@code INVALID_PLAYER_ID} if player not online)
//@returns {@code 0} if there's no next player param (whitespace followed by active id or part of player name)
//@remarks the value in {@param player} will be modified, even if this returns {@code 0}
//@remarks {@param idx} will not be increased if this returns {@code 0}
native Params_GetPlayer(cmdtext[], &idx, &player)

//@summary Scan the next part of {@param cmdtext} for a string
//@param cmdtext command text
//@param idx start location of the param in {@param cmdtext}, will be increased to next param's position if successful
//@param buf buffer to store string in
//@returns {@code 0} if there's no next string
//@remarks {@param idx} will not be increased if this returns {@code 0}
native Params_GetString(cmdtext[], &idx, buf[])

//@summary Resets all nav for a vehicle
//@param vehicleid vehicle to reset nav for
//@returns {@code 0} if nav was not enabled for vehicle
native Nav_Reset(vehicleid)

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

//@summary get the active nav in the specified vehicle
//@param vehicleid the vehicle to query its nav system
//@returns one of {@code NAV_NONE}, {@code NAV_ADF}, {@code NAV_VOR}, {@code NAV_ILS}
//@remarks {@code NAV_ILS &amp; NAV_VOR == NAV_VOR}
native Nav_GetActiveNavType(vehicleid)

//@summary Reset panel value indicators cache for a player
//@param playerid player to reset cache for
native Nav_ResetCache(playerid)

//@summary Gets the next weather and the message to broadcast and query string to execute for weather stats
//@param nextweatherindex index of next weather, MUST be from {@code 0} to {@code NEXT_WEATHER_POSSIBILITIES} (exclusive)
//@param bufmsg buffer to store message in (should be {@code buf144})
//@param bufquery buffer to store query in to save weather statistics (should be {@code buf4096})
//@returns the weather ID to set
//@remarks {@code bufquery[0]} will be {@code 0} if no query should be executed
//@remarks Also sets the current weather in the plugin for use in {@link Timecyc_GetCurrentWeatherMsg}
native Timecyc_GetNextWeatherMsgQuery(nextweatherindex, bufmsg[], bufquery[])

//@summary Gets the current weather message
//@param buf buffer to store message in (should be {@code buf144})
//@remarks {@link Timecyc_GetNextWeatherMsg} MUST be called at least once before invoking this function
native Timecyc_GetCurrentWeatherMsg(buf[]);

