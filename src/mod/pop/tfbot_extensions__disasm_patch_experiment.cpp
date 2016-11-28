//	class CPatch_CTFBot_GetFlagToFetch : public IPatch
//	{
//	public:
//		CPatch_CTFBot_GetFlagToFetch() {}
//		
//		virtual bool Verbose() const override    { return false; }
//		virtual bool VerifyOnly() const override { return false; }
//		
//		virtual bool Init() override
//		{
//			
//		}
//		virtual bool Check() override
//		{
//			
//		}
//		
//		virtual void Apply() override
//		{
//			
//		}
//		virtual void UnApply() override
//		{
//			
//		}
//		
//	private:
//		Disassembler m_Disasm;
//		
//		void *m_pFuncAddr = nullptr;
//		
//		
//	};
	
	class CPatch_CTFBot_GetFlagToFetch : public CPatch
	{
	public:
		CPatch_CTFBot_GetFlagToFetch() : CPatch(s_Size), m_Buf(s_Size), m_Mask(s_Size) {}
		
		virtual const char *GetFuncName() const override { return "CTFBot::GetFlagToFetch"; }
		
		virtual uint32_t GetFuncOffMin() const override { return this->m_FuncOff; }
		virtual uint32_t GetFuncOffMax() const override { return this->m_FuncOff; }
		
		virtual bool GetVerifyInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			if (!this->m_bFound) return false;
			
			// TODO
			return false;
		}
		
		virtual bool GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			if (!this->m_bFound) return false;
			
			// TODO
			return false;
		}
		
	private:
		virtual bool PostInit() override
		{
			auto func_base = (uintptr_t)this->GetFuncAddr();
			
			auto l_is_call_to_addr = [](const auto& insn, void *addr){
				if (insn.ID() != X86_INS_CALL) return false;
				
				auto operands = insn.Operands();
				if (operands.size() != 1) return false;
				
				auto op0 = operands[0];
				if (op0.Type() != X86_OP_IMM || op0.Imm_U32() != (uintptr_t)addr) return false;
				
				return true;
			};
			
			auto l_is_push_imm32_arg = [](const auto& insn, uint32_t esp_off, uint32_t imm_val){
				if (insn.ID() != X86_INS_MOV) return false;
				
				auto operands = insn.Operands();
				if (operands.size() != 2) return false;
				
				auto op0 = operands[0];
				if (op0.Type() != X86_OP_MEM || op0.Size() != 4 || op0.Mem_Seg() != X86_REG_INVALID ||
					op0.Mem_Base() != X86_REG_ESP || op0.Mem_Index() != X86_REG_INVALID || op0.Mem_Disp() != esp_off) return false;
				
				auto op1 = operands[1];
				if (op1.Type() != X86_OP_IMM || op1.Size() != 4 || op1.Imm_U32() != imm_val) return false;
				
				return true;
			};
			
			auto l_is_test_al = [](const auto& insn){
				if (insn.ID() != X86_INS_TEST) return false;
				
				auto operands = insn.Operands();
				if (operands.size() != 2) return false;
				
				auto op0 = operands[0];
				if (op0.Type() != X86_OP_REG || op0.Reg() != X86_REG_AL) return false;
				
				auto op1 = operands[1];
				if (op1.Type() != X86_OP_REG || op1.Reg() != X86_REG_AL) return false;
				
				return true;
			};
			
			auto l_is_jcc_imm = [](const auto& insn){
				switch (insn.ID()) {
				case X86_INS_JAE:
				case X86_INS_JA:
				case X86_INS_JBE:
				case X86_INS_JB:
				case X86_INS_JE:
				case X86_INS_JGE:
				case X86_INS_JG:
				case X86_INS_JLE:
				case X86_INS_JL:
				case X86_INS_JNE:
				case X86_INS_JNO:
				case X86_INS_JNP:
				case X86_INS_JNS:
				case X86_INS_JO:
				case X86_INS_JP:
				case X86_INS_JS:
					break;
				default:
					return false;
				}
				
				auto operands = insn.Operands();
				if (operands.size() != 1) return false;
				
				auto op0 = operands[0];
				if (op0.Type() != X86_OP_IMM) return false;
				
				return true;
			};
			
			void *call_target = AddrManager::GetAddr("CTFPlayer::IsPlayerClass");
			if (call_target == nullptr) return false;
			
			Disassembler<true> disasm;
			auto result = disasm.Disassemble(func_base, s_DisasmLimit);
			
			auto it_arg4 = result.end();
			auto it_call = result.end();
			auto it_test = result.end();
			auto it_jcc  = result.end();
			
			for (auto i = result.begin(); i != result.end(); ++i) {
			//	auto insn = *i;
			//	uintptr_t off = insn.Addr() - func_base;
				
			//	if (off >= 0x5e2 && off < 0x600) {
			//		DevMsg("\n+0x%03x: %s %s\n", off, insn.MnemonicStr(), insn.OperandStr());
			//		
			//		DevMsg("Bytes:");
			//		for (auto byte : insn.Bytes()) {
			//			DevMsg(" %02X", byte);
			//		}
			//		DevMsg("\n");
			//	}
				
				/* find the call to CTFPlayer::IsPlayerClass */
				if (l_is_call_to_addr(*i, call_target)) {
					it_call = i;
					DevMsg("Found call @ +0x%03x\n", (*it_call).Addr() - func_base);
					
					for (auto j = i - 1; j >= i - 4 && j != result.begin(); --j) {
						if (it_arg4 == result.end() && l_is_push_imm32_arg(*j, 0x4, TF_CLASS_ENGINEER)) {
							it_arg4 = j;
							DevMsg("Found arg4 @ +0x%03x\n", (*it_arg4).Addr() - func_base);
						}
					}
					
					for (auto j = i + 1; j <= i + 1 && j != result.end(); ++j) {
						if (it_test == result.end() && l_is_test_al(*j)) {
							it_test = j;
							DevMsg("Found test @ +0x%03x\n", (*it_test).Addr() - func_base);
						}
					}
					
					for (auto j = i + 2; j <= i + 2 && j != result.end(); ++j) {
						if (it_jcc == result.end() && l_is_jcc_imm(*j)) {
							it_jcc = j;
							DevMsg("Found jcc @ +0x%03x\n", (*it_jcc).Addr() - func_base);
						}
					}
				}
			}
			
		#if 0
			bool result = disasm.IterateRange(this->GetFuncAddr(), s_DisasmLimit, [=](const InstructionDetailed& insn){
				uint32_t off = insn.Addr() - (uintptr_t)this->GetFuncAddr();
				if (off >= 0x5e2 && off < 0x600) {
					DevMsg("\n+0x%03x: %s %s\n", off, insn.MnemonicStr(), insn.OperandStr());
					
					DevMsg("Bytes:");
					for (auto byte : insn.Bytes()) {
						DevMsg(" %02X", byte);
					}
					DevMsg("\n");
					
					
				}
				
				/*
				DevMsg("Insn @ %s+0x%03x: mnemonic '%s' opcode '%s'\n",
					this->GetFuncName(), (insn.Addr() - (uintptr_t)this->GetFuncAddr()),
					insn.MnemonicStr(), insn.OperandStr());
				
				if (insn.ID() != X86_INS_CALL) return true;
				
				DevMsg("Call @ %s+0x%03x: mnemonic '%s' opcode '%s'\n",
					this->GetFuncName(), (insn.Addr() - (uintptr_t)this->GetFuncAddr()),
					insn.MnemonicStr(), insn.OperandStr());
				DevMsg("Bytes:");
				for (auto byte : insn.Bytes()) {
					DevMsg(" %02X", byte);
				}
				DevMsg("\n");*/
				
				return true;
				
				// TODO: return false if we find what we're looking for
				// also set m_bFound
				// also set m_FuncOff
				// also set up m_Buf and m_Mask
			});
			
			DevMsg("PostInit: result is %d, error is '%s'\n", result, disasm.ErrorString());
		#endif
			
			// TODO: return false upon failure
			return true;
		}
		
		static constexpr size_t s_Size = 0x05;
		
		// ServerLinux 20161119a: @ +0x5f3
		static constexpr size_t s_DisasmLimit = 0x660;
		
		bool m_bFound = false;
		uint32_t m_FuncOff = 0;
		
		ByteBuf m_Buf;
		ByteBuf m_Mask;
	};
