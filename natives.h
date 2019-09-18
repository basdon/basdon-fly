
/* vim: set filetype=c ts=8 noexpandtab: */

extern int n_random;
extern cell *nc_params;
extern cell *nc_result;

#define NativeCall_random(MAX) nc_params[0]=1;nc_params[1]=MAX;amx_Callback(amx,n_random,nc_result,nc_params)
