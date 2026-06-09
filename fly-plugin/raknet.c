/**not static because it's used in samp.asm*/
struct RakServer *rakServer;
static struct RakServer_vtable *rakServerVtable;
static struct PlayerID rakPlayerID[MAX_PLAYERS];

static
__attribute__((unused))
int raknet_get_messages_in_send_buffer_for_player(int playerid)
{
	TRACE;
	register struct RakNetStatistics *stats;

	stats = rakServer->vtable->GetStatistics(rakServer, rakPlayerID[playerid]);
	return stats->messageSendBufferPrior0 +
		stats->messageSendBufferPrior1 +
		stats->messageSendBufferPrior2 +
		stats->messageSendBufferPrior3;
}

static
void bitstream_write_zero(struct BitStream *bitstream)
{
	TRACE;
	((void (*)(struct BitStream*))0x804D3E0)(bitstream);
}

static
void bitstream_write_one(struct BitStream *bitstream)
{
	TRACE;
	((void (*)(struct BitStream*))0x804D370)(bitstream);
}

static
void bitstream_write_bits(struct BitStream *bitstream, int value, int num_bits)
{
	TRACE;
	((void (*)(struct BitStream*,void*,int,char))0x804D250)(bitstream, &value, num_bits, 1);
}

static
void bitstream_write_bytes(struct BitStream *bitstream, void *value, int num_bytes)
{
	TRACE;
	((void (*)(struct BitStream*,void*,int))0x804D680)(bitstream, value, num_bytes);
}

static
void bitstream_write_quaternion(struct BitStream *bitstream, float w, float x, float y, float z)
{
	TRACE;
	((void (*)(struct BitStream*,float,float,float,float))0x80CE880)(bitstream, w, x, y, z);
}

static
void bitstream_write_velocity(struct BitStream *bitstream, float x, float y, float z)
{
	TRACE;
	((void (*)(struct BitStream*,float,float,float))0x80CEA50)(bitstream, x, y, z);
}
