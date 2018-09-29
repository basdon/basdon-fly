
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

