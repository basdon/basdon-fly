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

;/**
;Crashes if vehicle does not exist.
;*/
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

;/**
;Crashes if vehicle does not exist.
;{@param rot} must be ptr to {@code struct { float w, x, y, z }}.
;*/
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

;/**
;Crashes if vehicle does not exist.
;*/
;prot void GetVehicleZAngleUnsafe(int vehicleid, float *angle);
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

;prot void RakServer__GetPlayerIDFromIndex(struct PlayerID *outPlayerId, struct RakServer *rakServer, short playerIndex);
global RakServer__GetPlayerIDFromIndex:function
RakServer__GetPlayerIDFromIndex:
	push dword [esp+0Ch] ; playerIndex
	push dword [esp+0Ch] ; rakServer
	push dword [esp+0Ch] ; outPlayerId
	mov eax, 0807C720h
	call eax ; thiscall with 1 parameter; pops 1 arg
	add esp, 0x8
	ret

segment .data
logprintRetAddr	dd 00h

segment .rdata
aStreamDistance	db "stream_distance", 00h
aNewline	db 0Ah, 00h
