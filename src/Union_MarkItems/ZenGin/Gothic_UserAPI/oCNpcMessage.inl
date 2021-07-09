// Supported with union (c) 2020 Union team

// User API for oCNpcMessage
// Add your methods here

inline void Execute(oCNpc* target, zCVob* sourceVob = nullptr)
{
	target->GetEM()->OnMessage(this, sourceVob);
}