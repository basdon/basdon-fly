struct SampNetGame *samp_pNetGame;
struct SampPlayerPool *playerpool;
struct SampVehiclePool *vehiclepool;

static void* samp_pConsole;

static unsigned char vehicle_gear_state[MAX_VEHICLES];
static int vehicle_gear_change_time[MAX_VEHICLES];

static
void samp_core_init()
{
	TRACE;

	samp_pNetGame = *(struct SampNetGame**) 0x81CA4BC;
	samp_pConsole = *(int**) 0x81CA4B8;
	playerpool = samp_pNetGame->playerPool;
	vehiclepool = samp_pNetGame->vehiclePool;
	rakServer = samp_pNetGame->rakServer;
}
