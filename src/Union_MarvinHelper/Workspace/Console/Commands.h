#include <filesystem>

namespace fs = std::experimental::filesystem;

namespace NAMESPACE
{
	class CConDatCommand : public CConsoleCommand
	{
	protected:
		virtual void AddHints(std::vector<string>& hints) override
		{
			string word = args.back();
			word.Lower();

			for (zCConDat* con = zcon->list.root; con; con = con->next)
			{
				TParsedCommand par;
				par.Parse((A con->name).Lower());

				if (args.size() > par.tokens.size())
					continue;

				bool matched = true;
				for (size_t i = 0; i < args.size() - 1 && matched; i++)
					if (!args[i].CompareI(par.tokens[i]))
						matched = false;

				if (matched && HasWordI(par.tokens[args.size() - 1], args.back()))
					hints.push_back(par.tokens[args.size() - 1]);
			}

			std::sort(begin(hints), end(hints));
			hints.erase(std::unique(begin(hints), end(hints)), end(hints));
		}
	};

	class CShowVarsCommand : public CConsoleCommand
	{
	protected:
		virtual void AddHints(std::vector<string>& hints) override
		{
			CLeftAssignableIncomplete var(args.back());
			hints = var.GetHints(context.GetSymbolHelperCache());
		}

		virtual string Execute() override
		{
			size_t valid = 0;
			const string commandText = GetPrefix() + " ";

			for (const string& arg : args)
			{
				auto var = CLeftAssignable::TryCreate(arg);
				if (var)
				{
					valid += 1;
					context.GetShowList().push_back(std::make_unique<CSimpleEntry>(std::move(var), commandText + arg));
				}
			}

			return A valid + " out of " + args.size() + " vars are shown";
		}

	public:
		CShowVarsCommand() :
			CConsoleCommand("show vars", "Shows listed variables values")
		{

		}
	};

	class CHideVarsCommand : public CConsoleCommand
	{
	protected:
		virtual void AddHints(std::vector<string>& hints) override
		{
			std::unordered_set<int> nums;

			for (size_t i = 0; i < args.size() - 1; i++)
			{
				TParsedToken par;
				par.Parse(args[i]);

				if (par.IsInt() && par.GetInt() >= 1 && par.GetInt() <= (int)context.GetShowList().size())
					nums.insert(par.GetInt());
			}

			for (size_t i = 1; i <= context.GetShowList().size(); i++)
				if (nums.find(i) == std::end(nums))
				{
					string hint = i;
					if (hint.StartWith(args.back()))
						hints.push_back(hint);
				}
		}

		virtual string Execute() override
		{
			if (args.empty())
			{
				size_t num = context.GetShowList().size();
				context.GetShowList().clear();
				return A num + " entries hided";
			}

			std::vector<int> nums;

			for (size_t i = 0; i < args.size(); i++)
			{
				TParsedToken par;
				par.Parse(args[i]);

				if (par.IsInt() && par.GetInt() >= 1 && par.GetInt() <= (int)context.GetShowList().size())
					nums.push_back(par.GetInt());
			}

			std::sort(nums.begin(), nums.end(), std::greater<int>());
			auto end = std::unique(nums.begin(), nums.end());

			for (auto it = nums.begin(); it != end; it++)
				context.GetShowList().erase(context.GetShowList().begin() + *it - 1);

			return A(end - nums.begin()) + " entries hided";
		}

	public:
		CHideVarsCommand() :
			CConsoleCommand("hide vars", "Removes entries from the show list")
		{

		}
	};

	class CSetVarCommand : public CConsoleCommand
	{
	protected:
		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() == 1)
			{
				CLeftAssignableIncomplete left(args.front());
				hints = left.GetHints(context.GetSymbolHelperCache());
				return;
			}

			auto left = CLeftAssignable::TryCreate(args.front());
			if (!left)
				return;

			if (left->GetCount() < (int)args.size() - 1)
				return;

			for (size_t i = 1; i < args.size() - 1; i++)
			{
				auto right = CRightAssignable::TryCreate(args[i]);
				if (!right)
					return;

				if (!left->IsEleAssignableFrom(*right))
					return;
			}

			CRightAssignableIncomplete right(args.back());
			hints = right.GetHints(*left, context.GetSymbolHelperCache());
		}

		virtual string Execute() override
		{
			if (args.size() < 2)
				return "The command needs at least 2 arguments";

			auto left = CLeftAssignable::TryCreate(args.front());
			if (!left)
				return A"Cant assign to '" + args.front() + "'";

			if (left->GetCount() != args.size() - 1)
				return A"Assignment failed: expected " + left->GetCount() + " values, got " + (args.size() - 1);

			std::vector<std::unique_ptr<CRightAssignable>> rights;

			for (size_t i = 1; i < args.size(); i++)
			{
				auto right = CRightAssignable::TryCreate(args[i]);

				if (!right || !left->IsEleAssignableFrom(*right))
					return A"Cant assign from '" + args[i] + "'";

				rights.push_back(std::move(right));
			}

			left->Assign(rights);
			return "Ok";
		}

	public:
		CSetVarCommand() :
			CConsoleCommand("set var", "Sets the variable values")
		{

		}
	};

	class CCallableVisual : public CStringConvertible
	{
	private:
		std::unique_ptr<CCallable> func;
		std::vector<std::unique_ptr<CArgument>> args;
		std::vector<string> tokens;

	public:
		CCallableVisual(std::unique_ptr<CCallable>&& func, std::vector<std::unique_ptr<CArgument>>&& args, const std::vector<string>& tokens) :
			func(std::move(func)),
			args(std::move(args)),
			tokens(tokens)
		{

		}

		string CalcResult() const
		{
			CSymbolHelper res = func->Call(args);

			switch (res.GetType())
			{
			case CSymbolHelper::Type::DummyInt:
				return res.GetValue<int>(0);

			case CSymbolHelper::Type::DummyFloat:
				return res.GetValue<float>(0);

			case CSymbolHelper::Type::DummyString:
				return A"\"" + res.GetValue<string>(0) + A"\"";

			case CSymbolHelper::Type::Unknown:
				return "void";

			case CSymbolHelper::Type::Instance:
			case CSymbolHelper::Type::VarInstance:
				return AHEX32(res.GetValue<int>(0));

			default:
				ASSERT(false);
			}
		}

		virtual string ToString() const override
		{
			string s;

			for (const string& token : tokens)
			{
				if (s.Length())
					s += " ";
				s += token;
			}

			s += ": ";
			return s + CalcResult();
		}
	};

	class CCallCommand : public CConsoleCommand
	{
	protected:
		string TryExecuteImpl(std::unique_ptr<CCallable>& func, std::vector<std::unique_ptr<CArgument>>& params)
		{
			if (args.empty())
				return "Fail: function name have not provided";

			func = CCallable::TryCreate(args.front());
			if (!func)
				return A"Cant call '" + args.front() + "'";

			if ((int)args.size() - 1 != func->GetArgumentsCount())
				return A"Fail: expected " + func->GetArgumentsCount() + " arguments, got " + ((int)args.size() - 1);

			for (int i = 1; i < (int)args.size(); i++)
			{
				auto param = CArgument::TryCreate(args[i]);

				if (!param)
					return A"Invalid argument '" + args[i] + A"'";

				if (!param->CanPushAs(func->GetArgumentDescription(i)))
					return A"Incompatible argument '" + args[i] + A"'";

				params.push_back(std::move(param));
			}

			return "";
		}

		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() == 1)
			{
				CCallableIncomplete func(args.front());
				hints = func.GetHints(context.GetSymbolHelperCache());
				return;
			}

			auto func = CCallable::TryCreate(args.front());
			if (!func)
				return;

			if ((int)args.size() - 1 > func->GetArgumentsCount())
				return;

			for (int i = 1; i < (int)args.size() - 1; i++)
			{
				auto arg = CArgument::TryCreate(args[i]);
				if (!arg || !arg->CanPushAs(func->GetArgumentDescription(i)))
					return;
			}

			CArgumentIncomplete arg(args.back());
			hints = arg.GetHints(func->GetArgumentDescription(args.size() - 1), context.GetSymbolHelperCache());
		}

		virtual string Execute() override
		{
			std::unique_ptr<CCallable> func;
			std::vector<std::unique_ptr<CArgument>> params;

			string res = TryExecuteImpl(func, params);
			if (res.Length())
				return res;

			CCallableVisual visual(std::move(func), std::move(params), args);
			return A"Success! Result: " + visual.CalcResult();
		}

		CCallCommand(const string& prefix, const string& description) :
			CConsoleCommand(prefix, description)
		{

		}

	public:
		CCallCommand() :
			CConsoleCommand("call", "Calls script or external function")
		{

		}
	};

	class CShowCallCommand : public CCallCommand
	{
	protected:
		virtual string Execute() override
		{
			std::unique_ptr<CCallable> func;
			std::vector<std::unique_ptr<CArgument>> params;
			
			string res = TryExecuteImpl(func, params);
			if (res.Length())
				return res;

			auto callableVisual = std::make_unique<CCallableVisual>(std::move(func), std::move(params), args);
			context.GetShowList().push_back(std::make_unique<CSimpleEntry>(std::move(callableVisual), context.GetCommand().ToString()));
			return A"Success!";
		}

	public:
		CShowCallCommand() :
			CCallCommand("show call", "Calls script or external function each frame")
		{

		}
	};

	class CGotoWaypointCommand : public CConsoleCommand
	{
	protected:
		virtual void AddHints(std::vector<string>& hints) override
		{
			for (zCWaypoint* wp : ogame->GetGameWorld()->wayNet->wplist)
				if (wp && wp->name.Length() && HasWordI(wp->name, args.back()))
					hints.push_back((A wp->name).Lower());

			CVobTraverser traverser;
			traverser.handle = [&](zCVob* vob)
			{
				zCVobSpot* fp = COA2(vob, CastTo<zCVobSpot>());
				if (fp && fp->objectName.Length() && HasWordI(fp->objectName, args.back()))
					hints.push_back((A fp->objectName).Lower());
			};

			traverser.TraverseVobTree();
		}

	public:
		CGotoWaypointCommand() :
			CConsoleCommand("goto waypoint")
		{

		}
	};

	class CGotoVobCommand : public CConsoleCommand
	{
	protected:
		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() > 2)
				return;

			if (args.size() == 2)
			{
				zCArray<zCVob*> vobs;
				ogame->GetGameWorld()->SearchVobListByName((A args.front()).Upper(), vobs);

				hints.reserve(vobs.GetNum());

				for (int i = 1; i <= vobs.GetNum(); i++)
					if ((A i).StartWith(args.back()) && vobs[i - 1] && !vobs[i - 1]->CastTo<zCVobSpot>())
						hints.push_back(i);

				return;
			}

			std::unordered_set<string, CStringHasher, CStringEqualityComparer> names;

			CVobTraverser traverser;
			traverser.handle = [&](zCVob* vob)
			{
				if (!vob || vob->CastTo<zCVobSpot>())
					return;

				string vobName = vob->objectName;

				if (!CheckHintI(vobName))
					return;
				
				vobName.Lower();
				if (names.insert(vobName).second)
					hints.push_back(vobName);
			};

			traverser.TraverseVobTree();
			traverser.TraverseVobList();

			std::sort(hints.begin(), hints.end());
		}

	public:
		CGotoVobCommand() :
			CConsoleCommand("goto vob")
		{

		}
	};

	class CInsertCommand : public CConsoleCommand
	{
	protected:
		int c_npc, c_item;

		bool CheckSymbol(const CSymbolHelper& symbol) const
		{
			if (symbol.GetType() != CSymbolHelper::Type::Instance)
				return false;

			const int base = parser->GetBaseClass(symbol.GetIndex());
			return (c_npc == base || c_item == base) && symbol.IsGlobal();
		}

		bool CheckSymbolInfo(const TSymbolInfo& info) const
		{
			return CheckSymbol(info.first) && CheckHintI(info.second);
		}

		virtual void AddHints(std::vector<string>& hints) override
		{
			c_npc = parser->GetIndex(oCNpc::classDef->scriptClassName);
			c_item = parser->GetIndex(oCItem::classDef->scriptClassName);

			if (c_npc == -1)
				c_npc = -2;

			if (c_item == -1)
				c_item = -2;

			hints.reserve(200);

			for (const TSymbolInfo* info : context.GetSymbolHelperCache().Select(std::bind(&CInsertCommand::CheckSymbolInfo, this, std::placeholders::_1)))
				hints.push_back(info->second);
		}

	public:
		CInsertCommand() :
			CConsoleCommand("insert")
		{

		}
	};

	bool zFail(int error)
	{
		return error != 0 && error != 1;
	}

	class CExecuteCommand : public CConsoleCommand
	{
	protected:
		friend class CSaveShowListCommand;

		static std::string GetFolder()
		{
			return (A zoptions->GetDirString(DIR_ROOT) + "\\Console\\").GetVector();
		}

		static std::vector<fs::path> ListFiles()
		{
			std::vector<fs::path> files;

			std::error_code error;
			auto iterator = fs::directory_iterator(GetFolder(), error);
			if (error)
				return { };

			for (const auto& e : iterator)
			{
				if (e.path().has_filename())
					files.push_back(e.path());
			}

			return files;
		}

		std::vector<string> ListFiles(const string& regex) const
		{
			std::vector<fs::path> files = ListFiles();
			std::vector<string> filtered;
			std::regex re(regex.GetVector(), std::regex_constants::ECMAScript | std::regex_constants::icase);
			
			for (const fs::path& file : files)
			{
				std::smatch match;
				std::string pathStr = file.string();
				if (!std::regex_match(pathStr, match, re))
					continue;

				string name = match[1].str().c_str();
				name.Lower();
				filtered.push_back(name);
			}

			std::sort(filtered.begin(), filtered.end());
			filtered.erase(std::unique(filtered.begin(), filtered.end()), filtered.end());
			return filtered;
		}

		std::vector<string> ListNames() const
		{
			return ListFiles("^.*\\\\([^\\s]+)\\.CONSOLE$");
		}

		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() > 1)
				return;

			for (const string& name : ListNames())
				if (CheckHintI(name))
					hints.push_back(name);
		}

		virtual string Execute() override
		{
			if (args.empty())
				return "Fail: .console file name not provided";

			string name = args.front();
			name.Lower();

			std::ifstream in(GetFolder() + name.GetVector() + ".console");
			if (in.fail())
				return "Fail: cant read the file";

			auto commandScope = AssignTemp(context.GetCommand(), context.GetCommand());
			std::string line;
			int commands = 0;

			while (std::getline(in, line))
			{
				if (line.empty())
					continue;

				commands += 1;
				context.GetCommand().Parse(line.c_str());
				zcon->Evaluate(Z context.GetCommand().ToString());
			}

			return A commands + " commands have been executed";
		}

	public:
		CExecuteCommand() :
			CConsoleCommand("execute", "Executes content of .console file")
		{
			
		}
	};

	class CSaveShowListCommand : public CConsoleCommand
	{
	protected:
		virtual string Execute() override
		{
			if (context.GetShowList().empty())
				return "Fail: show list is empty";

			if (args.empty())
				return "Fail: filename not provided";

			fs::path folder = CExecuteCommand::GetFolder();
			fs::create_directory(folder);
			std::ofstream out(folder.string() + args.front().GetVector() + ".console");
			if (out.fail())
				return "Fail: cant write to file";

			for (const auto& e : context.GetShowList())
				out << e->GetCommandText().GetVector() << std::endl;

			return A"Success: " + context.GetShowList().size() + " commands have been saved";
		}


	public:
		CSaveShowListCommand() :
			CConsoleCommand("save showlist", "saves command sequence, that can build current show list")
		{
			
		}
	};
}