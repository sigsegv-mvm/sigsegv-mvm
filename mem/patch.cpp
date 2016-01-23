#include "mem/patch.h"
#include "extension.h"


bool IPatch::Init()
{
	this->m_pszFuncName = this->GetFuncName();
	this->m_iFuncOffMin = this->GetFuncOffsetMin();
	this->m_iFuncOffMax = this->GetFuncOffsetMax();
	
	this->m_pFuncAddr = AddrManager::GetAddr(this->m_pszFuncName);
	if (this->m_pFuncAddr == nullptr) {
		DevMsg("IPatch::Init: FAIL: no addr for \"%s\"\n", this->m_pszFuncName);
		return false;
	}
	
	this->m_MaskVerify.SetAll(0xff);
	this->m_MaskPatch.SetAll(0x00);
	
	this->GetVerifyInfo(this->m_BufVerify, this->m_MaskVerify);
	this->m_BufPatch.CopyFrom(this->m_BufVerify);
	this->GetPatchInfo(this->m_BufPatch, this->m_MaskPatch);
	
	return true;
}

bool IPatch::Check()
{
	std::vector<uint32_t> matches;
	for (uint32_t off = this->m_iFuncOffMin; off <= this->m_iFuncOffMax; ++off) {
		if (this->CheckOne(off)) {
			matches.push_back(off);
		}
	}
	
	if (matches.size() != 1) {
		DevMsg("IPatch::Check: FAIL: \"%s\": found %u matching regions\n", this->m_pszFuncName, matches.size());
		return false;
	}
	
	this->m_bFoundOffset = true;
	this->m_iFuncOffActual = matches[0];
	
	return true;
}

bool IPatch::CheckOne(uint32_t off)
{
	uint8_t *ptr = (uint8_t *)((uintptr_t)this->m_pFuncAddr + off);
	for (int i = 0; i < this->m_iLength; ++i) {
		uint8_t *mem = ptr + i;
		
		uint8_t v_byte = this->m_BufVerify[i];
		uint8_t v_mask = this->m_MaskVerify[i];
		
		if ((*mem & v_mask) != (v_byte & v_mask)) {
			return false;
		}
	}
	
	return true;
}


void IPatch::Apply()
{
	if (this->m_bApplied) {
		return;
	}
	
	if (!this->m_bFoundOffset) {
		DevWarning("IPatch::Apply: haven't found actual offset yet!\n");
		return;
	}
	
	DevMsg("IPatch::Apply: TODO: unprotect/protect pages\n");
	
	uint8_t *ptr = (uint8_t *)((uintptr_t)this->m_pFuncAddr + this->m_iFuncOffActual);
	for (int i = 0; i < this->m_iLength; ++i) {
		uint8_t *mem = ptr + i;
		
		this->m_BufRestore[i] = *mem;
		
		uint8_t p_byte = this->m_BufPatch[i];
		uint8_t p_mask = this->m_MaskPatch[i];
		
		*mem &= ~p_mask;
		*mem |= (p_byte & p_mask);
	}
	
	this->m_bApplied = true;
}

void IPatch::UnApply()
{
	if (!this->m_bApplied) {
		return;
	}
	
	if (!this->m_bFoundOffset) {
		DevWarning("IPatch::UnApply: haven't found actual offset yet!\n");
		return;
	}
	
	DevMsg("IPatch::UnApply: TODO: unprotect/protect pages\n");
	
	uint8_t *ptr = (uint8_t *)((uintptr_t)this->m_pFuncAddr + this->m_iFuncOffActual);
	for (int i = 0; i < this->m_iLength; ++i) {
		uint8_t *mem = ptr + i;
		
		uint8_t r_byte = this->m_BufRestore[i];
		uint8_t p_mask = this->m_MaskPatch[i];
		
		*mem &= ~p_mask;
		*mem |= (r_byte & p_mask);
	}
	
	this->m_bApplied = false;
}
