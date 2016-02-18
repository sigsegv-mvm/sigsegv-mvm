#ifndef _INCLUDE_SIGSEGV_DISASM_DISASM_H_
#define _INCLUDE_SIGSEGV_DISASM_DISASM_H_


class Disasm
{
public:
	Disasm() {};
	~Disasm();
	
	void Load();
	
private:
	// int 3
	// int
	// int0
	// int1
	// call
	// ret
	// jmp
	// jcc
	// iret
	// iretd
	
	// sysenter
	// sysexit
	
	// enter
	// leave
	
	// push ebp
	// mov ebp,esp
	
	// mov esp,ebp
	// pop ebp
	
	void ErrCheck(const char *what);
	
	csh m_Handle = 0;
	
	std::map<const void *, cs_insn *> m_Int3;
	std::map<const void *, cs_insn *> m_Ret;
};
extern Disasm g_Disasm;


#endif
