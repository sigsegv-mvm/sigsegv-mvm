; memory layout used by FXSAVE/FXRSTOR
struc FPUSTATE
	.fcw:   resw 1
	.fsw:   resw 1
	.ftw:   resb 1
	        resb 1
	.fop:   resw 1
	.fip:   resd 1
	.fcs:   resw 1
	        resw 1
	.fdp:   resd 1
	.fds:   resw 1
	        resw 1
	.mxcsr: resd 2
	
	.st0:   reso 1
	.st1:   reso 1
	.st2:   reso 1
	.st3:   reso 1
	.st4:   reso 1
	.st5:   reso 1
	.st6:   reso 1
	.st7:   reso 1
	
	.xmm0:  reso 1
	.xmm1:  reso 1
	.xmm2:  reso 1
	.xmm3:  reso 1
	.xmm4:  reso 1
	.xmm5:  reso 1
	.xmm6:  reso 1
	.xmm7:  reso 1
	
	        reso 14
endstruc
FPUSTATE_align equ 0x10
%if FPUSTATE_size != 0x200
	%error
%endif


%macro prologue 0
	push ebp
	mov ebp,esp
%endmacro

%macro epilogue 0
	leave
%endmacro


%macro savestate 0
	prologue
	
	sub esp,FPUSTATE_size
	and esp,~(FPUSTATE_align-1)
	fxsave [esp]
	
	push eax
	push ecx
	push edx
	push ebx
	push esi
	push edi
%endmacro

%macro reststate 0
	pop edi
	pop esi
	pop ebx
	pop edx
	pop ecx
	pop eax
	
	fxrstor [esp]
	
	epilogue
%endmacro


; global symbols: for access from C++
global __wrapper_Begin
global __wrapper_MOV_FuncAddr_1
global __wrapper_CALL_Pre
global __wrapper_CALL_Inner
global __wrapper_MOV_FuncAddr_2
global __wrapper_CALL_Post
global __wrapper_End

__wrapper_Begin          equ Wrapper.BEGIN
__wrapper_MOV_FuncAddr_1 equ Wrapper.A
__wrapper_CALL_Pre       equ Wrapper.B
__wrapper_CALL_Inner     equ Wrapper.C
__wrapper_MOV_FuncAddr_2 equ Wrapper.D
__wrapper_CALL_Post      equ Wrapper.E
__wrapper_End            equ Wrapper.END


; these placeholder values are intentionally in low memory so they're basically
; guaranteed to segfault if they aren't initialized but are then dereferenced
XX_FUNCADDR_1 equ 0x00000111
XX_CALL_PRE   equ 0x00000222
XX_CALL_INNER equ 0x00000333
XX_FUNCADDR_2 equ 0x00000444
XX_CALL_POST  equ 0x00000555
XX_RETADDR    equ 0x00000FFF


; calling convention: [[gnu::regparm(3)]]
%define REGP1 eax
%define REGP2 edx
%define REGP3 ecx

; calling convention: [[gnu::fastcall]] / __fastcall
%define FAST1 ecx
%define FAST2 edx

; ensure that NASM doesn't optimize any of our placeholder instructions into
; versions that use smaller-than-dword immediate values
%define strict4 strict dword


section .rodata

Wrapper:
.BEGIN:
	; WRAPPER-PRE BLOCK ================================================================================================
	savestate                       ; allocate a temporary stack frame and save all GP+FP register state
.A: mov FAST1,strict4 XX_FUNCADDR_1 ; wrapper-pre func arg 1: actual func addr
	lea FAST2,[ebp+0x4]             ; wrapper-pre func arg 2: ptr to our retaddr (i.e. actual func's caller)
.B:	call strict4 [XX_CALL_PRE]      ; call the wrapper-pre func: do pre things and save our retaddr
	reststate                       ; restore the GP+FP register state and tear down the temporary stack frame
	; WRAPPER-PRE BLOCK ================================================================================================
	
	; WRAPPER-INNER BLOCK ==============================================================================================
	add esp,4                       ; since we are calling rather than jumping, our retaddr needs to go away
.C:	call strict4 [XX_CALL_INNER]    ; call the actual func (or detour(s) thereof), with original reg+stack state
	push XX_RETADDR                 ; make a spot for our retaddr again (to be filled in by the wrapper-post func)
	; WRAPPER-INNER BLOCK ==============================================================================================
	
	; WRAPPER-POST BLOCK ===============================================================================================
	savestate                       ; allocate a temporary stack frame and save all GP+FP register state
.D: mov FAST1,strict4 XX_FUNCADDR_2 ; wrapper-post func arg 1: actual func addr
	lea FAST2,[ebp+0x4]             ; wrapper-post func arg 2: ptr to our retaddr (i.e. actual func's caller)
.E:	call strict4 [XX_CALL_POST]     ; call the wrapper-ost func: do post things and restore our retaddr
	reststate                       ; restore the GP+FP register state and tear down the temporary stack frame
	; WRAPPER-POST BLOCK ===============================================================================================
	
	ret                             ; we can safely return to the actual func's caller now
.END:
