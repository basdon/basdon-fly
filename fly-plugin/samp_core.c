static void samp_core_init();
static void StreamPlayerIn(struct SampPlayer *player, struct SampPlayer *subjectplayer);
static void StreamPlayerOut(struct SampPlayer *player, ushort subjectplayeridx);

#ifdef SAMP_CORE_IMPL
/*jeanine:p:i:4;p:0;a:b;y:1.88;n:StreamPlayerIn;*/
static
void StreamPlayerIn(struct SampPlayer *player, struct SampPlayer *subject)
{
	TRACE;
#pragma pack(push,1)
	union {
		struct RPCDATA_WorldPlayerAdd03DL rpcdata03DL;
		struct RPCDATA_WorldPlayerAdd037 rpcdata037;
		struct {
			int _pad0;
			struct RPCDATA_WorldPlayerAdd037 rpcdata037;
		} shifted;
	} data;
	EXPECT_SIZE(data, sizeof(struct RPCDATA_WorldPlayerAdd03DL));
#pragma pack(pop)
	struct RPCDATA_WorldPlayerAdd037 *writeptr;
	struct BitStream bs;

	bs.ptrData = &data;
	bs.numberOfBitsUsed = sizeof(struct RPCDATA_WorldPlayerAdd037);
	writeptr = &data.rpcdata037;
	writeptr->playerid = subject->playerid;
	writeptr->team = subject->spawnInfo.team;
	writeptr->skin = subject->spawnInfo.skin;
	if (is_player_using_client_version_DL[player->playerid]) {
		writeptr = &data.shifted.rpcdata037;
		/* This field is used for "custom skin".*/
		/*Brunoo16's packet list says to keep this 0 if it's not a default skin,*/
		/* but if I make either skin field 0, it always shows a CJ skin..*/
		writeptr->skin = subject->spawnInfo.skin;
		bs.numberOfBitsUsed = sizeof(struct RPCDATA_WorldPlayerAdd03DL);
	}
	writeptr->pos = subject->pos;
	writeptr->facingAngle = subject->facingAngle;
	writeptr->color = subject->color;
	memcpy(&writeptr->weaponSkill, subject->weaponSkill, sizeof(subject->weaponSkill));
	EXPECT_SIZE(writeptr->weaponSkill, sizeof(subject->weaponSkill));
	rakServerVtable->RPC_8C(rakServer, (void*) RPC_WorldPlayerAdd, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 2, rakPlayerID[player->playerid], 0, 0);

	/*SAMP here also sends RPC_SetPlayerAttachedObject for each slot but we currently don't use player attached objects*/

	/*OnPlayerStreamIn happens here*/
}
/*jeanine:p:i:3;p:4;a:b;y:1.88;n:StreamPlayerOut;*/
static
void StreamPlayerOut(struct SampPlayer *player, ushort subjectplayeridx)
{
	TRACE;
	struct RPCDATA_WorldPlayerRemove rpcdata;

	rpcdata.playerid = subjectplayeridx;
	SendRPCToPlayer(player->playerid, RPC_WorldPlayerRemove, &rpcdata, sizeof(rpcdata), 2);

	/*OnPlayerStreamOut happens here*/
}
/*jeanine:p:i:5;p:3;a:b;y:1.88;n:samp_core_init;*/
static
void samp_core_init()
{
	TRACE;

	samp = sampPlugins->GetSamp();
	samp_pConsole = sampPlugins->GetSampConsole();
	playerpool = samp->playerPool;
	vehiclepool = samp->vehiclePool;
	rakServer = samp->rakServer; /*also exposed at sampPlugins->GetRakServer()*/
	rakServerVtable = rakServer->vtable;

	mem_mkjmp(0x80CAF00, crash__this_codepath_should_be_unreachable); /*SampPlayer::StreamInPlayer*/
	mem_mkjmp(0x80CAFC0, crash__this_codepath_should_be_unreachable); /*SampPlayer::StreamOutPlayer*/
	mem_mkjmp(0x80D0EC0, crash__this_codepath_should_be_unreachable); /*SampPlayerPool::StreamInPlayer*/
	mem_mkjmp(0x80D0F00, crash__this_codepath_should_be_unreachable); /*SampPlayerPool::StreamOutPlayer*/
}

#endif /*SAMP_CORE_IMPL*/
