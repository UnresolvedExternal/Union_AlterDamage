#define Engine_G1 1
#define Engine_G1A 2
#define Engine_G2 3
#define Engine_G2A 4

#define CHECK_THIS_ENGINE (Union.GetEngineVersion() == (engine))
#define CHECK_THIS_ENGINE ((ENGINE) == Union.GetEngineVersion())