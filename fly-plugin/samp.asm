extern logprintf
extern samp_pNetGame

extern hook_OnDriverSync
extern printf

;prot void InitAfterPoolsCreated();
;test *(int*) 0x80AB939 == 0xFFFFD893 /*The logprintf call.*/
;test *(int*) 0x80AA0E8 == 0x81CA4BC /*The write to pNetGame.*/
global InitAfterPoolsCreated:function
InitAfterPoolsCreated:
	; get netgame
	mov eax, 081CA4BCh
	mov eax, [eax]
	mov [samp_pNetGame], eax

	; the hook replaced a call to logprintf
	push 080AB93Dh
	mov eax, 080A91D0h
	jmp eax

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

;prot void DriverSyncHook();
global DriverSyncHook:function
DriverSyncHook:
	mov ecx, [esi+0Ch] ; BitStream::ptrData
	inc ecx ; to skip packet_id
	mov [esp+4], ecx
	mov [esp], eax
	call hook_OnDriverSync
	mov eax, 080AEB71h
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

segment .data
aStreamDistance	db "stream_distance", 00h
aNewline	db 0Ah, 00h
logprintRetAddr	dd 00h
