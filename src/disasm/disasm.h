#ifndef _INCLUDE_SIGSEGV_DISASM_DISASM_H_
#define _INCLUDE_SIGSEGV_DISASM_DISASM_H_


////////////////////////////////////////////////////////////////////////////////
// Read-only pointer+count array accessor //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template<typename T, typename SIZE>
class ArrayView
{
public:
	ArrayView(const T *ptr, SIZE count) : m_pArray(ptr), m_Count(count) {}
	
	ArrayView(const ArrayView&) = delete;
	ArrayView(ArrayView&&)      = default;
	
	const T *begin() const { return this->m_pArray; }
	const T *end() const   { return this->m_pArray + this->m_Count; }
	
	const T *cbegin() const { return begin(); }
	const T *cend() const   { return end(); }
	
	SIZE size() const  { return this->m_Count; }
	bool empty() const { return (this->m_Count == 0); }
	
	const T& front() const { return this->m_pArray[0]; }
	const T& back() const  { return this->m_pArray[this->m_Count - 1]; }
	
	const T& operator[](SIZE idx) { return this->m_pArray[idx]; }
	
private:
	const T *m_pArray;
	SIZE m_Count;
};


////////////////////////////////////////////////////////////////////////////////
// Operand info ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class Operand : private cs_x86_op
{
public:
	x86_op_type  Type() const { return this->type; }
	uint_fast8_t Size() const { return this->size; }
	
	x86_reg Reg() const { return this->reg; }
	int64_t Imm() const { return this->imm; }
	double  FP () const { return this->fp;  }
	
	 uint8_t Imm_U8 () const { return static_cast< uint8_t>(this->imm); }
	  int8_t Imm_S8 () const { return static_cast<  int8_t>(this->imm); }
	uint16_t Imm_U16() const { return static_cast<uint16_t>(this->imm); }
	 int16_t Imm_S16() const { return static_cast< int16_t>(this->imm); }
	uint32_t Imm_U32() const { return static_cast<uint32_t>(this->imm); }
	 int32_t Imm_S32() const { return static_cast< int32_t>(this->imm); }
	uint64_t Imm_U64() const { return static_cast<uint64_t>(this->imm); }
	 int64_t Imm_S64() const { return static_cast< int64_t>(this->imm); }
	
	x86_reg      Mem_Seg  () const { return static_cast<x86_reg>(this->mem.segment); }
	x86_reg      Mem_Base () const { return static_cast<x86_reg>(this->mem.base   ); }
	x86_reg      Mem_Index() const { return static_cast<x86_reg>(this->mem.index  ); }
	uint_fast8_t Mem_Scale() const { return this->mem.scale;                         }
	int64_t      Mem_Disp () const { return this->mem.disp;                          }
	
	x86_avx_bcast AVX_Broadcast () const { return this->avx_bcast;       }
	bool          AVX_ZeroOpMask() const { return this->avx_zero_opmask; }
};
static_assert(sizeof(Operand) == sizeof(cs_x86_op), "");

////////////////////////////////////////////////////////////////////////////////
// Non-detailed instruction info ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class InstructionBase
{
private:
	using BytesView = ArrayView<uint8_t, uint_fast16_t>;
	
public:
	InstructionBase(const cs_insn *insn) : m_pInsn(insn) {}
	
	x86_insn ID  () const { return static_cast<x86_insn>(Insn().id); }
	uint32_t Addr() const { return Insn().address;                   }
	
	BytesView Bytes() const { return BytesView(Insn().bytes, Insn().size); }
	
	const char *MnemonicStr() const { return Insn().mnemonic; }
	const char *OperandStr () const { return Insn().op_str;   }
	
protected:
	const cs_insn& Insn() const { return *this->m_pInsn; }
	
private:
	const cs_insn *m_pInsn;
};

////////////////////////////////////////////////////////////////////////////////
// Detailed-only instruction info //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class InstructionDetailed : public InstructionBase
{
private:
	using RegsView    = ArrayView<uint8_t, uint_fast8_t>;
	using GroupsView  = ArrayView<uint8_t, uint_fast8_t>;
	using OpcodeView  = ArrayView<uint8_t, uint_fast8_t>;
	using OperandView = ArrayView<Operand, uint_fast8_t>;
	
public:
	InstructionDetailed(const cs_insn *insn) : InstructionBase(insn) {}
	
	RegsView RegsRead () const { return RegsView(Detail().regs_read,  Detail().regs_read_count);  }
	RegsView RegsWrite() const { return RegsView(Detail().regs_write, Detail().regs_write_count); }
	
	GroupsView Groups() const  { return GroupsView(Detail().groups, Detail().groups_count); }
	
	x86_prefix PrefixRepLock () const { return static_cast<x86_prefix>(X86().prefix[0]); }
	x86_prefix PrefixSegment () const { return static_cast<x86_prefix>(X86().prefix[1]); }
	x86_prefix PrefixOpSize  () const { return static_cast<x86_prefix>(X86().prefix[2]); }
	x86_prefix PrefixAddrSize() const { return static_cast<x86_prefix>(X86().prefix[3]); }
	
	OpcodeView Opcode() const { return OpcodeView(X86().opcode, 4); }
	
	uint8_t REX     () const { return X86().rex;       }
	uint8_t AddrSize() const { return X86().addr_size; }
	uint8_t ModRM   () const { return X86().modrm;     }
	uint8_t SIB     () const { return X86().sib;       }
	uint8_t Disp    () const { return X86().disp;      }
	
	x86_reg SIB_Index() const { return X86().sib_index; }
	uint8_t SIB_Scale() const { return X86().sib_scale; }
	x86_reg SIB_Base () const { return X86().sib_base;  }
	
	x86_sse_cc SSE_CC () const { return X86().sse_cc;  }
	x86_avx_cc AVX_CC () const { return X86().avx_cc;  }
	bool       AVX_SAE() const { return X86().avx_sae; }
	x86_avx_rm AVX_RM () const { return X86().avx_rm;  }
	
	OperandView Operands() const { return OperandView(reinterpret_cast<const Operand *>(X86().operands), X86().op_count); }
	
	// TODO: implement these funcs manually, without need for the csh handle
//	bool IsInGroup   (x86_insn_group group_id) const { return cs_insn_group(this->m_Handle, &this->Insn(), group_id); }
//	bool DoesReadReg (x86_reg reg_id)   const { return cs_reg_read  (this->m_Handle, &this->Insn(), reg_id);   }
//	bool DoesWriteReg(x86_reg reg_id)   const { return cs_reg_write (this->m_Handle, &this->Insn(), reg_id);   }
//	int  NumOperands (x86_op_type op_type)  const { return cs_op_count  (this->m_Handle, &this->Insn(), op_type);  }
	
private:
	const cs_detail& Detail() const { return *this->Insn().detail; }
	const cs_x86&    X86   () const { return this->Detail().x86;   }
};


template<bool DETAIL>
class DisasmResult
{
public:
	using InstructionType = std::conditional_t<DETAIL, InstructionDetailed, InstructionBase>;
	
	DisasmResult(size_t count, cs_insn *insns) :
		m_InsnCount(count), m_pInsnBuffer(insns)
	{
		for (size_t i = 0; i < count; ++i) {
			this->m_InsnVector.push_back(InstructionType(&insns[i]));
		}
	}
	~DisasmResult()
	{
		if (this->m_pInsnBuffer != nullptr) {
			cs_free(this->m_pInsnBuffer, this->m_InsnCount);
		}
	}
	
	DisasmResult(const DisasmResult&) = delete;
	DisasmResult(DisasmResult&&)      = default;
	
	auto begin() const { return this->m_InsnVector.cbegin(); }
	auto end() const   { return this->m_InsnVector.cend(); }
	
	auto cbegin() const { return this->m_InsnVector.cbegin(); }
	auto cend() const   { return this->m_InsnVector.cend(); }
	
	size_t size() const { return this->m_InsnVector.size(); }
	bool empty() const  { return this->m_InsnVector.empty(); }
	
	const auto& front() const { return this->m_InsnVector.front(); }
	const auto& back() const  { return this->m_InsnVector.back(); }
	
	const auto& operator[](size_t idx) { return this->m_InsnVector[idx]; }
	
private:
	size_t m_InsnCount;
	cs_insn *m_pInsnBuffer;
	
	std::vector<InstructionType> m_InsnVector;
};


template<bool DETAIL = true>
class Disassembler
{
private:
	static constexpr bool SKIP_DATA = false;
	
public:
	Disassembler()
	{
		auto arch = static_cast<cs_arch>(CS_ARCH_X86);
		auto mode = static_cast<cs_mode>(CS_MODE_32 | CS_MODE_LITTLE_ENDIAN);
		
		this->m_InitError = cs_open(arch, mode, &this->m_Handle);
		
		if (this->Initialized()) {
			(void)cs_option(this->m_Handle, CS_OPT_SYNTAX,   CS_OPT_SYNTAX_INTEL);
			(void)cs_option(this->m_Handle, CS_OPT_DETAIL,   (DETAIL    ? CS_OPT_ON : CS_OPT_OFF));
			(void)cs_option(this->m_Handle, CS_OPT_SKIPDATA, (SKIP_DATA ? CS_OPT_ON : CS_OPT_OFF));
		}
	}
	~Disassembler()
	{
		if (this->m_Handle != 0) {
			(void)cs_close(&this->m_Handle);
		}
	}
	
	cs_err InitError() const { return this->m_InitError; }
	
	cs_err ErrorCode() const        { return cs_errno(this->m_Handle); }
	const char *ErrorString() const { return cs_strerror(this->ErrorCode()); }
	
	DisasmResult<DETAIL> Disassemble(uintptr_t addr, size_t len, uintptr_t offset = 0, size_t max_insns = 0)
	{
		cs_insn *insns = nullptr;
		size_t count = cs_disasm(this->m_Handle, reinterpret_cast<const uint8_t *>(addr), len, addr + offset, max_insns, &insns);
		
		return DisasmResult<DETAIL>(count, insns);
	}
	
	template<typename FUNCTOR> bool IterateRange(uintptr_t addr, size_t len, FUNCTOR&& func)
	{
		using InstructionType = std::conditional_t<DETAIL, InstructionDetailed, InstructionBase>;
		
		auto     iter_code = reinterpret_cast<const uint8_t *>(addr);
		size_t   iter_size = len;
		auto     iter_addr = (uint64_t)addr;
		cs_insn *iter_insn = cs_malloc(this->m_Handle);
		
		do {
			if (!cs_disasm_iter(this->m_Handle, &iter_code, &iter_size, &iter_addr, iter_insn)) {
				cs_free(iter_insn, 1);
				return false;
			}
		} while (func(InstructionType(iter_insn)));
		
		cs_free(iter_insn, 1);
		return true;
	}
	template<typename FUNCTOR> bool Iterate(uintptr_t addr, FUNCTOR&& func)
	{
		return this->IterateRange(addr, SIZE_MAX, func);
	}
	
	// TODO: version of Iterate with num-of-instrs restriction
	
	const char *RegName(x86_reg reg_id) const            { return cs_reg_name  (this->m_Handle, reg_id);   }
	const char *InsnName(x86_insn insn_id) const         { return cs_insn_name (this->m_Handle, insn_id);  }
	const char *GroupName(x86_insn_group group_id) const { return cs_group_name(this->m_Handle, group_id); }
	
private:
	bool Initialized() const { return (this->m_InitError == CS_ERR_OK && this->m_Handle != 0); }
	
	csh m_Handle = 0;
	cs_err m_InitError;
};


#endif
