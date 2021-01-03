#define PLANE 0x1
#define HELI 0x2
#define RETRACTABLE_GEAR 0x4

/**
Some mapping, every air vehicle has a unique value 0-22, -1 for others
*/
extern char aircraftmodelindex[VEHICLE_MODEL_TOTAL];
extern unsigned int vehicleflags[VEHICLE_MODEL_TOTAL];

/**
Check if the given vehicle model is an air vehicle (plane or helicopter).
*/
int game_is_air_vehicle(int model);
/**
Check if the given vehicle model is a helicopter.
*/
int game_is_heli(int model);
/**
Check if the given vehicle model is a plane.
*/
int game_is_plane(int model);
/**
Gives a random 'normal' random vehicle colors combination for given model.

Data is from game's carcols.dat.

@param col1 ptr to int or NULL if primary color is not needed
@param col2 ptr to int or NULL if secondary color is not needed
*/
void game_random_carcol(int model, int *col1, int *col2);
void game_sa_init();

