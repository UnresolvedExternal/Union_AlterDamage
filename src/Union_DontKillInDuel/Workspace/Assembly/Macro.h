#define Engine_G1 1
#define Engine_G1A 2
#define Engine_G2 3
#define Engine_G2A 4

#define CHECK_THIS_ENGINE ((ENGINE) == Union.GetEngineVersion())
#define PLUGIN_NAME "Union_DontKillInDuel"

#if 1
inline void Assert(bool result, const char* file, const char* func, int line, const char* expr)
{
	if (result)
		return;

	string message;
	message += A"File: " + file + "\n";
	message += A"Func: " + func + "\n";
	message += A"Line: " + line + "\n";
	message += A"Expr: " + expr;

	Message::Error(message, "Assertion Failed");
}

#define ASSERT(expr) Assert((expr), __FILE__, __FUNCTION__, __LINE__, #expr)

#else

#define ASSERT(exp) ((void)0)

#endif