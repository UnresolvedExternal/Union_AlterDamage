extern "C"
{
	__declspec(dllexport) void PrintItemName(void* pItem)
	{
		switch (Union.GetEngineVersion())
		{
		case Engine_G1: return Gothic_I_Classic::PrintItemName(pItem);
		case Engine_G1A: return Gothic_I_Addon::PrintItemName(pItem);
		case Engine_G2: return Gothic_II_Classic::PrintItemName(pItem);
		case Engine_G2A: return Gothic_II_Addon::PrintItemName(pItem);
		}
	}
}