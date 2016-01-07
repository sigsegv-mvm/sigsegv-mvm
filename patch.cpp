#include "patch.h"
#include "extension.h"


ByteBuf::ByteBuf(int size) :
	m_iSize(size)
{
	this->m_Buf = new uint8_t[size];
}

ByteBuf::~ByteBuf()
{
	delete[] this->m_Buf;
}


bool IPatch::Init(char *error, size_t maxlen)
{
	this->m_pszFuncName = this->GetFuncName();
	this->m_iOffset = this->GetOffset();
	
	if (!g_pGameConf->GetMemSig(this->m_pszFuncName, &this->m_pFuncAddr) || this->m_pFuncAddr == nullptr) {
		snprintf(error, maxlen, "Patch error: signature lookup failed for %s", this->m_pszFuncName);
		return false;
	}
	
	this->m_MaskVerify.SetAll(0xff);
	this->m_MaskPatch.SetAll(0x00);
	
	this->GetVerifyInfo(this->m_BufVerify, this->m_MaskVerify);
	this->m_BufPatch.CopyFrom(this->m_BufVerify);
	this->GetPatchInfo(this->m_BufPatch, this->m_MaskPatch);
	
	return true;
}

bool IPatch::Check(char *error, size_t maxlen)
{
	uint8_t *ptr = (uint8_t *)((uintptr_t)this->m_pFuncAddr + this->m_iOffset);
	for (int i = 0; i < this->m_iLength; ++i) {
		uint8_t *mem = ptr + i;
		
		uint8_t v_byte = this->m_BufVerify[i];
		uint8_t v_mask = this->m_MaskVerify[i];
		
		if ((*mem & v_mask) != (v_byte & v_mask)) {
			snprintf(error, maxlen, "Patch/verify failure: func %s, offset 0x%x, byte 0x%x: < byte:%02x mask:%02x | mem:%02x >",
				this->m_pszFuncName, this->m_iOffset, i, v_byte, v_mask, *mem);
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
	
	uint8_t *ptr = (uint8_t *)((uintptr_t)this->m_pFuncAddr + this->m_iOffset);
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
	
	uint8_t *ptr = (uint8_t *)((uintptr_t)this->m_pFuncAddr + this->m_iOffset);
	for (int i = 0; i < this->m_iLength; ++i) {
		uint8_t *mem = ptr + i;
		
		uint8_t r_byte = this->m_BufRestore[i];
		uint8_t p_mask = this->m_MaskPatch[i];
		
		*mem &= ~p_mask;
		*mem |= (r_byte & p_mask);
	}
	
	this->m_bApplied = false;
}
