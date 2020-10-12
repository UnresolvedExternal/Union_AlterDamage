namespace NAMESPACE
{
	struct TGlobals
	{
		TParsedCommand parsedCommand;
		std::vector<std::unique_ptr<CCommandBase>> commands;
		std::unique_ptr<CGridView> gridView;
		std::vector<std::unique_ptr<CStringConvertible>> infos;
		
		string handledInput;
		bool consoleWasOpen;

		TGlobals() :
			consoleWasOpen(false)
		{

		}
	} globals;

	namespace Helpers
	{
		bool IsLessSymbolName(const string& x, const string& y)
		{
			bool xLocale = x.HasWord(".");
			bool yLocale = y.HasWord(".");

			if (xLocale == yLocale)
				return x < y;

			return xLocale < yLocale;
		}

		bool IsInConsole()
		{
			for (zCConsole* con : zCConsole::active_consoles)
				if (COA3(con, conview, ondesk))
					return true;
			return false;
		}

		bool IsVar(zCPar_Symbol* symbol)
		{
			static int c_info = parser->GetIndex("C_INFO");

			if (!symbol || !symbol->name.Length())
				return false;

			switch (symbol->type)
			{
			case zPAR_TYPE_INT:
			case zPAR_TYPE_FLOAT:
			case zPAR_TYPE_STRING:
				return !symbol->HasFlag(zPAR_FLAG_CLASSVAR);

			case zPAR_TYPE_INSTANCE:
			case zPAR_TYPE_PROTOTYPE:
				return c_info >= 0 && parser->GetBaseClass(symbol) == c_info && symbol->HasFlag(zPAR_FLAG_CONST);

			default:
				return false;
			}
		}

		int GetValuesCount(zCPar_Symbol* symbol)
		{
			switch (symbol->type)
			{
			case zPAR_TYPE_INT:
			case zPAR_TYPE_FLOAT:
			case zPAR_TYPE_STRING:
				return symbol->ele;

			default:
				return 1;
			}
		}

		void AddParallel(Array<string>& hints, const Array<string>& values, const string& substring)
		{
			int concurrency = 4;
			
			std::vector<std::pair<int, int>> partition(concurrency);
			for (int i = 0; i < concurrency; i++)
			{
				partition[i].first = i * ((int)values.GetNum() / concurrency);
				partition[i].second = partition[i].first + (int)values.GetNum() / concurrency;
			}
			partition[concurrency - 1].second = values.GetNum();

			std::vector<std::vector<string>> results(concurrency);
			std::vector<std::future<void>> futures;

			for (int i = 0; i < concurrency; i++)
				futures.push_back(std::async(
					[&](int index)
					{
						for (int i = partition[index].first; i < partition[index].second; i++)
							if (HasWord(values[i], substring))
								results[index].push_back(values[i]);
					}, i));

			for (int i = 0; i < concurrency; i++)
			{
				futures[i].get();
				for (const string& hint : results[i])
					hints.InsertEnd(hint);
			}
		}

		void AddVars(Array<string>& hints, const string& word)
		{
			static bool initialized = false;
			static Array<string> vars;

			if (!initialized)
			{
				initialized = true;

				for (int i = 0; i < parser->symtab.tablesort.GetNum(); i++)
				{
					int index = parser->symtab.tablesort[i];
					if (IsVar(parser->symtab.table[index]))
						vars.InsertEnd(parser->symtab.table[index]->name).Lower();
				}

				//for (zCPar_Symbol* symbol : parser->symtab.table)
				//	if (IsVar(symbol))
				//		vars.InsertEnd(symbol->name).Lower();

				//std::sort(vars.begin(), vars.end(), &Helpers::IsLessSymbolName);
			}

			for (const string& var : vars)
				if (HasWord(var, word))
					hints.InsertEnd(var);
		}

		class CVobsTraverser : public zCVobCallback
		{
		public:
			std::function<void(zCVob*)> func;

			virtual void HandleVob(zCVob* vob, void*) override
			{
				func(vob);
			}
		};

		void AddWaypoints(Array<string>& hints, const string& word)
		{
			static bool initialized = false;
			static Array<string> waypoints;
			static CSubscription deinit(TGameEvent::LoadBegin, []() { initialized = false; waypoints.Clear(); });

			if (!initialized)
			{
				initialized = true;

				for (zCWaypoint* wp : ogame->GetGameWorld()->wayNet->wplist)
					if (wp->name.Length())
						waypoints.InsertEnd(wp->name).Lower();

				CVobsTraverser traverser;
				traverser.func = [&](zCVob* vob)
				{
					if (!vob)
						return;

					zCVobSpot* fp = COA2(vob, CastTo<zCVobSpot>());
					if (fp && fp->objectName.Length())
						waypoints.InsertEnd(fp->objectName).Lower();
				};

				ogame->GetGameWorld()->TraverseVobTree(traverser, nullptr, nullptr);

				std::sort(waypoints.begin(), waypoints.end());
				uint size = std::unique(waypoints.begin(), waypoints.end()) - waypoints.begin();
				waypoints.DeleteAtBounds(size, waypoints.GetNum() - size);
			}
			
			for (const string& wp : waypoints)
				if (HasWord(wp, word))
					hints.InsertEnd(wp);
		}

		class CScriptVar : public CStringConvertible
		{
		public:
			string text;
			zCPar_Symbol* symbol;
			int index;
			int offset;
			int count;
			
			CScriptVar(string text) :
				symbol(nullptr),
				index(-1),
				offset(0),
				count(1)
			{
				text.Lower();
				this->text = text;
				std::regex exp("^((\\w|\\.)+)(\\((\\d{1,9}(-(\\d{1,9}))?)\\))?$");
				std::vector<string> tokens;

				for (auto it = std::cregex_iterator(text.GetVector(), text.GetVector() + text.Length(), exp); it != std::cregex_iterator(); it++)
				{
					for (auto sit = it->begin(); sit != it->end(); sit++)
						tokens.push_back(sit->str().c_str());
				}

				if (!tokens.size())
				{
					this->text += ": (syntax error)";
					return;
				}

				string name = tokens[1];
				name.Upper();
				string offset = tokens[3];
				string offsetMax = tokens[5];

				index = name.IsNumber() ? name.ToInt32() : parser->GetIndex(name);
				symbol = parser->GetSymbolInfo(index);

				if (!symbol)
				{
					this->text += ": (not found)";
					return;
				}

				if (offset.Length())
					this->offset = offset.ToInt32();

				if (this->offset >= GetValuesCount(symbol))
				{
					symbol = nullptr;
					this->text += ": (offset out of range)";
					return;
				}

				if (offsetMax.Length())
					this->count = offsetMax.ToInt32() - this->offset + 1;
				else
					if (!this->offset)
						this->count = GetValuesCount(symbol);

				if (this->count <= 0 || this->offset + this->count > GetValuesCount(symbol))
				{
					symbol = nullptr;
					this->text += ": (offset + count out of range)";
					return;
				}

				if (!IsVar(symbol))
				{
					symbol = nullptr;
					this->text += ": (invalid symbol type)";
					return;
				}
			}

			bool TrySetValue(const string& value)
			{
				if (!symbol)
					return false;

				oCInfo* info;

				switch (symbol->type)
				{
				case zPAR_TYPE_INT:
					symbol->SetValue((int)value.ToInt32(), offset);
					return true;

				case zPAR_TYPE_FLOAT:
					symbol->SetValue(value.ToReal32(), offset);
					return true;

				case zPAR_TYPE_STRING:
					if (value.Length() < 2 || value.First() != '"' || value.Last() != '"')
						return false;
					symbol->SetValue(zSTRING(value.Copy(1, value.Length() - 2)), offset);
					return true;

				case zPAR_TYPE_INSTANCE:
				case zPAR_TYPE_PROTOTYPE:
					info = COA3(ogame, GetInfoManager(), GetInformation(index));
					if (!info)
						return false;
					info->SetTold(value.ToInt32());
					return true;

				default:
					return false;
				}
			}

			virtual string ToString() override
			{
				if (!symbol)
					return text;

				string result = text;
				result += ": ";

				if (count > 1)
					result += "[";

				for (int i = 0; i < count; i++)
				{
					if (i)
						result += " ";

					int intVal;
					float floatVal;
					zSTRING stringVal;
					oCInfo* infoVal;

					switch (symbol->type)
					{
					case zPAR_TYPE_INT:
						symbol->GetValue(intVal, offset + i);
						result += intVal;
						break;

					case zPAR_TYPE_FLOAT:
						symbol->GetValue(floatVal, offset + i);
						result += floatVal;
						break;

					case zPAR_TYPE_STRING:
						symbol->GetValue(stringVal, offset + i);
						result += "\"" + string(stringVal) + "\"";
						break;

					case zPAR_TYPE_INSTANCE:
					case zPAR_TYPE_PROTOTYPE:
						infoVal = COA3(ogame, GetInfoManager(), GetInformation(index));
						if (infoVal)
							result += infoVal->told;
						else
							result += "ERROR";
						break;

					default:
						result += "ERROR";
					}
				}

				if (count > 1)
					result += "]";

				return result;
			}
		};
	}

	class CConsoleCommand : public CCommandBase
	{
	public:
		CConsoleCommand(const string& prefix, const string& description = A"") :
			CCommandBase(prefix, &globals.parsedCommand)
		{
			if (this->prefix.GetNum())
				zcon->Register(GetPrefix(), description);
		}
	};

	class CConDatCommand : public CConsoleCommand
	{
	public:
		CConDatCommand() : CConsoleCommand("", "")
		{

		}

		virtual void AddHints(Array<string>& hints) override
		{
			int num = parsedCommand->GetWordNumTyping();
			if (num < 0)
				return;

			Array<string> newHints;

			for (zCConDat* con = zcon->list.root; con; con = con->next)
			{
				Array<string> tokens = SearchByRegex(con->name, "\\w+");
				for (string& token : tokens)
					token.Lower();

				if (num >= (int)tokens.GetNum())
					continue;

				bool matched = true;
				for (int i = 0; i < num && matched; i++)
					if (!parsedCommand->tokens[i].CompareI(tokens[i]))
						matched = false;

				if (matched && HasWord(tokens[num], parsedCommand->GetWordTyping()))
					newHints.InsertSorted(tokens[num]);
			}

			auto uniqueEnd = std::unique(newHints.begin(), newHints.end());
			for (auto it = newHints.begin(); it != uniqueEnd; it++)
				hints.InsertEnd(*it);
		}
	};

	class CShowVarsCommand : public CConsoleCommand
	{
	public:
		CShowVarsCommand() :
			CConsoleCommand("show vars", "show script variable values")
		{

		}

		virtual void AddHints(Array<string>& hints) override
		{
			if (!IsPrefixMatched())
				return;

			Helpers::AddVars(hints, parsedCommand->GetWordTyping());
		}

		virtual bool TryExecute(string& output)
		{
			output = "";

			if (!IsPrefixMatched())
				return false;

			for (int i = (int)prefix.GetNum(); i < (int)parsedCommand->tokens.GetNum(); i++)
				globals.infos.push_back(std::make_unique<Helpers::CScriptVar>(parsedCommand->tokens[i]));

			output = "Ok";
			return true;
		}
	};

	class CGotoWaypointCommand : public CConsoleCommand
	{
	public:
		CGotoWaypointCommand() :
			CConsoleCommand("goto waypoint")
		{

		}

		virtual void AddHints(Array<string>& hints) override
		{
			if (!IsPrefixMatched() || parsedCommand->GetWordNumTyping() != (int)prefix.GetNum())
				return;

			Helpers::AddWaypoints(hints, parsedCommand->GetWordTyping());
		}
	};

	class CGotoVobCommand : public CConsoleCommand
	{
	public:
		CGotoVobCommand() : 
			CConsoleCommand("goto vob")
		{

		}

		virtual void AddHints(Array<string>& hints) override
		{
			if (!IsPrefixMatched() || parsedCommand->GetWordNumTyping() != (int)prefix.GetNum())
				return;

			Array<string> vobs;
			Helpers::CVobsTraverser traverser;
			traverser.func = [&](zCVob* vob)
			{
				if (vob->objectName.Length())
					vobs.InsertEnd(vob->objectName).Lower();
			};
			ogame->GetGameWorld()->TraverseVobTree(traverser, nullptr, nullptr);
			ogame->GetGameWorld()->TraverseVobList(traverser, nullptr);

			std::sort(vobs.begin(), vobs.end());
			uint count = std::unique(vobs.begin(), vobs.end()) - vobs.begin();
			vobs.DeleteAtBounds(count, vobs.GetNum() - count);

			for (const string& vob : vobs)
				if (HasWord(vob, parsedCommand->GetWordTyping()))
					hints.InsertEnd(vob);
		}
	};

	class CHideVarsCommand : public CConsoleCommand
	{
	public:
		CHideVarsCommand() :
			CConsoleCommand("hide vars")
		{

		}

		virtual void AddHints(Array<string>& hints) override
		{
			if (!IsPrefixMatched() || parsedCommand->GetWordNumTyping() < (int)prefix.GetNum())
				return;

			for (int i = 1; i <= (int)globals.infos.size(); i++)
			{
				string str = i;
				if (HasWord(str, parsedCommand->GetWordTyping()))
					hints.InsertEnd(str);
			}
		}

		virtual bool TryExecute(string& output) override
		{
			if (!IsPrefixMatched())
				return false;

			Array<int> nums;
			for (int i = (int)prefix.GetNum(); i < (int)parsedCommand->tokens.GetNum(); i++)
			{
				const string& token = parsedCommand->tokens[i];
				if (token.IsNumber())
					nums.InsertEnd(token.ToInt32());
			}

			if (!nums.GetNum())
				globals.infos.clear();
			else
			{
				std::sort(nums.begin(), nums.end(), [](const int& x, const int& y) { return x >= y; });
				auto end = std::unique(nums.begin(), nums.end());
				std::vector<int> validNums;
				std::copy_if(nums.begin(), end, std::back_inserter(validNums), [](const int& x) { return x >= 1 && x <= (int)globals.infos.size(); });

				for (const int& num : validNums)
					globals.infos.erase(globals.infos.begin() + (num - 1));
			}

			output = "Ok";
			return true;
		}
	};

	class CSetVarCommand : public CConsoleCommand
	{
	public:
		CSetVarCommand() :
			CConsoleCommand("set var")
		{

		}

		virtual void AddHints(Array<string>& hints) override
		{
			if (!IsPrefixMatched() || parsedCommand->GetWordNumTyping() != (int)prefix.GetNum())
				return;

			Helpers::AddVars(hints, parsedCommand->GetWordTyping());
		}

		virtual bool TryExecute(string& output) override
		{
			if (!IsPrefixMatched())
				return false;

			if (parsedCommand->tokens.GetNum() != prefix.GetNum() + 2)
			{
				output = "Wrong arguments number";
				return true;
			}

			Helpers::CScriptVar var(parsedCommand->tokens[prefix.GetNum()]);
			if (!var.symbol)
			{
				output = var.text;
				return true;
			}

			if (var.count != 1)
			{
				output = "Arrays not supported";
				return true;
			}

			const string& value = parsedCommand->tokens[prefix.GetNum() + 1];
			if (!var.TrySetValue(value))
			{
				output = "set value failed";
				return true;
			}

			output = "Ok.";
			return true;
		}
	};

	class CInsertCommand : public CConsoleCommand
	{
	public:
		CInsertCommand() :
			CConsoleCommand("insert")
		{

		}

		virtual void AddHints(Array<string>& hints) override
		{
			if (!IsPrefixMatched() || parsedCommand->GetWordNumTyping() != (int)prefix.GetNum())
				return;

			static bool initialized = false;
			static Array<string> names;
			static const int c_item = parser->GetIndex("C_ITEM");
			static const int c_npc = parser->GetIndex("C_NPC");

			if (!initialized)
			{
				initialized = true;

				for (zCPar_Symbol* sym : parser->symtab.table)
				{
					if (sym->type != zPAR_TYPE_INSTANCE && sym->type != zPAR_TYPE_PROTOTYPE)
						continue;

					if (!sym->HasFlag(zPAR_FLAG_CONST))
						continue;

					if (!sym->name.Length() || HasWord(sym->name, "."))
						continue;

					int baseClass = parser->GetBaseClass(sym);
					if (baseClass != c_item && baseClass != c_npc)
						continue;

					names.InsertEnd(sym->name).Lower();
				}

				std::sort(names.begin(), names.end());
			}

			for (const string& name : names)
				if (HasWord(name, parsedCommand->GetWordTyping()))
					hints.InsertEnd(name);
		}
	};

	int ConsoleEvalFunc(const zSTRING& inpstr, zSTRING& msg)
	{
		globals.parsedCommand.Parse(inpstr);
		
		string output;
		for (std::unique_ptr<CCommandBase>& command : globals.commands)
			if (command->TryExecute(output))
			{
				msg = output.GetVector();
				return true;
			}

		return false;
	}

	void __cdecl Hook__Game_InitConsole();
	CInvoke<void(__cdecl*)()> Ivk__Game_InitConsole(ZenDef<TInstance>(0x00645280, 0x0066CBA0, 0x00673470, 0x006D01F0), &Hook__Game_InitConsole, IvkEnabled(ENGINE));
	void __cdecl Hook__Game_InitConsole()
	{
		globals.commands.push_back(std::make_unique<CConDatCommand>());
		globals.commands.push_back(std::make_unique<CShowVarsCommand>());
		globals.commands.push_back(std::make_unique<CHideVarsCommand>());
		globals.commands.push_back(std::make_unique<CGotoWaypointCommand>());
		globals.commands.push_back(std::make_unique<CGotoVobCommand>());
		globals.commands.push_back(std::make_unique<CSetVarCommand>());
		globals.commands.push_back(std::make_unique<CInsertCommand>());

		zcon->AddEvalFunc(&ConsoleEvalFunc);
		Ivk__Game_InitConsole();
	}

	CSubscription freezePlayer(TGameEvent::Loop, []()
		{
			if (!player)
				return;

			player->SetSleeping(Helpers::IsInConsole());
		}, CHECK_THIS_ENGINE);

	CSubscription checkCommands(TGameEvent::Loop, []()
		{
			static Timer keyTimer;

			if (!globals.gridView)
			{
				globals.gridView = std::make_unique<CGridView>(0, 0, 8192, 8192 - 400);
				screen->InsertItem(globals.gridView.get());
			}

			if (ogame->singleStep)
			{
				globals.consoleWasOpen = false;
				globals.gridView->Build(Array<string>());
			}

			if (!zcon || !zcon->IsVisible())
			{
				globals.gridView->SetPos(0, 0);
				globals.gridView->SetSize(8192, 8192 - 400);
				globals.consoleWasOpen = false;

				if (!Helpers::IsInConsole())
				{
					Array<string> entries;
					for (int i = 0; i < (int)globals.infos.size(); i++)
						entries.InsertEnd(string(i + 1) + ". " + globals.infos[i]->ToString());
					globals.gridView->selection.index = -1;
					globals.gridView->Build(entries);
				}

				return;
			}

			if (!globals.consoleWasOpen)
			{
				globals.gridView->SetPos(0, zcon->conview->vposy + zcon->conview->vsizey);
				globals.gridView->SetSize(8192, 8192 - globals.gridView->vposy - 400);
			}

			if (!globals.consoleWasOpen || globals.handledInput != zcon->instr.ToChar())
			{
				globals.parsedCommand.Parse(zcon->instr.ToChar());

				Array<string> hints;
				for (const auto& command : globals.commands)
					command->AddHints(hints);

				globals.gridView->selection.index = 0;
				globals.gridView->Build(hints);
			}

			globals.consoleWasOpen = true;
			globals.handledInput = zcon->instr.ToChar();

			if (globals.gridView->selection.index != -1 && globals.gridView->entries.GetNum())
			{
				if (globals.gridView->selection.index < (int)globals.gridView->entries.GetNum() - 1 && zKeyPressed(KEY_DOWN) && keyTimer[__LINE__].Await(50, true))
				{
					globals.gridView->selection.index += 1; 
					globals.gridView->Update();
					return;
				}
				else if (globals.gridView->selection.index > 0 && zKeyPressed(KEY_UP) && keyTimer[__LINE__].Await(50, true))
				{
					globals.gridView->selection.index -= 1;
					globals.gridView->Update();
					return;
				}
				else if (zKeyToggled(KEY_RIGHT))
				{
					zcon->instr = "";
					for (int i = 0; i < globals.parsedCommand.GetWordNumTyping(); i++)
					{
						if (i)
							zcon->instr += " ";
						zcon->instr += globals.parsedCommand.tokens[i].GetVector();
					}

					if (zcon->instr.Length())
						zcon->instr += " ";

					zcon->instr += globals.gridView->entries[globals.gridView->selection.index].GetVector();
					zcon->instr += " ";

					zcon->ShowInput();
					return;
				}
			}

			if (zKeyToggled(KEY_LEFT))
			{
				zcon->instr = "";
				for (int i = 0; i < (int)globals.parsedCommand.tokens.GetNum() - 1; i++)
				{
					if (i)
						zcon->instr += " ";
					zcon->instr += globals.parsedCommand.tokens[i].GetVector();
				}

				if (zcon->instr.Length())
					zcon->instr += " ";

				zcon->ShowInput();
			}
		}, CHECK_THIS_ENGINE
	);

	CInvoke<void(__cdecl*)()> Ivk__Game_OpenConsole(ZenDef<TInstance>(0x00647120, 0x0066EDB0, 0x00675310, 0x006D2090), nullptr, IVK_DISABLED);

	CSubscription test(TGameEvent::Loop, []()
		{
			if (zKeyPressed(KEY_LSHIFT) && zKeyPressed(KEY_F2))
			{
				zinput->ClearKeyBuffer();
				Ivk__Game_OpenConsole();
			}
		}, CHECK_THIS_ENGINE);
}