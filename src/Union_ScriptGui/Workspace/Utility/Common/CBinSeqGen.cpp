#include "CBinSeqGen.h"
#include <cmath>

MEMPOOL_DEF(CBinSeqGen)

bool CBinSeqGen::Roll(float chance)
{
	return rand() / (float)RAND_MAX < chance;
}

CBinSeqGen::CBinSeqGen(float factor, float deviance) :
	factor(factor),
	deviance(deviance)
{
}

CBinSeqGen::CBinSeqGen(const CBinSeqGen& gen) :
	deviance(gen.deviance),
	factor(gen.factor)
{
}

bool CBinSeqGen::GenNext(float chance)
{
	float devSuccess = deviance + chance - 1;
	float devFail = deviance + chance;
	float devDiff = abs(devFail) - abs(devSuccess);
	float realChance = chance + devDiff * factor;
	bool success = Roll(realChance);
	deviance = success ? devSuccess : devFail;
	return success;
}

float CBinSeqGen::GetDeviance() const
{
	return deviance;
}
