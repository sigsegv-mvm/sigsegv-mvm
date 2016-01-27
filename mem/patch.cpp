#include "mem/patch.h"
#include "mem/scan.h"


bool IPatch::Init()
{
	this->m_pszFuncName = this->GetFuncName();
	this->m_iFuncOffMin = this->GetFuncOffMin();
	this->m_iFuncOffMax = this->GetFuncOffMax();
	
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
	using PatchScanner = CMaskedScanner<ScanDir::FORWARD, ScanResults::ALL, 1>;
	
	uintptr_t addr_min = (uintptr_t)this->m_pFuncAddr + this->m_iFuncOffMin;
	uintptr_t addr_max = (uintptr_t)this->m_pFuncAddr + this->m_iFuncOffMax + this->m_iLength;
	
	CScan<PatchScanner> scan(CAddrAddrBounds((void *)addr_min, (void *)addr_max), this->m_BufVerify, this->m_MaskVerify);
	if (scan.Matches().size() != 1) {
		DevMsg("IPatch::Check: FAIL: \"%s\": found %u matching regions\n", this->m_pszFuncName, scan.Matches().size());
		return false;
	}
	
	this->m_bFoundOffset = true;
	this->m_iFuncOffActual = (uintptr_t)scan.Matches()[0] - (uintptr_t)this->m_pFuncAddr;
	
	DevMsg("IPatch::Check: OK: \"%s\": actual offset +0x%04x\n", this->m_pszFuncName, this->m_iFuncOffActual);
	
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
