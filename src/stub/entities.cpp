#include "stub/entities.h"


IMPL_DATAMAP(int, CSpellPickup, m_nTier);


IMPL_SENDPROP(CHandle<CBaseEntity>, CTFReviveMarker, m_hOwner,   CTFReviveMarker);
IMPL_SENDPROP(short,                CTFReviveMarker, m_nRevives, CTFReviveMarker);
