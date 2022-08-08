namespace NAMESPACE
{
	class CShowListEntry : public CStringConvertible
	{
	public:
		virtual string GetCommandText() const = 0;
	};

	class CSimpleEntry : public CShowListEntry
	{
	private:
		std::unique_ptr<CStringConvertible> serializable;
		string commandText;

	public:
		CSimpleEntry(std::unique_ptr<CStringConvertible>&& serializable, const string& commandText) :
			serializable(std::move(serializable)),
			commandText(commandText)
		{

		}

		virtual string ToString() const override
		{
			return serializable->ToString();
		}

		virtual string GetCommandText() const override
		{
			return commandText;
		}
	};

	class CConsoleContext
	{
	private:
		enum class TState
		{
			NoConsole,
			NoConsoleShowList,
			MainConsole,
			MainConsoleShowHints,
			AuxConsole
		};

		TState state;
		string input;

		std::vector<CSubscription> subs;

		TParsedCommand command;
		std::vector<std::unique_ptr<CCommandBase>> commands;
		std::vector<std::unique_ptr<CShowListEntry>> showList;
		CGridView* gridView;

		std::unique_ptr<CSymbolCache> symbolHelperCache;
		std::unique_ptr<CFpsCounter> fpsCounter;

		static bool IsInAuxConsole()
		{
			return zCConsole::cur_console && zCConsole::cur_console != zcon;
		}

		static bool IsInMainConsole()
		{
			return zCConsole::cur_console && zCConsole::cur_console == zcon;
		}

		static bool IsInConsole()
		{
			return zCConsole::cur_console;
		}

		static bool IsInFreeLook()
		{
			return *(int*)ZENDEF(0x00869D50, 0x008AF144, 0x008BFDDC, 0x008CE42C);
		}

		void Print(const Array<string>& entities)
		{
			if (!screen)
				return;

			if (!gridView->ondesk)
				screen->InsertItem(gridView);
			gridView->Draw(entities);
		}

		bool TryToggleFreezePlayer()
		{
			if (!Settings::ConsoleFreezePlayer || !player)
				return false;

			if (IsInFreeLook())
			{
				player->SetSleeping(true);
				return true;
			}

			switch (state)
			{
			case TState::NoConsole:
			case TState::NoConsoleShowList:
				player->SetSleeping(false);
				return true;

			case TState::MainConsole:
			case TState::MainConsoleShowHints:
			case TState::AuxConsole:
				player->SetSleeping(true);
				return true;

			default:
				throw std::exception(A __FILE__ + A" " + A __LINE__);
			}
		}

		bool TryShowList()
		{
			if (IsInConsole() || ogame->singleStep)
				return false;

			size_t i = 0;
			Array<string> entries;
			for (const auto& entity : showList)
				entries.InsertEnd(A (++i) + ". " + entity->ToString());

			gridView->SetPos(0, 400);
			gridView->SetSize(8192, 8192 - 400);
			gridView->selection.index = -1;
			Print(entries);
			return true;
		}

		bool TryParseCommand()
		{
			string newInput = A zcon->instr;
			if (newInput != input)
			{
				input = newInput;
				command.Parse(input);
				return true;
			}

			return false;
		}

		bool TryShowHints(bool commandParsed)
		{
			const bool outdated = commandParsed || state != TState::MainConsoleShowHints;

			if (!IsInMainConsole() || !Settings::ConsoleShowHints || ogame->singleStep)
				return false;

			gridView->SetPos(0, zcon->conview->vposy + zcon->conview->vsizey);
			gridView->SetSize(8192, 7900 - gridView->vposy);

			if (outdated)
			{
				Array<string> allHints;

				for (const auto& command : commands)
				{
					std::vector<string> hints = command->GetHints();
					for (const string& hint : hints)
						allHints.InsertEnd(hint);
				}

				gridView->selection.index = 0;
				Print(allHints);
			}
			else
				gridView->Redraw();

			return true;
		}

		bool TryManipulateSelection()
		{
			if (!gridView->CheckSelection())
				return false;

			if (state != TState::MainConsoleShowHints)
				return false;

			if (Settings::KeySelectionInsert->GetToggled())
			{
				Settings::KeySelectionPrev->ResetTimer();
				Settings::KeySelectionNext->ResetTimer();

				zcon->instr = "";
				for (int i = 0; i < GetCommand().GetWordNumTyping(); i++)
				{
					if (i)
						zcon->instr += " ";
					zcon->instr += GetCommand().tokens[i].GetVector();
				}

				if (zcon->instr.Length())
					zcon->instr += " ";

				zcon->instr += gridView->GetSelected().GetVector();
				zcon->instr += " ";

#if ENGINE == Engine_G1 || ENGINE == Engine_G1A
				OnEdit(zcon);
#else
				zcon->lastCommandPos = zcon->lastcommand.GetNum();
#endif

				zcon->ShowInput();
				return true;
			}

			bool result = true;

			if (Settings::KeySelectionPrev->GetPressed(Settings::ChangeSelectionThrottleMs))
			{
				gridView->selection.index = std::max(gridView->selection.index - 1, 0);
				result = true;
			}

			if (Settings::KeySelectionNext->GetPressed(Settings::ChangeSelectionThrottleMs))
			{
				gridView->selection.index = std::min(gridView->selection.index + 1, (int)gridView->entries.GetNum() - 1);
				result = true;
			}
			return result;
		}

		bool TryRemoveWord()
		{
			switch (state)
			{
			case TState::MainConsole:
			case TState::MainConsoleShowHints:
				if (Settings::KeyConsoleDeleteWord->GetToggled() && GetCommand().tokens.size())
				{
					zcon->instr = "";

					for (int i = 0; i < (int)GetCommand().tokens.size() - 1; i++)
					{
						if (i)
							zcon->instr += " ";
						zcon->instr += GetCommand().tokens[i].GetVector();
					}

					if (zcon->instr.Length())
						zcon->instr += " ";

#if ENGINE == Engine_G1 || ENGINE == Engine_G1A
					OnEdit(zcon);
#else
					zcon->lastCommandPos = zcon->lastcommand.GetNum();
#endif

					zcon->ShowInput();
					return true;
				}
				return false;

			default:
				return false;
			}
		}

		void OnExit()
		{
			delete gridView;
		}

		void OnLoop()
		{
			if (!ogame)
				return OnLoadBegin();
			
			const bool listShown = TryShowList();
			const bool commandParsed = TryParseCommand();
			const bool hintsShown = TryShowHints(commandParsed);

			if (IsInMainConsole())
				state = hintsShown ? TState::MainConsoleShowHints : TState::MainConsole;
			else
				if (IsInAuxConsole())
					state = TState::AuxConsole;
				else
					state = listShown ? TState::NoConsoleShowList : TState::NoConsole;

			const bool playerFreezeToggled = TryToggleFreezePlayer();
			const bool selectionChanged = TryManipulateSelection();
			const bool wordRemoved = TryRemoveWord();

			if (selectionChanged || wordRemoved)
				if (TryParseCommand())
					TryShowHints(true);
		}

		void OnLoadBegin()
		{
			input = "";
			command.Parse(input);
			state = TState::NoConsole;
		}

		CConsoleContext() :
			state(TState::NoConsole)
		{
			command.Parse("");
			gridView = new CGridView();

			ADDSUB(Exit);
			ADDSUB(Loop);
			ADDSUB(LoadBegin);

			Settings::ConsoleFreezePlayer.onChange += [this](auto& option)
			{
				if (!Settings::ConsoleFreezePlayer && player && player->sleepingMode != zVOB_AWAKE && IsInConsole())
					player->SetSleeping(false);
			};
		}
	public:
		std::vector<std::unique_ptr<CShowListEntry>>& GetShowList()
		{
			return showList;
		}

		TParsedCommand& GetCommand()
		{
			return command;
		}

		CSymbolCache& GetSymbolHelperCache()
		{
			if (!symbolHelperCache)
				symbolHelperCache = std::make_unique<CSymbolCache>(parser);
			return *symbolHelperCache;
		}

		CFpsCounter& GetFpsCounter()
		{
			if (!fpsCounter)
				fpsCounter = std::make_unique<CFpsCounter>();
			return *fpsCounter;
		}

		template <class T, class ...TArgs>
		void AddCommand(TArgs&&... args)
		{
			commands.push_back(std::make_unique<T>(args...));
		}

		bool TryExecute(string& output)
		{
			for (std::unique_ptr<CCommandBase>& command : commands)
				if (command->TryExecute(output))
					return true;

			return false;
		}

		static CConsoleContext& GetInstance()
		{
			static CConsoleContext context;
			return context;
		}
	};
}