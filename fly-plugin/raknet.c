static struct RakServer *rakServer;
static struct PlayerID rakPlayerID[MAX_PLAYERS];

static
__attribute__((unused))
int raknet_get_messages_in_send_buffer_for_player(int playerid)
{
	register struct RakNetStatistics *stats;

	stats = rakServer->vtable->GetStatistics(rakServer, rakPlayerID[playerid]);
	return stats->messageSendBufferPrior0 +
		stats->messageSendBufferPrior1 +
		stats->messageSendBufferPrior2 +
		stats->messageSendBufferPrior3;
}
