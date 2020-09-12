extern hook_OnDriverSync
extern logprintf
extern printf

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
