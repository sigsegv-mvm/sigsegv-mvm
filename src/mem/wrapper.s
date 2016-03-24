STACK_ALIGN equ 0x10

LOCAL_GPREGS  equ 0x004
LOCAL_FPSTATE equ 0x220
LOCAL_TOTAL   equ 0x240


struc GPREGS
	.eax: resd 1
	.ecx: resd 1
	.edx: resd 1
	.ebx: resd 1
	.esi: resd 1
	.edi: resd 1
endstruc

struc FPUSTATE
	.fcw:  resw 1
	.fsw:  resw 1
	.ftw:  resw 1
	.fop:  resw 1
	.ip:   resd 1
	.cs:   resw 1
	       resw 1
	.st0:  reso 1
	.st1:  reso 1
	.st2:  reso 1
	.st3:  reso 1
	.st4:  reso 1
	.st5:  reso 1
	.st6:  reso 1
	.st7:  reso 1
	.xmm0: reso 1
	.xmm1: reso 1
	.xmm2: reso 1
	.xmm3: reso 1
	.xmm4: reso 1
	.xmm5: reso 1
	.xmm6: reso 1
	.xmm7: reso 1
	       reso 14
endstruc


%macro prologue 1
	push ebp
	mov ebp,esp
	sub esp,%1
%endmacro

%macro epilogue 0
	mov esp,ebp
	pop ebp
%endmacro

%macro saveregs 0
	mov [ebp-(LOCAL_GPREGS+GPREGS.eax)],eax
	mov [ebp-(LOCAL_GPREGS+GPREGS.ecx)],ecx
	mov [ebp-(LOCAL_GPREGS+GPREGS.edx)],edx
	mov [ebp-(LOCAL_GPREGS+GPREGS.ebx)],ebx
	mov [ebp-(LOCAL_GPREGS+GPREGS.esi)],esi
	mov [ebp-(LOCAL_GPREGS+GPREGS.edi)],edi
	
	lea ebx,[ebp-LOCAL_FPSTATE]
	and ebx,~(STACK_ALIGN-1)
	fxsave [ebx]
%endmacro

%macro restregs 0
	lea ebx,[ebp-LOCAL_FPSTATE]
	and ebx,~(STACK_ALIGN-1)
	fxrstor [ebx]
	
	mov edi,[ebp-(LOCAL_GPREGS+GPREGS.edi)]
	mov esi,[ebp-(LOCAL_GPREGS+GPREGS.esi)]
	mov ebx,[ebp-(LOCAL_GPREGS+GPREGS.ebx)]
	mov edx,[ebp-(LOCAL_GPREGS+GPREGS.edx)]
	mov ecx,[ebp-(LOCAL_GPREGS+GPREGS.ecx)]
	mov eax,[ebp-(LOCAL_GPREGS+GPREGS.eax)]
%endmacro


global Wrapper_begin
global Wrapper_push_func_addr_1
global Wrapper_call_wrapper_pre
global Wrapper_call_actual_func
global Wrapper_push_func_addr_2
global Wrapper_call_wrapper_post
global Wrapper_end

Wrapper_begin             equ Wrapper
Wrapper_push_func_addr_1  equ Wrapper.A
Wrapper_call_wrapper_pre  equ Wrapper.B
Wrapper_call_actual_func  equ Wrapper.C
Wrapper_push_func_addr_2  equ Wrapper.D
Wrapper_call_wrapper_post equ Wrapper.E
Wrapper_end               equ Wrapper.F


PLACEHOLDER1 equ 0x00001111
PLACEHOLDER2 equ 0x00002222
PLACEHOLDER3 equ 0x00003333
PLACEHOLDER4 equ 0x00004444
PLACEHOLDER5 equ 0x00005555


section .rodata

Wrapper:
	prologue LOCAL_TOTAL
	saveregs
	
	lea eax,[ebp+0x4]
	push eax
.A:	push strict dword PLACEHOLDER1
.B:	call strict dword [PLACEHOLDER2]
	add esp,8
	
	restregs
	epilogue
	add esp,4
.C:	call strict dword [PLACEHOLDER3]
	push 0
	prologue LOCAL_TOTAL
	saveregs
	
	lea eax,[ebp+0x4]
	push eax
.D:	push strict dword PLACEHOLDER4
.E:	call strict dword [PLACEHOLDER5]
	add esp,8
	
	restregs
	epilogue
	ret
.F:
