struct Samp *samp;
struct SampPlayerPool *playerpool;
struct SampVehiclePool *vehiclepool;

static void* samp_pConsole;

static unsigned char vehicle_gear_state[MAX_VEHICLES];
static int vehicle_gear_change_time[MAX_VEHICLES];

static
void samp_core_init()
{
	TRACE;

	samp = *(struct Samp**) 0x81CA4BC;
	samp_pConsole = *(int**) 0x81CA4B8;
	playerpool = samp->playerPool;
	vehiclepool = samp->vehiclePool;
	rakServer = samp->rakServer;
}
