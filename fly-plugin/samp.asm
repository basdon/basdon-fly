extern logprintf
extern samp_pNetGame
extern rakServer;

extern hook_OnOnfootSync
extern hook_OnDriverSync
extern hook_OnPassengerSync
extern hook_cmd_on_cmdtext
extern hook_dialog_on_response
extern printf

;prot void printf_logprintf(char *format, ...);
global printf_logprintf:function
printf_logprintf:
	pop dword [logprintRetAddr]
	call printf
	push aNewline;
	call printf
	pop eax
	mov eax, dword [logprintf]
	call eax
	push dword [logprintRetAddr]
	ret

;prot void OnfootSyncHook();
global OnfootSyncHook:function
OnfootSyncHook:
	mov [esp], eax ; playerid
	call hook_OnOnfootSync
	mov eax, 080AC8D1h
	jmp eax

;prot void DriverSyncHook();
global DriverSyncHook:function
DriverSyncHook:
	mov [esp], eax ; playerid
	call hook_OnDriverSync
	mov eax, 080AEB71h
	jmp eax

;prot void PassengerSyncHook();
global PassengerSyncHook:function
PassengerSyncHook:
	mov [esp], eax ; playerid
	call hook_OnPassengerSync
	mov eax, 080AE9D1h
	jmp eax

;prot void OnPlayerCommandTextHook();
global OnPlayerCommandTextHook
OnPlayerCommandTextHook:
	mov [esp], ebx ; playerid
	mov [esp+04h], edi ; cmdtext
	call hook_cmd_on_cmdtext
	mov eax, 080B15FDh
	jmp eax

;prot void OnDialogResponseHook();
global OnDialogResponseHook:function
OnDialogResponseHook:
	lea eax, [ebp-0258h]
	mov [esp+010h], eax ; inputtext
	movzx eax, word [ebp-013Ch]
	mov [esp+0Ch], eax ; listitem
	movzx eax, byte [ebp-0259h]
	mov [esp+08h], eax ; response
	movzx eax, word [ebp-013Ah]
	mov [esp+04h], eax ; dialogid
	mov [esp], ebx ; playerid
	call hook_dialog_on_response
	mov eax, 080B2A8Dh
	jmp eax

;prot float* samphost_GetPtrStreamDistance();
global samphost_GetPtrStreamDistance:function
samphost_GetPtrStreamDistance:
	push aStreamDistance
	mov eax, 081CA4B8h
	push dword [eax]
	mov eax, 080A00D0h
	call eax
	add esp, 0x8
	ret

;prot /**
;prot Crashes if vehicle does not exist.
;prot */
;prot void GetVehiclePosRotUnsafe(int vehicleid, struct vec4 *pos);
global GetVehiclePosRotUnsafe:function
GetVehiclePosRotUnsafe:
	push ebp
	mov ebp, esp
	push ebx
	sub esp, 024h
	;
	mov eax, [samp_pNetGame]
	mov ebx, [eax+0Ch]
	mov eax, [ebp+08h] ;vehicleid
	mov ebx, [ebx+eax*4+03F54h]
	;
	mov eax, [ebp+0Ch] ;*pos
	mov ecx, [ebx] ;x
	mov [eax], ecx
	mov ecx, [ebx+04h] ;y
	mov [eax+04h], ecx
	mov ecx, [ebx+08h] ;z
	mov [eax+08h], ecx
	;
	add eax, 0Ch ;pos->r
	mov [ebp-0Ch], eax ;phys_addr
	mov eax, 080DD032h ;n_GetVehicleZAngle
	jmp eax

;prot /**
;prot Crashes if vehicle does not exist.
;prot {@param rot} must be ptr to {@code struct { float w, x, y, z }}.
;prot */
;prot void GetVehicleRotationQuatUnsafe(int vehicleid, struct quat *rot);
;test *(int*) 0x80DD14C == 0xFFF6E970 /*Call to MatrixToQuat?*/
global GetVehicleRotationQuatUnsafe:function
GetVehicleRotationQuatUnsafe:
	push ebx
	mov eax, [samp_pNetGame]
	mov ebx, [eax+0Ch]
	mov eax, [esp+08h] ;vehicleid
	mov ebx, [ebx+eax*4+03F54h]
	add ebx, 0Ch
	mov eax, [esp+0Ch] ;*rot
	push eax
	push ebx
	mov eax, 0804BAC0h ;MatrixToQuat?
	call eax
	add esp, 08h
	pop ebx
	ret

;prot /**
;prot Crashes if vehicle does not exist.
;prot */
;prot void GetVehicleZAngleUnsafe(int vehicleid, float *angle);
;test __builtin_offsetof(struct SampNetGame, vehiclePool) == 0xC
global GetVehicleZAngleUnsafe:function
GetVehicleZAngleUnsafe:
	push ebp
	mov ebp, esp
	push ebx
	sub esp, 024h
	;
	mov eax, [ebp+0Ch] ;*angle
	mov [ebp-0Ch], eax ;phys_addr
	mov eax, [samp_pNetGame]
	mov ebx, [eax+0Ch]
	mov eax, [ebp+08h] ;vehicleid
	mov ebx, [ebx+eax*4+03F54h]
	mov eax, 080DD032h ;n_GetVehicleZAngle
	jmp eax

;prot /**
;prot  * Use SetVehicleSiren to set the siren flag (it's included in CreateVehicle for the PAWN API).
;prot  *
;prot  * AddStaticVehicle(Ex) is really not special.
;prot  * The only difference is that AddStaticVehicle(Ex) will add 3x FREIFLAT or STREAKC when spawning a FREIGHT or STREAK,
;prot  * and CreateVehicle will not allow creating FREIGHT or STREAK (but if you force it, it works?).
;prot  *
;prot  * Clients ALWAYS create the 3x FREIFLAT or STREAKC, so when the server doesn't create it, you'll get lots of
;prot  * "Warning: vehicle xx was not deleted" messages because vehicleid collisions could occur etc.
;prot  */
;prot int CreateVehicle(int model, struct vec4 *pos, int col1, int col2, int respawn_delay_ms);
;test __builtin_offsetof(struct SampNetGame, vehiclePool) == 0xC
;test __builtin_offsetof(struct vec4, r) == 0xC
global CreateVehicle:function
CreateVehicle:
	push edi
	push esi
	push dword [esp+01Ch] ; respawn_delay_ms
	push dword [esp+01Ch] ; col2
	push dword [esp+01Ch] ; col1
	mov eax, [esp+01Ch] ; pos
	push dword [eax+0Ch] ; pos->r
	push eax ; pos
	mov esi, [esp+020h] ; model
	push esi
	mov eax, [samp_pNetGame]
	mov eax, [eax+0Ch] ; vehiclePool
	push eax
	mov edi, 0x814CB10 ; SampVehiclePool::Add
	call edi ; SampVehiclePool::Add
	sub esi, 537 ; freight=537, streak=538
	cmp esi, 1
	ja end
	add esi, 569 ; freiflat=569, streakc=570
	mov [esp+04h], esi ; model
	call edi ; SampVehiclePool::Add
	call edi ; SampVehiclePool::Add
	call edi ; SampVehiclePool::Add
	sub eax, 3 ; need to return the vehicleid of the engine of course
end:
	add esp, 01Ch
	pop esi
	pop edi
	ret

;prot void RakServer__GetPlayerIDFromIndex(struct PlayerID *outPlayerId, struct RakServer *rakServer, short playerIndex);
global RakServer__GetPlayerIDFromIndex:function
RakServer__GetPlayerIDFromIndex:
	push dword [esp+0Ch] ; playerIndex
	push dword [esp+0Ch] ; rakServer
	push dword [esp+0Ch] ; outPlayerId
	mov eax, 0807C720h
	call eax ; thiscall with 1 parameter; pops 1 arg
	add esp, 08h
	ret

segment .data
logprintRetAddr	dd 00h

segment .rdata
aStreamDistance	db "stream_distance", 00h
aNewline	db 0Ah, 00h
