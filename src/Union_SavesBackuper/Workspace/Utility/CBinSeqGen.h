#pragma once

#include "Workspace\Memory\CMemPool.h"

class CBinSeqGen
{
	MEMPOOL_DECL(CBinSeqGen)

private:
	float deviance;
	float factor;

	bool Roll(float chance);

public:
	CBinSeqGen(float factor, float deviance = 0);
	CBinSeqGen(const CBinSeqGen& gen);
	bool GenNext(float chance);
	float GetDeviance() const;
};