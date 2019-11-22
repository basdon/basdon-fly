
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "objects.h"
#include "timer.h"

/*
radar from base offsets: (base facing W, radar on E pos)
4.7, -0.2, 11.4
*/
#define OBJ_RADAR_LA_X 1383.3f
#define OBJ_RADAR_LA_Y -2422.4f
#define OBJ_RADAR_LA_Z 29.0f
#define OBJ_RADAR_LV_X 1296.7f
#define OBJ_RADAR_LV_Y 1502.5f
#define OBJ_RADAR_LV_Z 26.0f
#define OBJ_RADAR_SF_X -1692.8f
#define OBJ_RADAR_SF_Y -620.9f
#define OBJ_RADAR_SF_Z 29.6f

/**
Object id for rotating radar objects.
*/
static int obj_radar_la, obj_radar_lv, obj_radar_sf;

/**
Timer callback that rotates radars at LV, SF, LA
*/
static
int objs_radar_rotate_timer(void *DATA)
{
	static float obj_radar_z_rot = 0.0f;
	static int z_off_i = 0;

	float zoffset;

	obj_radar_z_rot += 179.99f;
	if (obj_radar_z_rot > 360.0f) {
		obj_radar_z_rot -= 360.0f;
	}
	z_off_i ^= 0x3bc49ba6;
	zoffset = *((float*) &z_off_i);

	NC_PARS(8);
	nc_paramf[5] = 0.0012f;
	nc_paramf[6] = nc_paramf[7] = 0.0f;
	nc_paramf[8] = obj_radar_z_rot;

	nc_params[1] = obj_radar_la;
	nc_paramf[2] = OBJ_RADAR_LA_X;
	nc_paramf[3] = OBJ_RADAR_LA_Y;
	nc_paramf[4] = OBJ_RADAR_LA_Z + zoffset;
	NC(n_MoveObject);

	nc_params[1] = obj_radar_lv;
	nc_paramf[2] = OBJ_RADAR_LV_X;
	nc_paramf[3] = OBJ_RADAR_LV_Y;
	nc_paramf[4] = OBJ_RADAR_LV_Z + zoffset;
	NC(n_MoveObject);

	nc_params[1] = obj_radar_sf;
	nc_paramf[2] = OBJ_RADAR_SF_X;
	nc_paramf[3] = OBJ_RADAR_SF_Y;
	nc_paramf[4] = OBJ_RADAR_SF_Z + zoffset;
	NC(n_MoveObject);
	return 5000;
}

void objs_on_gamemode_init()
{
	timer_set(2000, objs_radar_rotate_timer, NULL);

	NC_PARS(8);
	nc_params[1] = 1682;
	nc_paramf[5] = nc_paramf[6] = nc_paramf[7] = 0.0f;
	nc_paramf[8] = 400.0f;

	nc_paramf[2] = OBJ_RADAR_LA_X;
	nc_paramf[3] = OBJ_RADAR_LA_Y;
	nc_paramf[4] = OBJ_RADAR_LA_Z;
	obj_radar_la = NC(n_CreateObject);

	nc_paramf[2] = OBJ_RADAR_LV_X;
	nc_paramf[3] = OBJ_RADAR_LV_Y;
	nc_paramf[4] = OBJ_RADAR_LV_Z;
	obj_radar_lv = NC(n_CreateObject);

	nc_paramf[2] = OBJ_RADAR_SF_X;
	nc_paramf[3] = OBJ_RADAR_SF_Y;
	nc_paramf[4] = OBJ_RADAR_SF_Z;
	obj_radar_sf = NC(n_CreateObject);

	/*LA radar base*/
	nc_params[1] = 7981;
	nc_paramf[2] = OBJ_RADAR_LA_X + 4.7f;
	nc_paramf[3] = OBJ_RADAR_LA_Y - 0.2f;
	nc_paramf[4] = OBJ_RADAR_LA_Z - 11.4f;
	nc_paramf[7] = 180.0f;
	NC(n_CreateObject);
	printf("%d %d %d\n", obj_radar_la, obj_radar_lv, obj_radar_sf);
}

void objs_on_player_connect(int playerid)
{
	NC_PARS(8);
	nc_params[1] = playerid;

	/*radars*/
	nc_params[2] = 1682;
	/*LA*/
	/*
	1709.4f, -2362.7f, 31.7f
	1663.6f, -2362.7f, 31.7f
	*/
	nc_paramf[3] = 1686.5f;
	nc_paramf[4] = -2362.7f;
	nc_paramf[5] = 31.7f;
	nc_paramf[6] = 23.5f;
	NC(n_RemoveBuildingForPlayer);
	/*LV*/
	nc_paramf[3] = 1295.1f;
	nc_paramf[4] = 1502.9f;
	nc_paramf[5] = 26.2f;
	nc_paramf[6] = 1.2f;
	NC(n_RemoveBuildingForPlayer);
	/*SF*/
	nc_paramf[3] = -1691.6f;
	nc_paramf[4] = -619.7f;
	nc_paramf[5] = 29.6f;
	nc_paramf[6] = 1.2f;
	NC(n_RemoveBuildingForPlayer);
	
	/*runway ramps*/
	nc_params[2] = 3664;
	/*
	2042.8f, -2442.2f, 19.3f // LA ramp random
	1388.0f, -2494.3f, 19.3f // LA ramp runwayN
	1388.0f, -2593.0f, 19.3f // LA ramp runwayS
	*/
	nc_paramf[3] = 1715.4f;
	nc_paramf[4] = -2517.6f;
	nc_paramf[5] = 19.3f;
	nc_paramf[6] = 387.5f;
	NC(n_RemoveBuildingForPlayer);

	nc_paramf[6] = 0.25f;
	/*LA annoying dock crane*/
	nc_params[2] = 1378;
	nc_paramf[3] = 2232.4375f;
	nc_paramf[4] = -2458.5781f;
	nc_paramf[5] = 36.1953f;
	NC(n_RemoveBuildingForPlayer);
	/*LA annoying dock crane LOD*/
	nc_params[2] = 1396;
	nc_paramf[3] = 2232.4375f;
	nc_paramf[4] = -2458.5781f;
	nc_paramf[5] = 36.1953f;
	NC(n_RemoveBuildingForPlayer);
	/*cable or blue control*/
	nc_params[2] = 1377;
	nc_paramf[3] = 2201.6250f;
	nc_paramf[4] = -2458.5781f;
	nc_paramf[5] = 38.9844f;
	NC(n_RemoveBuildingForPlayer);
	/*cable or blue control*/
	nc_params[2] = 1376;
	nc_paramf[3] = 2227.1016f;
	nc_paramf[4] = -2458.5938f;
	nc_paramf[5] = 31.6797f;
	NC(n_RemoveBuildingForPlayer);

	/*annoying stairs at LA*/
	nc_params[2] = 3663;
	/*
	1580.09f, -2433.83f, 14.5703f
	1664.45f, -2439.8f, 14.4688f
	1832.45f, -2388.44f, 14.4688f
	1882.27f, -2395.78f, 14.4688f
	*/
	nc_paramf[3] = 1731.18f;
	nc_paramf[4] = -2417.79f;
	nc_paramf[5] = 14.4688f;
	nc_paramf[6] = 175.0f;
	NC(n_RemoveBuildingForPlayer);

}
