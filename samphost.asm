extern logprintf
extern samp_pNetGame

extern hook_OnDriverSync
extern printf

;void InitAfterPoolsCreated:function();
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

;void printf_logprintf(char *format, ...);
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

;void DriverSyncHook();
global DriverSyncHook:function
DriverSyncHook:
	push dword [esi+0Ch] ; BitStream::ptrData
	push eax
	call hook_OnDriverSync
	add esp, 08h
	mov eax, 080AEB71h
	jmp eax

;float* samphost_GetPtrStreamDistance();
global samphost_GetPtrStreamDistance:function
samphost_GetPtrStreamDistance:
	push aStreamDistance
	mov eax, 081CA4B8h
	push dword [eax]
	mov eax, 080A00D0h
	call eax
	add esp, 0x8
	ret

segment .data
aStreamDistance	db "stream_distance", 00h
aNewline	db 0Ah, 00h
logprintRetAddr	dd 00h
