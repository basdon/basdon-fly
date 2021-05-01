/*See MAX_MISSIONPOINTS_PER_AIRPORT. 15 is enough.
If an airport has more, they are probably split out enough anyways.*/
#define MAX_MISSION_INDICATORS (15)
#define MAX_SERVICE_MAP_ICONS (4)
/*If this many vehicles are streamed in, there are bigger problems.*/
#define MAX_VEHICLE_TEXTLABELS (400)

/*ACTOR IDS*/

/*Octavia's actor id on Octavia Island*/
#define OCTA_ACTORID 999

/*DIALOG TRANSACTION IDS*/

#define DLG_TID_NONE 0 /*Can be used for zero importance dialogs that will be overridden by any dialog.*/
#define DLG_TID_AIRPORT_NEAREST 1000
#define DLG_TID_AIRPORT_NEAREST_DETAIL 1001
#define DLG_TID_AIRPORT_BEACONS 1100
#define DLG_TID_ADMIN_VEHINFO 2000
#define DLG_TID_ADMIN_VEHINFO_ASSIGNAP 2001
#define DLG_TID_CHANGELOG 3000
#define DLG_TID_CHANGEPASS 4000
#define DLG_TID_CHANGEPASS_ABORTED 4001
#define DLG_TID_CHANGEPASS_WRONG 4002
#define DLG_TID_LOGIN 5000
#define DLG_TID_GUESTREGISTER 6000
#define DLG_TID_MISSION_OVERVIEW 7000
#define DLG_TID_PREFERENCES 8000
#define DLG_TID_PREFERENCES_NAMETAGDISTANCE 8001
#define DLG_TID_SPAWN 9000
#define DLG_TID_CMDLIST 10000

/*MAPICON IDS*/

#define SERVICEPOINT_MAPICON_ID_BASE (0)
STATIC_ASSERT(SERVICEPOINT_MAPICON_ID_BASE + MAX_SERVICE_MAP_ICONS < MAX_MAPICONS);

/*OBJECT IDS*/

/*Max amount of objects the mapsystem may use.*/
/*These are also the ids, so mapsystem uses ids 0-899
(unless the code changed to not use objectid 0).*/
#define MAX_MAPSYSTEM_OBJECTS (900)
#define OBJECT_DEV_PLATFORM (983)
#define OBJECT_MISSION_INDICATOR_BASE (984)
#define OBJECT_ROTATING_RADAR (999)
#if OBJECT_MISSION_INDICATOR_BASE + MAX_MISSION_INDICATORS != OBJECT_ROTATING_RADAR
#error "update object ids"
#endif

/*TEXTDRAW IDS*/

/*Ids 0-2047 are global TDs, ids 2048-2303 are player TDs.*/
/*Drawing order: higher id gets drawn over lower id, but globals are drawn before player[citation needed].*/

/*TODO make some order in this chaos.*/

/*Missionmap: need consecutive ids (that makes it easier to show them using textdraws_show)*/
#define NUM_MISSIONMAP_LOCATIONS_TEXTDRAWS (8)
#define TEXTDRAW_MISSIONMAP_LOCATIONS_BASE 2296
#define TEXTDRAW_MISSIONMAP_ISLAND_IGZU 2296
#define TEXTDRAW_MISSIONMAP_ISLAND_BNSA 2297
#define TEXTDRAW_MISSIONMAP_ISLAND_DUNE 2298
/*Textdraws above this line are compied from the template textdraws in the text file.*/
#define TEXTDRAW_MISSIONMAP_LOCATIONS_BASE_FROMFILE 2299
#define NUM_MISSIONMAP_LOCATIONS_TEXTDRAWS_INFILE (5)
#define TEXTDRAW_MISSIONMAP_ISLAND_CATA 2299
#define TEXTDRAW_MISSIONMAP_ISLAND_OCTA 2300
#define TEXTDRAW_MISSIONMAP_ISLAND_MAINLAND 2301
#define TEXTDRAW_MISSIONMAP_LOCATION_TO 2302
#define TEXTDRAW_MISSIONMAP_LOCATION_FROM 2303

#define TEXTDRAW_MISSIONHELP_BASE (1000)
/*Missionmap: using same base as missionhelp, since they can't show at the same time anyways.*/
#define TEXTDRAW_MISSIONMAP_BASE TEXTDRAW_MISSIONHELP_BASE
#define TEXTDRAW_JOBSATISFACTION (TEXTDRAW_MISSIONHELP_BASE - 1)
#define TEXTDRAW_GPS (TEXTDRAW_JOBSATISFACTION - 1)
#define NUM_KNEEBOARD_TEXTDRAWS (3)
#define TEXTDRAW_KNEEBOARD_BASE (TEXTDRAW_GPS - NUM_KNEEBOARD_TEXTDRAWS)
#define NUM_PANEL_TEXTDRAWS (28)
#define TEXTDRAW_PANEL_BASE (TEXTDRAW_KNEEBOARD_BASE - NUM_PANEL_TEXTDRAWS)
#define TEXTDRAW_VEHSPD (TEXTDRAW_PANEL_BASE - 1)
STATIC_ASSERT(TEXTDRAW_VEHSPD > 0);

/*TEXTLABEL IDS*/

/*0-1023 are global textlabels, 1024-2047 are player textlabels*/

/*Ids 0-MAX_PLAYERS are used for player nametags.*/

#define NAMETAG_TEXTLABEL_ID_BASE (0)
#define DEBUG_RUNWAY_LABEL_ID_BASE (500) /*num is amount of runways*2 basically*/
#define SERVICEPOINT_TEXTLABEL_ID_BASE (MAX_PLAYERS)
#define VEHICLE_TEXTLABEL_ID_BASE (SERVICEPOINT_TEXTLABEL_ID_BASE + MAX_SERVICE_MAP_ICONS)
STATIC_ASSERT(VEHICLE_TEXTLABEL_ID_BASE + MAX_VEHICLE_TEXTLABELS < MAX_3DTEXT_LABELS);
