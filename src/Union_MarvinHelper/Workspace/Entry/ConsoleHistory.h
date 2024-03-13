#if ENGINE == Engine_G1 || ENGINE == Engine_G1A

namespace NAMESPACE
{
	struct ConsoleHistory
	{
		std::vector<zSTRING> commands;
		size_t index = 0u;

		void OnEdit()
		{
			index = commands.size();
		}

		void OnExecute(const zSTRING& command)
		{
			index = commands.size();

			if (command.IsEmpty() || !commands.empty() && commands.back() == command)
				return;

			commands.push_back(command);
			index = commands.size();
		}

		zSTRING OnInsert()
		{
			if (commands.empty())
				return "";

			if (index > 0)
				index -= 1;

			return commands[index];
		}
	};

	std::unordered_map<zCConsole*, ConsoleHistory> histories;

	void __stdcall OnEdit(zCConsole* console)
	{
		histories[console].OnEdit();
	}

	void __stdcall OnExecute(zCConsole* console)
	{
		histories[console].OnExecute(console->lastcommand);
	}

	void __stdcall OnInsert(zCConsole* console)
	{
		zSTRING value = histories[console].OnInsert();

		if (!value.IsEmpty())
			console->instr = value;
	}

#define MAKEFUNC(name) \
	CPatchInteger name##_var; \
	name##_var.Init(); \
	name##_var.SetObjectName(#name); \
	name##_var.SetValue(reinterpret_cast<int>(&name)); \
	name##_var.DontRemove();

	CSubscription executePatch(ZSUB(Entry), []
		{
			MAKEFUNC(OnEdit);
			MAKEFUNC(OnExecute);
			MAKEFUNC(OnInsert);

			CPatch::ExecuteResource(CPlugin::GetCurrentPlugin()->GetModule(), MAKEINTRESOURCE(IDR_PATCH1), "PATCH");
		});

#undef MAKEFUNC
}

#endif