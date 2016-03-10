section .rodata


FUNC_ADDR_PLACEHOLDER    equ 0
FUNC_CALL_PLACEHOLDER    equ 0
WRAPPER_PRE_PLACEHOLDER  equ 0
WRAPPER_POST_PLACEHOLDER equ 0

;extern _ZN13CDetouredFunc10WrapperPreEPvPj
;extern _ZN13CDetouredFunc11WrapperPostEPvPj


global Wrapper_begin
global Wrapper_push_func_addr_1
global Wrapper_call_wrapper_pre
global Wrapper_call_actual_func
global Wrapper_push_func_addr_2
global Wrapper_call_wrapper_post
global Wrapper_end


Wrapper_begin:
	pusha
	lea eax,[esp+0x20]
	
	push ebp
	mov ebp,esp
	sub esp,0x200
	
	and esp,0xfffffff0
	fxsave [esp]
	
	push eax
Wrapper_push_func_addr_1:
	push strict dword FUNC_ADDR_PLACEHOLDER
Wrapper_call_wrapper_pre:
	call strict dword [WRAPPER_PRE_PLACEHOLDER]
	add esp,8
	
	fxrstor [esp]
	
	mov esp,ebp
	pop ebp
	popa
	
	
	add esp,4
Wrapper_call_actual_func:
	call strict dword [FUNC_CALL_PLACEHOLDER]
	push 0
	
	
	pusha
	lea eax,[esp+0x20]
	
	push ebp
	mov ebp,esp
	sub esp,0x200
	
	and esp,0xfffffff0
	fxsave [esp]
	
	push eax
Wrapper_push_func_addr_2:
	push strict dword FUNC_ADDR_PLACEHOLDER
Wrapper_call_wrapper_post:
	call strict dword [WRAPPER_POST_PLACEHOLDER]
	add esp,8
	
	fxrstor [esp]
	
	mov esp,ebp
	pop ebp
	popa
	ret
Wrapper_end:
