#define Engine_G1 1
#define Engine_G1A 2
#define Engine_G2 3
#define Engine_G2A 4

#undef min
#undef max

#define ZENDEF2(g1, g2) ZENDEF(g1, g1, g2, g2)
#define CHECK_THIS_ENGINE ((ENGINE) == Union.GetEngineVersion())
#define ZTEST(exp) (CHECK_THIS_ENGINE ? (exp) : decltype(exp){})
#define ZENFOR(g1, g1a, g2, g2a) ZTEST(ZENDEF(g1, g1a, g2, g2a))

#define LOG(exp) cmd << #exp << ": " << (exp) << endl
#define LOGS(exp) screen->Print(2000, (y += screen->FontY()) %= 8192, Z #exp + Z": " + Z(exp))

#if 1

void Assert(bool result, const char* file, const char* func, int line, const char* expr)
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