#include "mem/patch.h"
#include "mem/scan.h"
#include "mem/protect.h"


bool IPatch::Init()
{
	this->m_pszFuncName = this->GetFuncName();
	this->m_iFuncOffMin = this->GetFuncOffMin();
	this->m_iFuncOffMax = this->GetFuncOffMax();
	
	if (this->Verbose()) {
		DevMsg("IPatch::Init: \"%s\" %s\n", this->m_pszFuncName, typeid(*this).name());
	}
	
	this->m_pFuncAddr = AddrManager::GetAddr(this->m_pszFuncName);
	if (this->m_pFuncAddr == nullptr) {
		DevMsg("IPatch::Init: FAIL: no addr for \"%s\"\n", this->m_pszFuncName);
		return false;
	}
	
	this->m_MaskVerify.SetAll(0xff);
	this->m_MaskPatch.SetAll(0x00);
	
	if (!this->GetVerifyInfo(this->m_BufVerify, this->m_MaskVerify)) {
		DevMsg("IPatch::Init: FAIL: \"%s\": GetVerifyInfo returned false\n", this->m_pszFuncName);
		return false;
	}
	
	this->m_BufPatch.CopyFrom(this->m_BufVerify);
	if (!this->GetPatchInfo(this->m_BufPatch, this->m_MaskPatch)) {
		DevMsg("IPatch::Init: FAIL: \"%s\": GetPatchInfo returned false\n", this->m_pszFuncName);
		return false;
	}
	
	return true;
}

bool IPatch::Check()
{
	using PatchScanner = CMaskedScanner<ScanDir::FORWARD, ScanResults::ALL, 1>;
	
	uintptr_t addr_min = (uintptr_t)this->m_pFuncAddr + this->m_iFuncOffMin;
	uintptr_t addr_max = (uintptr_t)this->m_pFuncAddr + this->m_iFuncOffMax + this->m_iLength + 1;
	
	if (this->Verbose()) {
		DevMsg("IPatch::Check: \"%s\" %s\n", this->m_pszFuncName, typeid(*this).name());
		DevMsg("IPatch::Check: func     %08x\n", (uintptr_t)this->m_pFuncAddr);
		DevMsg("IPatch::Check: off_min      %04x\n", this->m_iFuncOffMin);
		DevMsg("IPatch::Check: off_max      %04x\n", this->m_iFuncOffMax);
		DevMsg("IPatch::Check: addr_min %08x\n", addr_min);
		DevMsg("IPatch::Check: addr_max %08x\n", addr_max);
	}
	
	CScan<PatchScanner> scan(CAddrAddrBounds((void *)addr_min, (void *)addr_max), this->m_BufVerify, this->m_MaskVerify);
	if (!scan.ExactlyOneMatch()) {
		DevMsg("IPatch::Check: FAIL: \"%s\": found %u matching regions\n", this->m_pszFuncName, scan.Matches().size());
		return false;
	}
	
	this->m_bFoundOffset = true;
	this->m_iFuncOffActual = (uintptr_t)scan.FirstMatch() - (uintptr_t)this->m_pFuncAddr;
	
	DevMsg("IPatch::Check: OK: \"%s\": actual offset +0x%04x\n", this->m_pszFuncName, this->m_iFuncOffActual);
	
	return true;
}


void IPatch::Apply()
{
	if (this->Verbose()) {
		DevMsg("IPatch::Apply: \"%s\" %s\n", this->m_pszFuncName, typeid(*this).name());
	}
	
	if (this->m_bApplied) {
		return;
	}
	
	if (!this->m_bFoundOffset) {
		DevWarning("IPatch::Apply: haven't found actual offset yet!\n");
		return;
	}
	
	uint8_t *ptr = (uint8_t *)((uintptr_t)this->m_pFuncAddr + this->m_iFuncOffActual);
	MemUnprotector prot(ptr, this->m_iLength);
	
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
	if (this->Verbose()) {
		DevMsg("IPatch::UnApply: \"%s\" %s\n", this->m_pszFuncName, typeid(*this).name());
	}
	
	if (!this->m_bApplied) {
		return;
	}
	
	if (!this->m_bFoundOffset) {
		DevWarning("IPatch::UnApply: haven't found actual offset yet!\n");
		return;
	}
	
	uint8_t *ptr = (uint8_t *)((uintptr_t)this->m_pFuncAddr + this->m_iFuncOffActual);
	MemUnprotector prot(ptr, this->m_iLength);
	
	for (int i = 0; i < this->m_iLength; ++i) {
		uint8_t *mem = ptr + i;
		
		uint8_t r_byte = this->m_BufRestore[i];
		uint8_t p_mask = this->m_MaskPatch[i];
		
		*mem &= ~p_mask;
		*mem |= (r_byte & p_mask);
	}
	
	this->m_bApplied = false;
}


uint32_t IPatch::GetActualOffset() const
{
	if (!this->m_bFoundOffset) return -1;
	return this->m_iFuncOffActual;
}

void *IPatch::GetActualLocation() const
{
	if (!this->m_bFoundOffset) return nullptr;
	return (void *)((uintptr_t)this->m_pFuncAddr + this->m_iFuncOffActual);
}
