#include "mem/patch.h"
#include "extension.h"


bool IPatch::Init()
{
	this->m_pszFuncName = this->GetFuncName();
	this->m_iFuncOffset = this->GetFuncOffset();
	
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
	uint8_t *ptr = (uint8_t *)((uintptr_t)this->m_pFuncAddr + this->m_iFuncOffset);
	for (int i = 0; i < this->m_iLength; ++i) {
		uint8_t *mem = ptr + i;
		
		uint8_t v_byte = this->m_BufVerify[i];
		uint8_t v_mask = this->m_MaskVerify[i];
		
		if ((*mem & v_mask) != (v_byte & v_mask)) {
			DevMsg("IPatch::Check: FAIL: func \"%s\", off 0x%x, byte 0x%x: < byte:%02x mask:%02x | mem:%02x >\n",
				this->m_pszFuncName, this->m_iFuncOffset, i, v_byte, v_mask, *mem);
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
	
	DevMsg("IPatch::Apply: TODO: unprotect/protect pages\n");
	
	uint8_t *ptr = (uint8_t *)((uintptr_t)this->m_pFuncAddr + this->m_iFuncOffset);
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
	
	DevMsg("IPatch::UnApply: TODO: unprotect/protect pages\n");
	
	uint8_t *ptr = (uint8_t *)((uintptr_t)this->m_pFuncAddr + this->m_iFuncOffset);
	for (int i = 0; i < this->m_iLength; ++i) {
		uint8_t *mem = ptr + i;
		
		uint8_t r_byte = this->m_BufRestore[i];
		uint8_t p_mask = this->m_MaskPatch[i];
		
		*mem &= ~p_mask;
		*mem |= (r_byte & p_mask);
	}
	
	this->m_bApplied = false;
}
