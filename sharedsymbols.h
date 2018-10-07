
/* vim: set filetype=c ts=8 noexpandtab: */

/* global */
#define MAX_PLAYERS (50)
#ifdef MAX_PLAYER_NAME
#assert MAX_PLAYER_NAME == 24
#else
#define MAX_PLAYER_NAME (24)
#endif

/* airport.c / airport.pwn */
#define MAX_AIRPORT_NAME (24)
/* dialog.c / dialog.pwn */
#define LIMIT_DIALOG_CAPTION (64)
#define LIMIT_DIALOG_TEXT (4096)
/* button len limit is not defined */
#define LIMIT_DIALOG_BUTTON (32)

/* login.c / login.pwn */
#define PW_HASH_LENGTH (65) /* including zero term */

/* no parens around these numbers or gm may stop compiling */
/* game_sa.c / game_sa.pwn, global */
#define MODEL_LEVIATHAN 417
#define MODEL_HUNTER 425
#define MODEL_SEASPARROW 447
#define MODEL_SKIMMER 460
#define MODEL_RCBARON 464
#define MODEL_RCRAIDER 465
#define MODEL_SPARROW 469
#define MODEL_RUSTLER 476
#define MODEL_MAVERICK 487
#define MODEL_NEWSCHOPPER 488
#define MODEL_POLMAVERICK 497
#define MODEL_RCGOBLIN 501
#define MODEL_BEAGLE 511
#define MODEL_CROPDUSTER 512
#define MODEL_STUNTPLANE 513
#define MODEL_SHAMAL 519
#define MODEL_HYDRA 520
#define MODEL_CARGOBOB 548
#define MODEL_NEVADA 553
#define MODEL_RAINDANCE 563
#define MODEL_AT400 577
#define MODEL_ANDROMADA 592
#define MODEL_DODO 593

#define MODEL_TOTAL (611-400)

