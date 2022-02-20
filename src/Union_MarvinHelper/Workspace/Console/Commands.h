#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <iomanip>

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
			CSymbol res = func->Call(args);

			switch (res.GetType())
			{
			case CSymbol::Type::DummyInt:
				return res.GetValue<int>(0);

			case CSymbol::Type::DummyFloat:
				return res.GetValue<float>(0);

			case CSymbol::Type::DummyString:
				return A"\"" + res.GetValue<string>(0) + A"\"";

			case CSymbol::Type::Unknown:
				return "void";

			case CSymbol::Type::Instance:
			case CSymbol::Type::VarInstance:
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
			if (args.size() > 1)
				return;

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
				std::unordered_set<zCVob*> set;

				for (int i = 1; i <= vobs.GetNum(); i++)
				{
					if (!set.count(vobs[i - 1]) && (A i).StartWith(args.back()) && vobs[i - 1] && !vobs[i - 1]->CastTo<zCVobSpot>())
					{
						hints.push_back(i);
						set.insert(vobs[i - 1]);
					}
				}

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

		bool CheckSymbol(const CSymbol& symbol) const
		{
			if (symbol.GetType() != CSymbol::Type::Instance)
				return false;

			const int base = parser->GetBaseClass(symbol.GetSymbol());
			return (c_npc == base || c_item == base) && symbol.IsGlobal();
		}

		bool CheckSymbolInfo(const TSymbolInfo& info) const
		{
			return CheckSymbol(info.first) && CheckHintI(info.second);
		}

		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() != 1)
				return;

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

	class CLowfpsVisual : public CStringConvertible
	{
	private:
		const double threshold, time;
		const string title;

		mutable CFpsCounter::CBenchmark benchmark;

		mutable double lowfpsTime, tempTime, avgFrameTime;
		mutable size_t frameCount;

		bool CalcAvgFrameTime(double frameTime) const
		{
			frameCount += 1;
			return (avgFrameTime += frameTime) >= time;
		}

		bool CalcLowfpsTime(double frameTime) const
		{
			if (frameTime > avgFrameTime / threshold)
				lowfpsTime += frameTime;

			tempTime += frameTime;
			return tempTime >= time;
		}

	public:
		CLowfpsVisual(CFpsCounter& fpsCounter, const double& threshold, const double& time) :
			threshold(threshold),
			time(time),
			benchmark(fpsCounter, time),
			title(A"lowfps " + threshold + " " + time + ": ")
		{

		}

		virtual string ToString() const override
		{
			lowfpsTime = tempTime = avgFrameTime = frameCount = 0;

			benchmark.handleFrameTime = std::bind(&CLowfpsVisual::CalcAvgFrameTime, this, std::placeholders::_1);
			benchmark.ProcessFrameTimes();

			if (frameCount <= 0 || avgFrameTime <= 0)
				return title + "undefined";

			avgFrameTime /= frameCount;

			benchmark.handleFrameTime = std::bind(&CLowfpsVisual::CalcLowfpsTime, this, std::placeholders::_1);
			benchmark.ProcessFrameTimes();

			return title + A(lowfpsTime / tempTime * 100) + "%";
		}
	};

	class CShowLowfpsCommand : public CConsoleCommand
	{
	protected:
		virtual string Execute() override
		{
			double threshold = 0.5;
			double time = 10.0;

			if (args.size() >= 1)
				threshold = CoerceInRange(atof(args[0].GetVector()), 0.0, 0.01, 2.00);

			if (args.size() >= 2)
				time = CoerceInRange(atof(args[1].GetVector()), 0.0, 0.01, 10000.0);

			string command = GetPrefix() + " " + threshold + " " + time;
			auto visual = std::make_unique<CLowfpsVisual>(context.GetFpsCounter(), threshold, time);
			context.GetShowList().push_back(std::make_unique<CSimpleEntry>(std::move(visual), command));

			return "Ok";
		}

	public:
		CShowLowfpsCommand() :
			CConsoleCommand("show lowfps", "low fps benchmark")
		{

		}
	};

	class CDecompileCommand : public CConsoleCommand
	{
	protected:
		static std::string GetFolder()
		{
			return (A zoptions->GetDirString(DIR_ROOT) + "\\Console\\").GetVector();
		}

		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() != 1)
				return;

			auto infos = context.GetSymbolHelperCache().Select([&](const TSymbolInfo& info)
				{
					bool success = false;
					success = success || info.first.GetType() == CSymbol::Type::Instance;
					success = success || info.first.GetType() == CSymbol::Type::Prototype;
					success = success || info.first.GetType() == CSymbol::Type::Func;
					success = success && CheckHintI(info.second);
					return success;
				});

			for (const TSymbolInfo* info : infos)
				hints.push_back(info->second);
		}

		virtual string Execute() override
		{
			if (args.empty())
				return "Fail: no argument provided";

			CSymbol symbol(parser, args.front());

			if (symbol.GetType() == CSymbol::Type::Unknown)
				return "Fail: symbol not found";

			bool success = false;
			success = success || symbol.GetType() == CSymbol::Type::Instance;
			success = success || symbol.GetType() == CSymbol::Type::Prototype;
			success = success || symbol.GetType() == CSymbol::Type::Func;

			if (!success)
				return "Fail: symbol has wrong type";

			try
			{
				auto ast = BuildAst(symbol);
				std::string path = GetFolder() + (A symbol.GetSymbol()->name).Lower().GetVector() + ".d";

				{
					auto file = std::make_unique<zFILE_FILE>(path.c_str());
					file->DirCreate();
				}

				std::ofstream out(path);

				CSymbol instance = symbol;
				EmitCode(out, symbol, ast.get(), instance);

				out.close();
				system((std::string("start \"\" \"") + path + std::string("\"")).c_str());
			}
			catch (const std::exception& e)
			{
				return A"Fail: unable to decompile (" + e.what() + A")";
			}

			return "Ok.";
		}

	public:
		CDecompileCommand() :
			CConsoleCommand("decompile", "decompiles a function")
		{

		}
	};

	class CShowCursorCommand : public CConsoleCommand
	{
	protected:
		virtual string Execute() override
		{
			int& winCursorShowState = *(int*)ZenDef(0x0086F4D0, 0x008B50EC, 0x008C5C0C, 0x008D4244);
			
			for (int tryNum = 1; true; tryNum++)
			{
				winCursorShowState = ShowCursor(true);

				if (winCursorShowState > 0)
					return "Ok.";
			}
		}

	public:
		CShowCursorCommand() :
			CConsoleCommand("show cursor", "shows mouse cursor")
		{

		}
	};

	class CHideCursorCommand : public CConsoleCommand
	{
	protected:
		virtual string Execute() override
		{
			int& winCursorShowState = *(int*)ZenDef(0x0086F4D0, 0x008B50EC, 0x008C5C0C, 0x008D4244);

			for (int tryNum = 1; true; tryNum++)
			{
				winCursorShowState = ShowCursor(false);

				if (winCursorShowState < -1)
					return "Ok.";
			}
		}

	public:
		CHideCursorCommand() :
			CConsoleCommand("hide cursor", "hides mouse cursor")
		{

		}
	};

	class CPrintWeaponsCommand : public CConsoleCommand
	{
	protected:
		static std::string GetFolder()
		{
			return (A zoptions->GetDirString(DIR_ROOT) + "\\Console\\").GetVector();
		}

		static std::vector<zVEC3> GetVertexes(zCProgMeshProto* visual)
		{
			std::vector<zVEC3> vertexes;
			vertexes.reserve(64u);

			auto& posList{ visual->posList };
			auto& subMeshList{ visual->subMeshList };

			for (int i = 0; i < visual->numSubMeshes; i++)
				for (int p = 0; p < subMeshList[i].triList.GetNum(); p++)
					for (int k = 0; k < 3; k++)
						vertexes.emplace_back(posList[subMeshList[i].wedgeList[subMeshList[i].triList[p].wedge[k]].position]);

			return vertexes;
		}

		virtual string Execute() override
		{
			std::string path = GetFolder() + "weapons.txt";
			std::make_unique<zFILE_FILE>(path.c_str())->DirCreate();

			std::vector<std::pair<ZOwner<oCItem>, float>> items;
			int c_item = parser->GetIndex(oCItem::classDef->scriptClassName);

			for (int i = 0; i < parser->symtab.GetNumInList(); i++)
			{
				CSymbol symbol{ parser, i };

				if (symbol.GetType() != CSymbol::Type::Instance)
					continue;

				if (parser->GetBaseClass(symbol.GetSymbol()) != c_item)
					continue;

				std::pair<ZOwner<oCItem>, float> ele{ ZOwner<oCItem>(zfactory->CreateItem(i)), 0.0f };

				if (!ele.first)
					continue;

				if (!ele.first->HasFlag(ITM_CAT_NF))
					continue;

				items.emplace_back(std::move(ele));
				auto& e = items.back();

				e.first->CreateVisual();
				zCProgMeshProto* visual = COA3(e.first, visual, CastTo<zCProgMeshProto>());

				if (!visual)
					continue;

				for (const zVEC3& vertex : GetVertexes(visual))
					if (-vertex[VX] > e.second)
						e.second = -vertex[VX];
			}

			std::sort(items.begin(), items.end(), [](auto& x, auto& y)
				{
					if (x.second && y.second)
						return x.first->range - x.second > y.first->range - y.second;

					if (!x.second && !y.second)
						return false;

					return !x.second;
				});

			std::ofstream out(path);
			out << std::left << std::fixed << std::setprecision(2);

			out << std::setw(8) << "ID"
				<< std::setw(32) << "SYMBOL"
				<< std::setw(64) << "NAME"
				<< std::setw(12) << "RANGE"
				<< std::setw(12) << "VISUALRANGE"
				<< std::setw(12) << "DIFFERENCE"
				<< std::endl;

			for (const auto& item : items)
			{
				out << std::setw(8) << item.first->instanz
					<< std::setw(32) << item.first->objectName
					<< std::setw(64) << item.first->name
					<< std::setw(12) << item.first->range;

				if (item.second)
					out << std::setw(12) << item.second
						<< std::setw(12) << item.first->range - item.second;

				out << std::endl;
			}

			out.close();
			system((std::string("start \"\" \"") + path + std::string("\"")).c_str());
			return "Ok.";
		}

	public:
		CPrintWeaponsCommand() :
			CConsoleCommand("print weapons", "prints melee weapons script and visual ranges")
		{

		}
	};

	class CGoRouteCommand : public CConsoleCommand
	{
	protected:
		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() > 3)
				return;

			if (args.size() == 1)
			{
				std::unordered_set<int> instances;

				for (oCNpc* npc : ogame->GetGameWorld()->voblist_npcs)
					if (npc && npc->instanz > 0 && parser->GetSymbol(npc->instanz)->offset == reinterpret_cast<int>(npc))
						if (HasWordI(parser->GetSymbol(npc->instanz)->name, args.back()) && instances.insert(npc->instanz).second)
							hints.push_back((A parser->GetSymbol(npc->instanz)->name).Lower());

				return;
			}

			if (args.size() == 2)
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
				return;
			}

			if (args.size() == 3)
			{
				if (HasWordI("walk", args.back()))
					hints.push_back("walk");

				if (HasWordI("run", args.back()))
					hints.push_back("run");

				return;
			}
		}

		virtual string Execute() override
		{
			if (args.size() < 2)
				return "Fail! Expected at least 2 arguments.";

			string name = args[0];
			name.Upper();

			oCNpc* npc = dynamic_cast<oCNpc*>(ogame->GetGameWorld()->SearchVobByName(name));

			if (!npc)
				return "Fail! Npc not found.";

			int mode = -1;

			if (args.size() >= 3)
			{
				const string& arg = args[2];

				if (arg.CompareI(A"run"))
					mode = ANI_WALKMODE_RUN;
				else
					if (arg.CompareI(A"walk"))
						mode = ANI_WALKMODE_WALK;
			}

			if (mode != -1)
				npc->GetEM(false)->OnMessage(zNEW(oCMsgMovement)(oCMsgMovement::EV_SETWALKMODE, mode), npc);

			npc->GetEM(false)->OnMessage(zNEW(oCMsgMovement)(oCMsgMovement::EV_GOROUTE, (A args[1]).Upper()), npc);
			return "Ok.";
		}

	public:
		CGoRouteCommand() :
			CConsoleCommand("goroute", "ai command for npc")
		{

		}
	};

#pragma region Zuku05

	bool connectionMode = true;

	class CWpConnectCommand : public CConsoleCommand
	{
	protected:
		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() > 2)
				return;

			for (zCWaypoint* wp : ogame->GetGameWorld()->wayNet->wplist)
				if (wp && wp->name.Length() && HasWordI(wp->name, args.back()) && (args.size() == 1 || !args[0].CompareI(A wp->name)))
					hints.push_back((A wp->name).Lower());
		}

		virtual string Execute() override
		{
			if (args.size() < 2)
				return "Fail! Expected 2 arguments!";

			zCWaypoint* const mwp1 = ogame->GetWorld()->wayNet->GetWaypoint(A args[0].Upper());

			if (!mwp1)
				return A"Fail! Waypoint doesn't exists: " + args[0];

			zCWaypoint* const mwp2 = ogame->GetWorld()->wayNet->GetWaypoint(A args[1].Upper());

			if (!mwp2)
				return A"Fail! Waypoint doesn't exists: " + args[1];

			if (mwp1 == mwp2)
				return "Fail! Waypoints are same!";

			if (mwp1->HasWay(mwp2))
			{
				ogame->GetWorld()->wayNet->DeleteWay(mwp1, mwp2);
				return "Disconnected waypoints: " + mwp1->GetName() + " - " + mwp2->GetName();
			}

			ogame->GetWorld()->wayNet->CreateWay(mwp1, mwp2);
			return "Connected waypoints: " + mwp1->GetName() + " - " + mwp2->GetName();
		}

	public:
		CWpConnectCommand() :
			CConsoleCommand("wp connect", "connects or disconnects two waypoints")
		{

		}
	};

	class CWpAddCommand : public CConsoleCommand
	{
	protected:
		friend class CWpRenameCommand;
		friend class CFpAddCommand;

		static bool IsFreeWaypointName(const zSTRING& name)
		{
			return !ogame->world->wayNet->GetWaypoint(name);
		}

		static zCWaypoint* AddWaypoint(const string& name, const zVEC3& position, const zVEC3& atVector)
		{
			zCVobWaypoint* const vobWp = new zCVobWaypoint();
			vobWp->SetVobName(name);
			ogame->world->AddVob(vobWp);
			vobWp->SetCollDet(false);
			vobWp->SetPositionWorld(position);
			vobWp->SetHeadingAtWorld(atVector);

			zCWaypoint* const wp = zfactory->CreateWaypoint();
			wp->Init(vobWp);
			wp->SetName(Z name);
			ogame->world->wayNet->InsertWaypoint(wp);
			vobWp->Release();

			return wp;
		}

		static zSTRING IncrementName(const zSTRING& name, const std::function<bool(const zSTRING&)>& predicate)
		{
			zSTRING newName = name;

			if (!isdigit(name.Last()))
				newName += "_01";

			while (!predicate(newName))
			{
				int i;
				int overflow;

				for (i = newName.Length() - 1, overflow = 1; i >= 0 && isdigit(newName[static_cast<unsigned>(i)]); i--)
				{
					int digit = newName[static_cast<unsigned>(i)] - '0' + overflow;
					overflow = digit / 10;
					digit %= 10;

					newName[static_cast<unsigned>(i)] = '0' + digit;
				}

				if (overflow)
				{
					newName[static_cast<unsigned>(i + 1)] = '1';
					newName += "0";
				}
			}

			return newName;
		}

		static zSTRING GenerateWaypointName()
		{
			static int counter = 0;
			counter += 1;
			
			zSTRING name = "WP_";

			for (int i = 100; i > counter; i /= 10)
				name += "0";

			name += counter;

			if (IsFreeWaypointName(name))
				return name;

			return IncrementName(name, &CWpAddCommand::IsFreeWaypointName);
		}

		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() > 1)
				return;

			for (zCWaypoint* wp : ogame->GetGameWorld()->wayNet->wplist)
				if (wp && wp->name.Length() && HasWordI(wp->name, args.back()))
					hints.push_back((A wp->name).Lower());
		}

		virtual string Execute() override
		{
			zSTRING wpName;

			if (args.empty())
				wpName = GenerateWaypointName();
			else
			{
				wpName = (Z args[0]).Upper();

				if (!IsFreeWaypointName(wpName))
					wpName = IncrementName(wpName, &CWpAddCommand::IsFreeWaypointName);
			}

			zCWaypoint* const connectedWp = connectionMode ? ogame->world->wayNet->GetNearestWaypoint(player->GetPositionWorld()) : nullptr;
			zCWaypoint* const wp = AddWaypoint(wpName, player->GetPositionWorld(), player->GetAtVectorWorld());

			if (connectedWp)
				ogame->world->wayNet->CreateWay(connectedWp, wp);

			if (connectedWp)
				return A"Waypoints connected: " + (A connectedWp->GetName()).Lower() + " - " + (A wpName).Lower();

			return A"Waypoint created: " + (A wpName).Lower();
		}

	public:
		CWpAddCommand() :
			CConsoleCommand("wp add", "adds a new waypoint")
		{

		}
	};

	class CWpRemoveCommand : public CConsoleCommand
	{
	protected:
		string Remove(zCWaypoint* wp)
		{
			const string message = "Waypoint removed: " + wp->name;
			ogame->world->wayNet->DeleteWaypoint(wp);
			return message;
		}

		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() > 1)
				return;

			for (zCWaypoint* wp : ogame->GetGameWorld()->wayNet->wplist)
				if (wp && wp->name.Length() && HasWordI(wp->name, args.back()))
					hints.push_back((A wp->name).Lower());
		}

		virtual string Execute() override
		{
			if (args.empty())
			{
				zCWaypoint* const nearestWp = ogame->world->wayNet->GetNearestWaypoint(player->GetPositionWorld());

				if (!nearestWp)
					return "Fail! No waypoint found!";

				return Remove(nearestWp);
			}

			zCWaypoint* const wp = ogame->GetWorld()->wayNet->GetWaypoint(A args[0].Upper());

			if (!wp)
				return A"Fail! No waypoint found: " + (A args[0]).Lower();

			return Remove(wp);
		}

	public:
		CWpRemoveCommand() :
			CConsoleCommand("wp remove", "removes a waypoint")
		{

		}
	};

	class CWpRenameCommand : public CConsoleCommand
	{
	protected:
		friend class CWpMoveCommand;

		static zCWaypoint* Recreate(zCWaypoint* wp, string name, const zVEC3& position, const zVEC3& atVector)
		{
			std::vector<zCWaypoint*> left;
			std::vector<zCWaypoint*> right;

			for (zCWay* way : ogame->world->wayNet->waylist)
			{
				if (!way)
					continue;

				if (way->left == wp)
					right.push_back(way->right);

				if (way->right == wp)
					left.push_back(way->left);
			}

			ogame->world->wayNet->DeleteWaypoint(wp);

			if (!CWpAddCommand::IsFreeWaypointName(name))
				name = CWpAddCommand::IncrementName(name, &CWpAddCommand::IsFreeWaypointName).ToChar();

			wp = CWpAddCommand::AddWaypoint(name, position, atVector);

			for (zCWaypoint* connected : left)
				ogame->world->wayNet->CreateWay(connected, wp);

			for (zCWaypoint* connected : right)
				ogame->world->wayNet->CreateWay(wp, connected);

			return wp;
		}

		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() > 2)
				return;

			for (zCWaypoint* wp : ogame->world->wayNet->wplist)
				if (wp && wp->name.Length() && HasWordI(wp->name, args.back()))
					hints.push_back((A wp->name).Lower());
		}

		virtual string Execute() override
		{
			if (args.size() < 2)
				return "Fail! Not enough arguments.";

			zCWaypoint* wp = ogame->world->wayNet->GetWaypoint((Z args[0]).Upper());

			if (!wp)
				return A"Fail! Waypoint not found: " + args[0];

			const zVEC3 position = wp->GetPositionWorld();
			const zVEC3 atVector = wp->wpvob ? wp->wpvob->GetAtVectorWorld() : zVEC3{ 1, 0, 0 };

			string name = args[1];
			name.Upper();

			wp = Recreate(wp, name, position, atVector);
			return A"Waypoint renamed to " + (A wp->name).Lower();
		}

	public:
		CWpRenameCommand() :
			CConsoleCommand("wp rename", "renames a waypoint")
		{

		}
	};

	class CWpMoveCommand : public CConsoleCommand
	{
	protected:
		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() != 1)
				return;

			for (zCWaypoint* wp : ogame->world->wayNet->wplist)
				if (wp && wp->name.Length() && HasWordI(wp->name, args.back()))
					hints.push_back((A wp->name).Lower());
		}

		virtual string Execute() override
		{
			if (args.empty())
				return "Fail! Not enough arguments.";

			zCWaypoint* wp = ogame->world->wayNet->GetWaypoint((Z args[0]).Upper());

			if (!wp)
				return A"Fail! Waypoint not found: " + args[0];

			CWpRenameCommand::Recreate(wp, wp->name, player->GetPositionWorld(), player->GetAtVectorWorld());
			return A"Waypoint moved: " + (A wp->name).Lower();
		}

	public:
		CWpMoveCommand() :
			CConsoleCommand("wp move", "moves a waypoint to player position")
		{

		}
	};

	class CFpAddCommand : public CConsoleCommand
	{
	protected:
		friend class CFpRemoveCommand;
		friend class CFpRenameCommand;
		friend class CFpMoveCommand;

		static void AddHints(std::vector<string>& hints, const string& arg)
		{
			const size_t size = hints.size();

			for (zCVob* vob : ogame->world->globalVobTree)
				if (dynamic_cast<zCVobSpot*>(vob) && vob->objectName.Length() && HasWordI(vob->objectName, arg))
					hints.push_back((A vob->objectName).Lower());

			std::sort(hints.begin() + size, hints.end());
		}

		static std::unordered_set<std::string> GetAllFreepointNames()
		{
			std::unordered_set<std::string> names;
			names.reserve(512);

			for (zCVob* vob : ogame->world->globalVobTree)
				if (dynamic_cast<zCVobSpot*>(vob))
					names.insert(vob->objectName.ToChar());

			return names;
		}

		static string GenerateFreepointName(const std::unordered_set<std::string>& names, const string& proposal)
		{
			string name = proposal;
			name.Upper();

			if (!name.StartWith("FP_"))
				name = A"FP_" + name;

			if (names.find(name.GetVector()) == names.end())
				return name;

			return CWpAddCommand::IncrementName(name, [&names](const zSTRING& name) { return names.find(name.ToChar()) == names.end(); });
		}

		static zSTRING GenerateFreepointName(const std::unordered_set<std::string>& names)
		{
			static int counter = 0;
			counter += 1;
			zSTRING name = "FP_";

			for (int i = 100; i > counter; i /= 10)
				name += "0";

			name += counter;

			if (names.find(name.ToChar()) == names.end())
				return name;

			return CWpAddCommand::IncrementName(name, [&names](const zSTRING& name) { return names.find(name.ToChar()) == names.end(); });
		}

		static void AddFreepoint(const string& name, const zVEC3& position, const zVEC3& atVector)
		{
			zCVobSpot* const fp = new zCVobSpot();
			fp->SetVobName(name);
			ogame->world->AddVob(fp);
			fp->SetCollDet(false);
			fp->SetPositionWorld(position);
			fp->SetHeadingAtWorld(atVector);
			fp->Release();
		}

		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() > 1)
				return;

			AddHints(hints, args[0]);
		}

		virtual string Execute() override
		{
			std::unordered_set<std::string> names = GetAllFreepointNames();
			const string fpName = args.empty() ? GenerateFreepointName(names) : GenerateFreepointName(names, args[0]);
			AddFreepoint(fpName, player->GetPositionWorld(), player->GetAtVectorWorld());
			return A"Freepoint created: " + (A fpName).Lower();
		}

	public:
		CFpAddCommand() :
			CConsoleCommand("fp add", "adds a new freepoint")
		{

		}
	};

	class CFpRemoveCommand : public CConsoleCommand
	{
	protected:
		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() != 1)
				return;

			CFpAddCommand::AddHints(hints, args[0]);
		}

		virtual string Execute() override
		{
			zCVobSpot* fp = nullptr;

			if (args.empty())
			{
				float fpDistance = 0.0f;
				
				for (zCVob* vob : ogame->world->globalVobTree)
				{
					if (!dynamic_cast<zCVobSpot*>(vob))
						continue;

					const float currentDistance = player->GetDistanceToVob(*vob);

					if (!fp || currentDistance < fpDistance)
					{
						fp = static_cast<zCVobSpot*>(vob);
						fpDistance = currentDistance;
					}
				};
			}
			else
				for (zCVob* vob : ogame->world->globalVobTree)
					if (dynamic_cast<zCVobSpot*>(vob) && vob->objectName.CompareI(Z args[0]))
					{
						fp = static_cast<zCVobSpot*>(vob);
						break;
					}

			if (!fp)
				return "Fail! No freepoint found!";

			const string message = "Freepoint removed: " + A fp->objectName;
			ogame->world->RemoveVob(fp);
			return message;
		}

	public:
		CFpRemoveCommand() :
			CConsoleCommand("fp remove", "removes a freepoint")
		{

		}
	};

	class CFpRenameCommand : public CConsoleCommand
	{
	protected:
		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() > 2)
				return;

			return CFpAddCommand::AddHints(hints, args.back());
		}

		virtual string Execute() override
		{
			if (args.size() < 2)
				return "Fail! Not enough arguments.";

			zCVobSpot* fp = dynamic_cast<zCVobSpot*>(ogame->world->SearchVobByName((Z args[0]).Upper()));

			if (!fp)
				return A"Fail! Freepoint not found: " + args[0];

			const zVEC3 position = fp->GetPositionWorld();
			const zVEC3 atVector = fp->GetAtVectorWorld();
			
			ogame->world->RemoveVob(fp);

			std::unordered_set<std::string> names = CFpAddCommand::GetAllFreepointNames();
			const string name = CFpAddCommand::GenerateFreepointName(names, args[1]);
			CFpAddCommand::AddFreepoint(name, position, atVector);
			return A"Freepoint renamed to " + (A name).Lower();
		}

	public:
		CFpRenameCommand() :
			CConsoleCommand("fp rename", "renames a freepoint")
		{

		}
	};

	class CFpMoveCommand : public CConsoleCommand
	{
	protected:
		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() != 1)
				return;

			const size_t size = hints.size();

			for (zCVob* vob : ogame->world->globalVobTree)
				if (zCVobSpot* fp = dynamic_cast<zCVobSpot*>(vob))
					if (fp->objectName.Length() && HasWordI(fp->objectName, args[0]))
					{
						hints.emplace_back(fp->objectName);
						hints.back().Lower();
					}

			std::sort(hints.begin() + size, hints.end());
		}

		virtual string Execute() override
		{
			if (args.empty())
				return A"Fail! Not enough arguments.";

			zCArray<zCVob*> vobs;
			ogame->world->SearchVobListByName((Z args[0]).Upper(), vobs);
			
			for (zCVob* vob : vobs)
				if (zCVobSpot* fp = dynamic_cast<zCVobSpot*>(vob))
				{
					string name = fp->objectName;
					
					ogame->world->RemoveVob(fp);
					CFpAddCommand::AddFreepoint(name, player->GetPositionWorld(), player->GetAtVectorWorld());

					name.Lower();
					return A"Freepoint moved: " + name;
				}

			return A"Fail! Freepoint not found: " + args[0];
		}

	public:
		CFpMoveCommand() :
			CConsoleCommand("fp move", "moves a freepoint to player position")
		{

		}
	};

	class CWpLinkingCommand : public CConsoleCommand
	{
	protected:
		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() > 1)
				return;

			if (HasWordI("on", args.back()))
				hints.push_back("on");

			if (HasWordI("off", args.back()))
				hints.push_back("off");
		}

		virtual string Execute() override
		{
			if (args.empty())
				return "Fail! Argument missing!";

			if (args[0].CompareI(A"on"))
			{
				connectionMode = true;
				return "Waypoint linking ON";
			}
				
			if (args[0].CompareI(A"off"))
			{
				connectionMode = false;
				return "Waypoint linking OFF";
			}

			return "Fail! Wrong parameter!";
		}

	public:
		CWpLinkingCommand() :
			CConsoleCommand("wp linking", "sets waypoint autoconnection mode for 'wp add' command")
		{

		}
	};

#pragma endregion

	class CPlayAniCommand : public CConsoleCommand
	{
	protected:
		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() != 1 || !player || !player->GetModel())
				return;

			const size_t oldSize = hints.size();

			for (zCModelPrototype* proto : player->GetModel()->modelProtoList)
				if (proto)
					for (int i = 0; i < proto->protoAnis.GetNum(); i++)
						if (zCModelAni* ani = proto->protoAnis[i])
							if (HasWordI(ani->aniName, args.back()))
							{
								hints.push_back(ani->aniName);
								hints.back().Lower();
							}

			std::sort(hints.begin() + oldSize, hints.end());
		}

	public:
		CPlayAniCommand() :
			CConsoleCommand("play ani")
		{

		}
	};

	class CPlayFaceAniCommand : public CConsoleCommand
	{
	protected:
		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() != 1 || !player || !player->GetModel())
				return;

			if (zCModelNodeInst* head = player->GetModel()->SearchNode("BIP01 HEAD"))
				if (zCMorphMesh* mesh = dynamic_cast<zCMorphMesh*>(head->nodeVisual))
					if (mesh->morphProto)
						for (int i = 0; i < mesh->morphProto->aniList.GetNum(); i++)
							if (zCMorphMeshAni* ani = mesh->morphProto->aniList[i])
								if (HasWordI(ani->aniName, args.back()))
								{
									hints.push_back(ani->aniName);
									hints.back().Lower();
								}
		}

	public:
		CPlayFaceAniCommand() :
			CConsoleCommand("play faceani")
		{

		}
	};

	class CAiGotoCommand : public CConsoleCommand
	{
	protected:
		virtual void AddHints(std::vector<string>& hints) override
		{
			if (args.size() > 1)
				return;

			for (zCWaypoint* wp : ogame->GetGameWorld()->wayNet->wplist)
				if (wp && wp->name.Length() && HasWordI(wp->name, args.back()))
					hints.push_back((A wp->name).Lower());
		}

	public:
		CAiGotoCommand() :
			CConsoleCommand("aigoto", "")
		{

		}
	};
}