#include "UnionAfx.h"
#include "CKarma.h"

CKarma::CKarma() :
	karma(0)
{

}

bool CKarma::RollSuccess(float chance)
{
	chance = min(1, chance);
	chance = max(0, chance);
	karma += chance;

	if (karma >= 1)
	{
		karma -= 1;
		return true;
	}

	return false;
}