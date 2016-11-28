#include "disasm/disasm.h"
#include "library.h"
#include "util/prof.h"


Disasm g_Disasm;


#if 0
#include <distorm.h>
#include <mnemonics.h>
namespace Disasm_diStorm
{
	void Test()
	{
		DevMsg("Disasm_diStorm::Test BEGIN\n");
		
		LibInfo info = LibMgr::GetInfo(Library::SERVER);
		
		uint32_t lib_base = info.baseaddr;
		
		uint32_t text_off = info.segs[".text"].off;
		uint32_t text_len = info.segs[".text"].len;
		
		
		// _DInst: 64 bytes per instruction
		
		// all instructions:
		// 2,305,482
		// 148 MB
		
		// just flow control:
		// 706,387
		// 45 MB
		
		
		{
			_CodeInfo codeinfo;
			codeinfo.codeOffset = lib_base + text_off;
			codeinfo.code       = (const uint8_t *)(lib_base + text_off);
			codeinfo.codeLen    = text_len;
			codeinfo.dt         = Decode32Bits;
			codeinfo.features   = DF_NONE;
			
			constexpr unsigned int maxInstructions = 3000000;
			DevMsg("  sizeof(OFFSET_INTEGER) = %u\n", sizeof(OFFSET_INTEGER));
			DevMsg("  sizeof(_DInst) = %u\n", sizeof(_DInst));
			DevMsg("  calling malloc(%u)\n", sizeof(_DInst) * maxInstructions);
			_DInst *instrs = (_DInst *)malloc(sizeof(_DInst) * maxInstructions);
			DevMsg("  malloc returned: %08x\n", (uintptr_t)instrs);
			
			Prof::Begin();
			unsigned int usedInstructionsCount = 0;
			_DecodeResult result = distorm_decompose(&codeinfo, instrs, maxInstructions, &usedInstructionsCount);
			Prof::End("diStorm all instrs");
			
			DevMsg("  result: %d\n", result);
			DevMsg("  usedInstructionsCount: %u\n", usedInstructionsCount);
			
			FILE *file = fopen("D:\\server.txt", "w");
			if (file != nullptr) {
				char *buf = (char *)malloc(1024 * 1024);
				setvbuf(file, buf, _IOFBF, 1024 * 1024);
				
				for (unsigned int i = 0; i < usedInstructionsCount; ++i) {
					const _DInst *ins = instrs + i;
					
					uintptr_t addr = ins->addr - lib_base;
					
					if (ins->opcode == I_INT_3) {
						fprintf(file, "%08x: int 0x3\n", addr);
					} else if (ins->opcode == I_RET) {
						if (ins->ops[0].type == O_NONE) {
							fprintf(file, "%08x: ret\n", addr);
						} else if (ins->ops[0].type == O_IMM) {
							fprintf(file, "%08x: ret 0x%x\n", addr, ins->imm.word);
						} else {
							fprintf(file, "%08x: ret ???\n", addr);
						}
					} else if (ins->opcode == I_PUSH) {
					//	fprintf(file, "%08x: push [type:%x index:%x size:%x]\n", addr,
					//		ins->ops[0].type, ins->ops[0].index, ins->ops[0].size);
						if (ins->ops[0].type == O_REG && ins->ops[0].index == R_EBP) {
							fprintf(file, "%08x: push ebp\n", addr);
						}
					}
				}
				
				fclose(file);
				free(buf);
			}
			
			free(instrs);
		}
		
		{
			_CodeInfo codeinfo;
			codeinfo.codeOffset = lib_base + text_off;
			codeinfo.code       = (const uint8_t *)(lib_base + text_off);
			codeinfo.codeLen    = text_len;
			codeinfo.dt         = Decode32Bits;
			codeinfo.features   = DF_RETURN_FC_ONLY;
			
			constexpr unsigned int maxInstructions = 3000000;
			DevMsg("  sizeof(OFFSET_INTEGER) = %u\n", sizeof(OFFSET_INTEGER));
			DevMsg("  sizeof(_DInst) = %u\n", sizeof(_DInst));
			DevMsg("  calling malloc(%u)\n", sizeof(_DInst) * maxInstructions);
			_DInst *instrs = (_DInst *)malloc(sizeof(_DInst) * maxInstructions);
			DevMsg("  malloc returned: %08x\n", (uintptr_t)instrs);
			
			Prof::Begin();
			unsigned int usedInstructionsCount = 0;
			_DecodeResult result = distorm_decompose(&codeinfo, instrs, maxInstructions, &usedInstructionsCount);
			Prof::End("diStorm flow control only");
			
			DevMsg("  result: %d\n", result);
			DevMsg("  usedInstructionsCount: %u\n", usedInstructionsCount);
			
			free(instrs);
		}
		
		DevMsg("Disasm_diStorm::Test END\n");
	}
}
#endif


#if 0
#include <capstone.h>
namespace Disasm_Capstone
{
	csh handle = 0;
	
	
	void *my_malloc(size_t size)
	{
		DevMsg("    malloc(%u)\n", size);
		return malloc(size);
	}
	void *my_calloc(size_t nmemb, size_t size)
	{
		DevMsg("    calloc(%u, %u)\n", nmemb, size);
		return calloc(nmemb, size);
	}
	void *my_realloc(void *ptr, size_t size)
	{
		DevMsg("    realloc(%08x, %u)\n", (uintptr_t)ptr, size);
		return realloc(ptr, size);
	}
	void my_free(void *ptr)
	{
		DevMsg("    free(%08x)\n", (uintptr_t)ptr);
		free(ptr);
	}
	int my_vsnprintf(char *str, size_t size, const char *format, va_list ap)
	{
		return vsnprintf(str, size, format, ap);
	}
	
	
	void ErrCheck(const char *what)
	{
		cs_err err = cs_errno(handle);
		if (err != cs_err::CS_ERR_OK) {
			DevMsg("  error in %s: %s\n", what, cs_strerror(err));
		}
	}
	
	void Test()
	{
		DevMsg("Disasm_Capstone::Test BEGIN\n");
		
		LibInfo info = LibMgr::GetInfo(Library::SERVER);
		
		uint32_t lib_base = info.baseaddr;
		
		uint32_t text_off = info.segs[".text"].off;
		uint32_t text_len = info.segs[".text"].len;
		
		uint32_t code_begin = lib_base + text_off;
		uint32_t code_end   = lib_base + text_off + text_len;
		
		
		cs_opt_mem my_mem = {
			&my_malloc,
			&my_calloc,
			&my_realloc,
			&my_free,
			&my_vsnprintf,
		};
		
		
		cs_open(CS_ARCH_X86, CS_MODE_32, &handle);
		ErrCheck("cs_open");
		
		cs_option(handle, CS_OPT_SYNTAX,   CS_OPT_SYNTAX_INTEL);
		ErrCheck("CS_OPT_SYNTAX");
		cs_option(handle, CS_OPT_DETAIL,   CS_OPT_OFF);
		ErrCheck("CS_OPT_DETAIL");
		cs_option(handle, CS_OPT_SKIPDATA, CS_OPT_ON);
		ErrCheck("CS_OPT_SKIPDATA");
		cs_option(handle, CS_OPT_MEM, (size_t)&my_mem);
		ErrCheck("CS_OPT_MEM");
		
		
		cs_insn *insn = cs_malloc(handle);
		
		const uint8_t *code = (const uint8_t *)code_begin;
		size_t         size = text_len;
		uint64_t    address = code_begin;
		
		FILE *file = fopen("D:\\server.txt", "w");
		
		long n_insns = 0;
		while (cs_disasm_iter(handle, &code, &size, &address, insn)) {
			fprintf(file, "    %08llx: %s\t%s\n", insn->address, insn->mnemonic, insn->op_str);
			++n_insns;
		}
		DevMsg("  %ld insns\n", n_insns);
		
		fclose(file);
		
		cs_free(insn, 1);
		
		cs_close(&handle);
		
		DevMsg("Disasm_Capstone::Test END\n");
	}
}
#endif


#if 0
#include <X86Disasm.hh>
namespace Disasm_Capstone_Cxx
{
	void Test()
	{
		DevMsg("Disasm_Capstone::Test BEGIN\n");
		
		LibInfo info = LibMgr::GetInfo(Library::SERVER);
		
		uint32_t lib_base = info.baseaddr;
		
		uint32_t text_off = info.segs[".text"].off;
		uint32_t text_len = info.segs[".text"].len;
		
		
		CX86Disasm86 dis;
		
		cs_err err;
		if ((err = dis.GetError()) != CS_ERR_OK) {
			DevMsg("  capstone error: '%s'\n", dis.ErrToStr(err));
		}
		
		dis.SetSyntax(cs_opt_value::CS_OPT_SYNTAX_INTEL);
		dis.SetDetail(cs_opt_value::CS_OPT_OFF);
		dis.SetSkipData(cs_opt_value::CS_OPT_ON);
		
		auto insns = dis.Disasm((const void *)(lib_base + text_off), text_len, lib_base + text_off);
//		if (!insns.get()) {
//			DevMsg("  can't get insn\n");
//		}
		
		FILE *file = fopen("D:\\server.txt", "w");
		DevMsg("  %u insns\n", insns->Count);
		for (size_t i = 0; i < insns->Count; ++i) {
			auto insn = insns->Instructions(i);
			
			fprintf(file, "-> %08llx: %s\t%s\n", insn->address, insn->mnemonic, insn->op_str);
		}
		fclose(file);
		
		DevMsg("Disasm_Capstone::Test END\n");
	}
}
#endif


#if 0
#include <udis86.h>
namespace Disasm_udis86
{
	void Test()
	{
		DevMsg("Disasm_udis86::Test BEGIN\n");
		DevMsg("Disasm_udis86::Test END\n");
	}
}
#endif


#if 0
namespace Disasm
{
	void Test()
	{
#if defined _MSC_VER
		Disasm_diStorm::Test();
		Disasm_Capstone::Test();
//		Disasm_CapstoneCxx::Test();
//		Test_udis86();
#endif
	}
}
#endif


Disasm::~Disasm()
{
	// TODO: free memory
}


void Disasm::ErrCheck(const char *what)
{
	cs_err err = cs_errno(this->m_Handle);
	if (err != cs_err::CS_ERR_OK) {
		DevMsg("  error in %s: %s\n", what, cs_strerror(err));
	}
}

void Disasm::Load()
{
	DevMsg("Disasm_Capstone::Test BEGIN\n");
	
	LibInfo info = LibMgr::GetInfo(Library::SERVER);
	
	uint32_t lib_base = info.baseaddr;
	
	uint32_t text_off = info.segs[".text"].off;
	uint32_t text_len = info.segs[".text"].len;
	
	uint32_t code_begin = lib_base + text_off;
	uint32_t code_end   = lib_base + text_off + text_len;
	
	cs_open(CS_ARCH_X86, CS_MODE_32, &this->m_Handle);
	ErrCheck("cs_open");
	
	cs_option(this->m_Handle, CS_OPT_SYNTAX,   CS_OPT_SYNTAX_INTEL);
	ErrCheck("CS_OPT_SYNTAX");
	cs_option(this->m_Handle, CS_OPT_DETAIL,   CS_OPT_ON); // TODO: disable detail if possible
	ErrCheck("CS_OPT_DETAIL");
	cs_option(this->m_Handle, CS_OPT_SKIPDATA, CS_OPT_ON);
	ErrCheck("CS_OPT_SKIPDATA");
//	cs_option(this->m_Handle, CS_OPT_MEM, (size_t)&my_mem);
//	ErrCheck("CS_OPT_MEM");
	
	cs_insn *insn = cs_malloc(this->m_Handle);
	
	const uint8_t *code = (const uint8_t *)code_begin;
	size_t         size = text_len;
	uint64_t    address = code_begin;
	
	FILE *file = fopen("D:\\server.txt", "w");
	
	
	// PASS: iterate all insns, store all immediate operands that are addrs in .text
	// PASS: iterate thru .rdata, store all dword-aligned dwords that are addrs in .text
	// (in both cases, filter out addrs which are not 0x10-aligned)
	
	// treat all these addrs as "potential func start points"
	// so if we have a long forward jmp and pass one of these addrs, assume the func ended
	
	
	struct FuncInfo
	{
		uint32_t addr;
		uint32_t len;
	};
	
	std::vector<FuncInfo> funcs;
	
	FuncInfo func = {(uint32_t)address, 0};
	bool in_func = true;
	
	while (cs_disasm_iter(this->m_Handle, &code, &size, &address, insn)) {
		
		
		if (insn->id == X86_INS_INT3) {
			if (in_func) {
				func.len = insn->address - func.addr;
				funcs.push_back(func);
				in_func = false;
			}
		} else {
			if (!in_func) {
				func.addr = insn->address;
				in_func = true;
			}
		}
	}
	
	if (in_func) {
		func.len = insn->address - func.addr;
		funcs.push_back(func);
	}
	
	for (const auto& func : funcs) {
		fprintf(file, "  func: %08x\n", 0x10001000 + func.addr - code_begin);
		fprintf(file, "        %08x\n", 0x10001000 + func.addr + func.len - code_begin - 1);
		fprintf(file, "\n");
	}
	
	
	// TODO: try to handle switch jump tables properly
	// - look at how they work
	// - see how we may be mishandling them
	// - figure out a way to handle them properly
	
	
#if 0
	long n_insns = 0;
	std::map<std::string, long> stats;
	while (cs_disasm_iter(this->m_Handle, &code, &size, &address, insn)) {
		++stats[insn->mnemonic];
//		fprintf(file, "    %08llx: %s\t%s\n", insn->address, insn->mnemonic, insn->op_str);
		++n_insns;
	}
	DevMsg("  %ld insns\n", n_insns);
	
	struct InsnStat
	{
		std::string name;
		long count;
	};
	std::vector<InsnStat> sorted;
	
	for (const auto& pair : stats) {
		sorted.push_back({pair.first, pair.second});
	}
	std::sort(sorted.begin(), sorted.end(), [](InsnStat& lhs, InsnStat& rhs){
			return lhs.count > rhs.count;
		});
	
	for (const auto& stat : sorted) {
		DevMsg("  %6ld %s\n", stat.count, stat.name.c_str());
	}
#endif
	
	
	fclose(file);
	
	cs_free(insn, 1);
	cs_close(&this->m_Handle);
	
	DevMsg("Disasm_Capstone::Test END\n");
}
