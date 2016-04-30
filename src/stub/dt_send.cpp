//#include "stub/dt_send.h"
#include "link/link.h"


static StaticFuncThunk<void, const SendProp *, const void *, const void *, DVariant *, int, int> ft_SendProxy_UInt8ToInt32("SendProxy_UInt8ToInt32");
void SendProxy_UInt8ToInt32(const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID)
{
	ft_SendProxy_UInt8ToInt32(pProp, pStruct, pData, pOut, iElement, objectID);
}
