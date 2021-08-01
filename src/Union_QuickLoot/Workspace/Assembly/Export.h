extern "C"
{
	__declspec(dllexport) void __cdecl PrintItemName(void* pItem)
	{
		switch (Union.GetEngineVersion())
		{

#ifdef __G1
		case Engine_G1: return Gothic_I_Classic::PrintItemName(pItem);
#endif

#ifdef __G1A
		case Engine_G1A: return Gothic_I_Addon::PrintItemName(pItem);
#endif

#ifdef __G2
		case Engine_G2: return Gothic_II_Classic::PrintItemName(pItem);
#endif

#ifdef __G2A
		case Engine_G2A: return Gothic_II_Addon::PrintItemName(pItem);
#endif

		}
	}
}