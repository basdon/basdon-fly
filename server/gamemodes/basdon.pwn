// stack/heap size:
// - compiler report says estimated max is 9 cells
// - that does not include heap space needed for vararg calls
// - might not include heap space needed for pass-by-reference parameters
// - on top of that, samp/plugins might push strings, which need heapspace...
// TODO: lower this again (to 512?) when ssocket is added into the plugin (though does bcrypt/mysql need it?)
#pragma dynamic 1024

#define export%0\32%1(%2) forward %1(%2);public %1(%2)

#define NATIVE_ENTRY ();native
forward __UNUSED
#include "natives"
#undef _inc_natives
()
#undef NATIVE_ENTRY

export dummies()
{
#define NATIVE_ENTRY
#include "natives"
#undef _inc_natives
}

main()
{
}

export OnGameModeInit()
{
#emit SYSREQ.C B_OnGameModeInit
#emit RETN
}

export OnGameModeExit()
{
#emit SYSREQ.C B_OnGameModeExit
#emit RETN
}

export OnIncomingConnection(playerid, ip_address[], port)
{
#emit SYSREQ.C B_OnIncomingConnection
#emit RETN
}

export OnPlayerCommandText(playerid, cmdtext[])
{
#emit SYSREQ.C B_OnPlayerCommandText
#emit RETN
}

export OnPlayerConnect(playerid)
{
#emit SYSREQ.C B_OnPlayerConnect
#emit RETN
}

export OnPlayerDeath(playerid, killerid, reason)
{
#emit SYSREQ.C B_OnPlayerDeath
#emit RETN
}

export OnPlayerDisconnect(playerid, reason)
{
#emit SYSREQ.C B_OnPlayerDisconnect
#emit RETN
}

export OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
#emit SYSREQ.C B_OnPlayerEnterVehicle
#emit RETN
}

export OnPlayerRequestClass(playerid, classid)
{
#emit SYSREQ.C B_OnPlayerRequestClass
#emit RETN
}

export OnPlayerRequestSpawn(playerid)
{
#emit SYSREQ.C B_OnPlayerRequestSpawn
#emit RETN
}

export OnPlayerSpawn(playerid)
{
#emit SYSREQ.C B_OnPlayerSpawn
#emit RETN
}

export OnPlayerStateChange(playerid, newstate, oldstate)
{
#emit SYSREQ.C B_OnPlayerStateChange
#emit RETN
}

export OnPlayerText(playerid, text[])
{
#emit SYSREQ.C B_OnPlayerText
#emit RETN
}

export OnPlayerUpdate(playerid)
{
#emit SYSREQ.C B_OnPlayerUpdate
#emit RETN
}

export OnVehicleSpawn(vehicleid)
{
#emit SYSREQ.C B_OnVehicleSpawn
#emit RETN
}

export OnVehicleStreamIn(vehicleid, forplayerid)
{
#emit SYSREQ.C B_OnVehicleStreamIn
#emit RETN
}

export OnVehicleStreamOut(vehicleid, forplayerid)
{
#emit SYSREQ.C B_OnVehicleStreamOut
#emit RETN
}

export SSocket_OnRecv(ssocket:handle, data[], len)
{
#emit SYSREQ.C B_OnRecv
#emit RETN
}

export OnQueryError(errorid, error[], callback[], query[], connectionHandle)
{
#emit SYSREQ.C B_OnQueryError
#emit RETN
}

export MM(function, data)
{
#emit SYSREQ.C B_OnCallbackHit
#emit RETN
}
