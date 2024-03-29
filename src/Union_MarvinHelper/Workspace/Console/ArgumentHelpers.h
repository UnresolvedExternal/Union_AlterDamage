namespace NAMESPACE
{
	string UpperSafe(const string& str)
	{
		if (!str.Length())
			return { };

		if (str[0] == (char)255)
			return string(str[0]) + str.Copy(1, str.Length() - 1).Upper();

		return (A str).Upper();
	}

	class CLeftAssignable;

	class CRightAssignableIncomplete
	{
	protected:
		TParsedToken token;

		bool CheckSymbol(const CLeftAssignable& left, const CSymbol& symbol) const;
		bool CheckName(const string& name) const;
		bool CheckInfo(const CLeftAssignable& left, const TSymbolInfo& info) const;
		void AddHints(std::vector<string>& hints, const TSymbolInfo& info) const;
		std::vector<string> AsHints(const std::vector<const TSymbolInfo*>& infos) const;

	public:
		CRightAssignableIncomplete(const string& token);
		std::vector<string> GetHints(const CLeftAssignable& left, CCache<TSymbolInfo>& cache) const;
	};

	class CRightAssignable : protected CRightAssignableIncomplete
	{
	protected:
		CSymbol symbol;
		int start;

		CRightAssignable(const string& token);

	public:
		static std::unique_ptr<CRightAssignable> TryCreate(const string& token);

		template <class T>
		bool IsValueType() const;

		template <class T>
		T GetValue() const;
	};

	class CLeftAssignableIncomplete
	{
	protected:
		TParsedVarName varName;
		int c_info;

		bool CheckSymbol(const CSymbol& symbol) const;
		bool CheckName(const string& name) const;
		bool CheckInfo(const TSymbolInfo& info) const;
		void AddHints(std::vector<string>& hints, const TSymbolInfo& info) const;
		std::vector<string> AsHints(const std::vector<const TSymbolInfo*>& infos) const;

	public:
		CLeftAssignableIncomplete(const string& token);
		std::vector<string> GetHints(CCache<TSymbolInfo>& cache) const;
	};

	class CLeftAssignable : protected CLeftAssignableIncomplete, public CStringConvertible
	{
	protected:
		string origin;
		CSymbol symbol;
		oCInfo* info;
		int start, count;

		CLeftAssignable(const string& token);

	public:
		static std::unique_ptr<CLeftAssignable> TryCreate(const string& token);

		template <class T>
		bool IsEleAssignableFrom() const;

		bool IsEleAssignableFrom(const CRightAssignable& right) const;
		int GetCount() const;
		void Assign(const std::vector<std::unique_ptr<CRightAssignable>>& values);
		virtual string ToString() const override;
	};

	class CArgumentIncomplete
	{
	protected:
		TParsedToken token;

		bool CheckSymbol(const CSymbol& paramDesc, const CSymbol& symbol) const;
		bool CheckName(const string& name) const;
		bool CheckInfo(const CSymbol& paramDesc, const TSymbolInfo& info) const;
		void AddHints(std::vector<string>& hints, const TSymbolInfo& info) const;
		std::vector<string> AsHints(const std::vector<const TSymbolInfo*>& infos) const;

	public:
		CArgumentIncomplete(const string& token);
		std::vector<string> GetHints(const CSymbol& paramDesc, CCache<TSymbolInfo>& cache) const;
	};

	class CArgument : protected CArgumentIncomplete
	{
	protected:
		CSymbol symbol;
		int start;

		CArgument(const string& token);

	public:
		static std::unique_ptr<CArgument> TryCreate(const string& token);
		bool CanPushAs(const CSymbol& paramDesc) const;
		void PushAs(const CSymbol& paramDesc) const;
	};

	class CCallableIncomplete
	{
	protected:
		TParsedVarName varName;

		bool CheckSymbol(const CSymbol& symbol) const;
		bool CheckName(const string& name) const;
		bool CheckInfo(const TSymbolInfo& info) const;
		void AddHints(std::vector<string>& hints, const TSymbolInfo& info) const;
		std::vector<string> AsHints(const std::vector<const TSymbolInfo*> infos) const;

	public:
		CCallableIncomplete(const string& token);
		std::vector<string> GetHints(CCache<TSymbolInfo>& cache) const;
	};

	class CCallable : protected CCallableIncomplete
	{
	protected:
		CSymbol symbol;

		CCallable(const string& token);

	public:
		static std::unique_ptr<CCallable> TryCreate(const string& token);
		int GetArgumentsCount() const;
		CSymbol GetArgumentDescription(int index) const;
		CSymbol Call(const std::vector<std::unique_ptr<CArgument>>& arguments) const;
	};

#pragma region CRightAssignableIncomplete

	bool CRightAssignableIncomplete::CheckSymbol(const CLeftAssignable& left, const CSymbol& symbol) const
	{
		if (!symbol.IsGlobal())
			return false;

		switch (symbol.GetType())
		{
		case CSymbol::Type::ExternalVar:
			if (symbol.IsValueType<int>())
				return left.IsEleAssignableFrom<int>();
			if (symbol.IsValueType<float>())
				return left.IsEleAssignableFrom<float>();
			if (symbol.IsValueType<string>())
				return left.IsEleAssignableFrom<string>();

			ASSERT(false);
			return false;

		case CSymbol::Type::VarString:
			return left.IsEleAssignableFrom<string>();

		case CSymbol::Type::VarFloat:
			return left.IsEleAssignableFrom<float>();

		case CSymbol::Type::VarInt:
		case CSymbol::Type::Instance:
		case CSymbol::Type::Func:
			return left.IsEleAssignableFrom<int>();

		case CSymbol::Type::Prototype:
		case CSymbol::Type::ClassVar:
		case CSymbol::Type::Class:
		case CSymbol::Type::ExternalFunc:
		case CSymbol::Type::VarInstance:
			return false;

		case CSymbol::Type::DummyString:
		case CSymbol::Type::DummyFloat:
		case CSymbol::Type::DummyInt:
		case CSymbol::Type::Unknown:
		default:
			ASSERT(false);
		}
	}

	bool CRightAssignableIncomplete::CheckName(const string& name) const
	{
		const TParsedVarName& varName = token.GetVar();

		switch (varName.state)
		{
		case TParsedVarName::TState::Initial:
			return true;

		case TParsedVarName::TState::Name:
			return name.HasWord(varName.name);

		case TParsedVarName::TState::StartIndexInitial:
		case TParsedVarName::TState::StartIndex:
		case TParsedVarName::TState::EndIndexInitial:
		case TParsedVarName::TState::EndIndex:
		case TParsedVarName::TState::Stop:
			return name.Compare(varName.name);

		case TParsedVarName::TState::Fail:
		default:
			ASSERT(false);
		}
	}

	bool CRightAssignableIncomplete::CheckInfo(const CLeftAssignable& left, const TSymbolInfo& info) const
	{
		return CheckSymbol(left, info.first) && CheckName(info.second);
	}

	// applied to filtered infos
	void CRightAssignableIncomplete::AddHints(std::vector<string>& hints, const TSymbolInfo& info) const
	{
		const TParsedVarName& varName = token.GetVar();

		switch (varName.state)
		{
		case TParsedVarName::TState::Initial:
		case TParsedVarName::TState::Name:
			hints.push_back(info.second);
			return;

		case TParsedVarName::TState::StartIndexInitial:
		case TParsedVarName::TState::StartIndex:
		case TParsedVarName::TState::EndIndexInitial:
		case TParsedVarName::TState::EndIndex:
		case TParsedVarName::TState::Stop:
			varName.TryAddIndexedHints(hints, { 0, info.first.GetValuesCount() - 1 }, 1);
			return;

		case TParsedVarName::TState::Fail:
		default:
			ASSERT(false);
		}
	}

	std::vector<string> CRightAssignableIncomplete::AsHints(const std::vector<const TSymbolInfo*>& infos) const
	{
		std::vector<string> hints;
		hints.reserve(infos.size());
		std::for_each(std::begin(infos), std::end(infos), [&](const TSymbolInfo * info) { AddHints(hints, *info); });
		return hints;
	}

	CRightAssignableIncomplete::CRightAssignableIncomplete(const string& token)
	{
		this->token.Parse(token);
	}

	std::vector<string> CRightAssignableIncomplete::GetHints(const CLeftAssignable& left, CCache<TSymbolInfo>& cache) const
	{
		if (token.GetVar().state == TParsedVarName::TState::Fail)
			return { };

		std::vector<const TSymbolInfo*> infos = cache.Select(std::bind(&CRightAssignableIncomplete::CheckInfo, this, left, std::placeholders::_1));
		return AsHints(infos);
	}

#pragma endregion

#pragma region CRightAssignable

	CRightAssignable::CRightAssignable(const string& token) :
		CRightAssignableIncomplete(token),
		start(0)
	{
		if (this->token.IsInt())
			symbol = CSymbol(this->token.GetInt());
		else
			if (this->token.IsFloat())
				symbol = CSymbol(this->token.GetFloat());
			else
				if (this->token.IsString())
					symbol = CSymbol(this->token.GetString());
				else
				{
					if (!this->token.GetVar().IsCompleted())
						return;

					if (this->token.GetVar().endIndex != -1 && this->token.GetVar().endIndex != this->token.GetVar().startIndex)
						return;

					symbol = CSymbol(parser, UpperSafe(this->token.GetVar().name));
					start = this->token.GetVar().startIndex;
				}
	}

	std::unique_ptr<CRightAssignable> CRightAssignable::TryCreate(const string& token)
	{
		std::unique_ptr<CRightAssignable> e(new CRightAssignable(token));

		switch (e->symbol.GetType())
		{
		case CSymbol::Type::ExternalVar:
		case CSymbol::Type::VarString:
		case CSymbol::Type::VarFloat:
		case CSymbol::Type::VarInt:
			if (e->start == -1)
			{
				if (e->symbol.GetValuesCount() != 1)
					return { };
				e->start = 0;
			}

			if (!e->symbol.CheckRange(e->start, 1))
				return { };
			
			if (e->symbol.IsGlobal())
				return e;
			return { };

		case CSymbol::Type::Instance:
		case CSymbol::Type::Func:
			if (e->symbol.IsGlobal())
				return e;
			return { };

		case CSymbol::Type::Prototype:
		case CSymbol::Type::ClassVar:
		case CSymbol::Type::Class:
		case CSymbol::Type::ExternalFunc:
		case CSymbol::Type::VarInstance:
			return { };

		case CSymbol::Type::DummyString:
		case CSymbol::Type::DummyFloat:
		case CSymbol::Type::DummyInt:
			return e;

		case CSymbol::Type::Unknown:
			return { };

		default:
			ASSERT(false);
		}
	}

	template <class T>
	bool CRightAssignable::IsValueType() const
	{
		static_assert(std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, string>, "Bad template type");
		return symbol.IsValueType<T>();
	}

	template <class T>
	T CRightAssignable::GetValue() const
	{
		ASSERT(IsValueType<T>());

		switch (symbol.GetType())
		{
		case CSymbol::Type::ExternalVar:
		{
			if (IsValueType<int>()) goto GetInt;
			if (IsValueType<float>()) goto GetFloat;
			if (IsValueType<string>()) goto GetString;
			ASSERT(false);
		}

		case CSymbol::Type::VarString:
		case CSymbol::Type::DummyString:
		GetString:
			return NoConvert<string, T>(symbol.GetValue<string>(start));

		case CSymbol::Type::VarFloat:
		case CSymbol::Type::DummyFloat:
		GetFloat:
			return NoConvert<float, T>(symbol.GetValue<float>(start));

		case CSymbol::Type::VarInt:
		case CSymbol::Type::DummyInt:
		GetInt:
			return NoConvert<int, T>(symbol.GetValue<int>(start));

		case CSymbol::Type::Instance:
		case CSymbol::Type::Func:
			return NoConvert<int, T>(symbol.GetIndex());

		case CSymbol::Type::Prototype:
		case CSymbol::Type::ClassVar:
		case CSymbol::Type::Class:
		case CSymbol::Type::ExternalFunc:
		case CSymbol::Type::VarInstance:
		case CSymbol::Type::Unknown:
		default:
			ASSERT(false);
		}
	}

#pragma endregion

#pragma region CLeftAssignableIncomplete

	bool CLeftAssignableIncomplete::CheckSymbol(const CSymbol& symbol) const
	{
		switch (symbol.GetType())
		{
			// allow regular global vars assignment
		case CSymbol::Type::ExternalVar:
		case CSymbol::Type::VarString:
		case CSymbol::Type::VarFloat:
		case CSymbol::Type::VarInt:
			return symbol.IsGlobal();

			// allow dialog state assignment
		case CSymbol::Type::Instance:
			if (varName.IsIndexed())
				return false;
			if (parser->GetBaseClass(symbol.GetSymbol()) != c_info)
				return false;
			return true;

		default:
			return false;
		}
	}

	bool CLeftAssignableIncomplete::CheckName(const string& name) const
	{
		switch (varName.state)
		{
		case TParsedVarName::TState::Initial:
			return true;

		case TParsedVarName::TState::Name:
			return name.HasWord(varName.name);

		case TParsedVarName::TState::StartIndexInitial:
		case TParsedVarName::TState::StartIndex:
		case TParsedVarName::TState::EndIndexInitial:
		case TParsedVarName::TState::EndIndex:
		case TParsedVarName::TState::Stop:
			return name.Compare(varName.name);

		case TParsedVarName::TState::Fail:
			return false;

		default:
			ASSERT(false);
		}
	}

	bool CLeftAssignableIncomplete::CheckInfo(const TSymbolInfo& info) const
	{
		return CheckSymbol(info.first) && CheckName(info.second);
	}

	// applied to filtered infos
	void CLeftAssignableIncomplete::AddHints(std::vector<string>& hints, const TSymbolInfo& info) const
	{
		switch (varName.state)
		{
		case TParsedVarName::TState::Initial:
		case TParsedVarName::TState::Name:
			hints.push_back(info.second);
			return;

		case TParsedVarName::TState::StartIndexInitial:
		case TParsedVarName::TState::StartIndex:
		case TParsedVarName::TState::EndIndexInitial:
		case TParsedVarName::TState::EndIndex:
		case TParsedVarName::TState::Stop:
			varName.TryAddIndexedHints(hints, { 0, info.first.GetValuesCount() - 1 }, 4095);
			return;

		case TParsedVarName::TState::Fail:
		default:
			ASSERT(false);
		}
	}

	std::vector<string> CLeftAssignableIncomplete::AsHints(const std::vector<const TSymbolInfo*>& infos) const
	{
		std::vector<string> hints;
		hints.reserve(infos.size());
		std::for_each(std::begin(infos), std::end(infos), [&](const TSymbolInfo * info) { AddHints(hints, *info); });
		return hints;
	}

	CLeftAssignableIncomplete::CLeftAssignableIncomplete(const string& token) :
		c_info(parser->GetIndex("C_INFO"))
	{
		varName.Parse((A token).Lower());
	}

	std::vector<string> CLeftAssignableIncomplete::GetHints(CCache<TSymbolInfo>& cache) const
	{
		if (varName.state == TParsedVarName::TState::Fail)
			return { };

		std::vector<const TSymbolInfo*> infos = cache.Select(std::bind(&CLeftAssignableIncomplete::CheckInfo, this, std::placeholders::_1));
		return AsHints(infos);
	}

#pragma endregion

#pragma region CLeftAssignable

	CLeftAssignable::CLeftAssignable(const string& token) :
		CLeftAssignableIncomplete(token),
		origin(token)
	{
		switch (varName.state)
		{
		case TParsedVarName::TState::Initial:
			return;

		case TParsedVarName::TState::Name:
			symbol = CSymbol(parser, UpperSafe(varName.name));
			if (CheckSymbol(symbol))
			{
				start = 0;
				count = symbol.GetValuesCount();

				if (symbol.GetType() == CSymbol::Type::Instance)
				{
					info = COA3(ogame, GetInfoManager(), GetInformation(symbol.GetIndex()));
					if (!info)
						symbol = { };
				}
			}
			else
				symbol = { };
			return;

		case TParsedVarName::TState::StartIndexInitial:
		case TParsedVarName::TState::StartIndex:
		case TParsedVarName::TState::EndIndexInitial:
		case TParsedVarName::TState::EndIndex:
			return;

		case TParsedVarName::TState::Stop:
			symbol = CSymbol(parser, UpperSafe(varName.name));
			start = varName.startIndex;
			count = (varName.endIndex == -1) ? 1 : (varName.endIndex - start + 1);
			if (!symbol.CheckRange(start, count))
				symbol = { };
			return;

		case TParsedVarName::TState::Fail:
			return;

		default:
			ASSERT(false);
		}
	}

	std::unique_ptr<CLeftAssignable> CLeftAssignable::TryCreate(const string & token)
	{
		std::unique_ptr<CLeftAssignable> e(new CLeftAssignable(token));

		if (e->symbol.GetType() == CSymbol::Type::Unknown)
			return { };

		return e;
	}

	template <class T>
	bool CLeftAssignable::IsEleAssignableFrom() const
	{
		static_assert(std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, string>, "Bad template type");

		if (symbol.IsValueType<int>())
			return std::is_same_v<T, int>;

		if (symbol.IsValueType<float>())
			return std::is_same_v<T, int> || std::is_same_v<T, float>;

		if (symbol.IsValueType<string>())
			return std::is_same_v<T, string>;

		ASSERT(false);
		return false;
	}

	bool CLeftAssignable::IsEleAssignableFrom(const CRightAssignable& right) const
	{
		switch (symbol.GetType())
		{
		case CSymbol::Type::ExternalVar:
		case CSymbol::Type::VarString:
		case CSymbol::Type::VarFloat:
		case CSymbol::Type::VarInt:
			if (right.IsValueType<int>())
				return IsEleAssignableFrom<int>();
			if (right.IsValueType<float>())
				return IsEleAssignableFrom<float>();
			if (right.IsValueType<string>())
				return IsEleAssignableFrom<string>();
			ASSERT(false);

		case CSymbol::Type::Instance:
			return right.IsValueType<int>();

		case CSymbol::Type::ClassVar:
		case CSymbol::Type::Class:
		case CSymbol::Type::ExternalFunc:
		case CSymbol::Type::Prototype:
		case CSymbol::Type::Func:
		case CSymbol::Type::VarInstance:
		case CSymbol::Type::DummyString:
		case CSymbol::Type::DummyFloat:
		case CSymbol::Type::DummyInt:
		case CSymbol::Type::Unknown:
		default:
			ASSERT(false);
		}
	}

	int CLeftAssignable::GetCount() const
	{
		return count;
	}

	void CLeftAssignable::Assign(const std::vector<std::unique_ptr<CRightAssignable>>& values)
	{
		ASSERT(values.size() == GetCount());

		if (info)
		{
			info->told = (bool)values[0]->GetValue<int>();
			return;
		}

		for (int i = 0; i < GetCount(); i++)
		{
			ASSERT(IsEleAssignableFrom(*values[i]));
			const int index = start + i;

			if (values[i]->IsValueType<int>())
			{
				const int value = values[i]->GetValue<int>();
				symbol.IsValueType<int>() ? symbol.SetValue(index, value) : symbol.SetValue(index, (float)value);
				continue;
			}

			if (values[i]->IsValueType<float>())
			{
				const float value = values[i]->GetValue<float>();
				symbol.SetValue(index, value);
				continue;
			}

			if (values[i]->IsValueType<string>())
			{
				const string value = values[i]->GetValue<string>();
				symbol.SetValue(index, value);
				continue;
			}

			ASSERT(false);
		}
	}

	string CLeftAssignable::ToString() const
	{
		if (info)
			return origin + ": " + info->told;

		string s = origin + ":";

		for (int i = 0; i < GetCount(); i++)
		{
			s += " ";

			if (symbol.IsValueType<int>())
			{
				s += symbol.GetValue<int>(start + i);
				continue;
			}

			if (symbol.IsValueType<float>())
			{
				s += symbol.GetValue<float>(start + i);
				continue;
			}

			if (symbol.IsValueType<string>())
			{
				s += A"\"" + symbol.GetValue<string>(start + i) + A"\"";
				continue;
			}

			ASSERT(false);
		}

		return s;
	}

#pragma endregion

#pragma region CArgumentIncomplete

	bool CArgumentIncomplete::CheckSymbol(const CSymbol& paramDesc, const CSymbol& symbol) const
	{
		if (!symbol.IsGlobal())
			return false;

		return symbol.CanPushAsParam(parser, 0, paramDesc);
	}

	bool CArgumentIncomplete::CheckName(const string& name) const
	{
		const TParsedVarName& varName = token.GetVar();

		switch (varName.state)
		{
		case TParsedVarName::TState::Initial:
			return true;

		case TParsedVarName::TState::Name:
			return name.HasWord(varName.name);

		case TParsedVarName::TState::StartIndexInitial:
		case TParsedVarName::TState::StartIndex:
		case TParsedVarName::TState::EndIndexInitial:
		case TParsedVarName::TState::EndIndex:
		case TParsedVarName::TState::Stop:
			return name.Compare(varName.name);

		case TParsedVarName::TState::Fail:
		default:
			ASSERT(false);
		}
	}

	bool CArgumentIncomplete::CheckInfo(const CSymbol& paramDesc, const TSymbolInfo& info) const
	{
		return CheckSymbol(paramDesc, info.first) && CheckName(info.second);
	}

	// applied to filtered infos
	void CArgumentIncomplete::AddHints(std::vector<string>& hints, const TSymbolInfo& info) const
	{
		const TParsedVarName& varName = token.GetVar();

		switch (varName.state)
		{
		case TParsedVarName::TState::Initial:
		case TParsedVarName::TState::Name:
			hints.push_back(info.second);
			return;

		case TParsedVarName::TState::StartIndexInitial:
		case TParsedVarName::TState::StartIndex:
		case TParsedVarName::TState::EndIndexInitial:
		case TParsedVarName::TState::EndIndex:
		case TParsedVarName::TState::Stop:
			varName.TryAddIndexedHints(hints, { 0, info.first.GetValuesCount() - 1 }, 1);
			return;

		case TParsedVarName::TState::Fail:
		default:
			ASSERT(false);
		}
	}

	std::vector<string> CArgumentIncomplete::AsHints(const std::vector<const TSymbolInfo*>& infos) const
	{
		std::vector<string> hints;
		hints.reserve(infos.size());
		std::for_each(std::begin(infos), std::end(infos), [&](const TSymbolInfo* info) { AddHints(hints, *info); });
		return hints;
	}

	CArgumentIncomplete::CArgumentIncomplete(const string& token)
	{
		this->token.Parse(token);
	}

	std::vector<string> CArgumentIncomplete::GetHints(const CSymbol& paramDesc, CCache<TSymbolInfo>& cache) const
	{
		if (token.GetVar().state == TParsedVarName::TState::Fail)
			return { };

		std::vector<const TSymbolInfo*> infos = cache.Select(std::bind(&CArgumentIncomplete::CheckInfo, this, paramDesc, std::placeholders::_1));
		return AsHints(infos);
	}

#pragma endregion

#pragma region CArgument

	CArgument::CArgument(const string& token) :
		CArgumentIncomplete(token),
		start(0)
	{
		if (this->token.IsInt())
			symbol = CSymbol(this->token.GetInt());
		else
			if (this->token.IsFloat())
				symbol = CSymbol(this->token.GetFloat());
			else
				if (this->token.IsString())
					symbol = CSymbol(this->token.GetString());
				else
				{
					if (!this->token.GetVar().IsCompleted())
						return;

					if (this->token.GetVar().endIndex != -1 && this->token.GetVar().endIndex != this->token.GetVar().startIndex)
						return;

					symbol = CSymbol(parser, UpperSafe(this->token.GetVar().name));
					start = this->token.GetVar().startIndex;
					start = std::max(start, 0);
				}
	}

	std::unique_ptr<CArgument> CArgument::TryCreate(const string& token)
	{
		std::unique_ptr<CArgument> e(new CArgument(token));

		switch (e->symbol.GetType())
		{
		case CSymbol::Type::ExternalVar:
		case CSymbol::Type::VarString:
		case CSymbol::Type::VarFloat:
		case CSymbol::Type::VarInt:
			if (e->start == -1)
			{
				if (e->symbol.GetValuesCount() != 1)
					return { };
				e->start = 0;
			}

			if (!e->symbol.CheckRange(e->start, 1))
				return { };

			if (e->symbol.IsGlobal())
				return e;
			return { };

		case CSymbol::Type::VarInstance:
		case CSymbol::Type::Instance:
		case CSymbol::Type::Func:
			if (e->symbol.IsGlobal())
				return e;
			return { };

		case CSymbol::Type::Prototype:
		case CSymbol::Type::ClassVar:
		case CSymbol::Type::Class:
		case CSymbol::Type::ExternalFunc:
			return { };

		case CSymbol::Type::DummyString:
		case CSymbol::Type::DummyFloat:
		case CSymbol::Type::DummyInt:
			return e;

		case CSymbol::Type::Unknown:
			return { };

		default:
			ASSERT(false);
		}
	}

	bool CArgument::CanPushAs(const CSymbol& paramDesc) const
	{
		return symbol.CanPushAsParam(parser, start, paramDesc);
	}

	void CArgument::PushAs(const CSymbol& paramDesc) const
	{
		const bool success = symbol.TryPushAsParam(parser, start, paramDesc);
		ASSERT(success);
	}

#pragma endregion

#pragma region CCallableIncomplete

	bool CCallableIncomplete::CheckSymbol(const CSymbol& symbol) const
	{
		switch (symbol.GetType())
		{
		case CSymbol::Type::ExternalFunc:
		case CSymbol::Type::Func:
			return symbol.IsGlobal();

		case CSymbol::Type::ClassVar:
		case CSymbol::Type::Class:
		case CSymbol::Type::ExternalVar:
		case CSymbol::Type::Instance:
		case CSymbol::Type::Prototype:
		case CSymbol::Type::VarInstance:
		case CSymbol::Type::VarString:
		case CSymbol::Type::VarFloat:
		case CSymbol::Type::VarInt:
		case CSymbol::Type::DummyString:
		case CSymbol::Type::DummyFloat:
		case CSymbol::Type::DummyInt:
		case CSymbol::Type::Unknown:
			return false;

		default:
			ASSERT(false);
		}
	}

	bool CCallableIncomplete::CheckName(const string& name) const
	{
		switch (varName.state)
		{
		case TParsedVarName::TState::Initial:
			return true;

		case TParsedVarName::TState::Name:
			return name.HasWord(varName.name);

		case TParsedVarName::TState::StartIndexInitial:
		case TParsedVarName::TState::StartIndex:
		case TParsedVarName::TState::EndIndexInitial:
		case TParsedVarName::TState::EndIndex:
		case TParsedVarName::TState::Stop:
		case TParsedVarName::TState::Fail:
			return false;

		default:
			ASSERT(false);
		}
	}

	bool CCallableIncomplete::CheckInfo(const TSymbolInfo& info) const
	{
		return CheckSymbol(info.first) && CheckName(info.second);
	}

	// applied to filtered infos
	void CCallableIncomplete::AddHints(std::vector<string>& hints, const TSymbolInfo& info) const
	{
		switch (varName.state)
		{
		case TParsedVarName::TState::Initial:
		case TParsedVarName::TState::Name:
			hints.push_back(info.second);
			return;

		case TParsedVarName::TState::StartIndexInitial:
		case TParsedVarName::TState::StartIndex:
		case TParsedVarName::TState::EndIndexInitial:
		case TParsedVarName::TState::EndIndex:
		case TParsedVarName::TState::Stop:
		case TParsedVarName::TState::Fail:
		default:
			ASSERT(false);
		}
	}

	std::vector<string> CCallableIncomplete::AsHints(const std::vector<const TSymbolInfo*> infos) const
	{
		std::vector<string> hints;
		hints.reserve(infos.size());
		std::for_each(std::begin(infos), std::end(infos), [&](const TSymbolInfo * info) { AddHints(hints, *info); });
		return hints;
	}

	CCallableIncomplete::CCallableIncomplete(const string& token)
	{
		varName.Parse((A token).Lower());
	}

	std::vector<string> CCallableIncomplete::GetHints(CCache<TSymbolInfo>& cache) const
	{
		if (varName.state == TParsedVarName::TState::Fail)
			return { };

		std::vector<const TSymbolInfo*> infos = cache.Select(std::bind(&CCallableIncomplete::CheckInfo, this, std::placeholders::_1));
		return AsHints(infos);
	}

#pragma endregion

#pragma region CCallable

	CCallable::CCallable(const string& token) :
		CCallableIncomplete(token)
	{
		switch (varName.state)
		{
		case TParsedVarName::TState::Initial:
			return;

		case TParsedVarName::TState::Name:
			symbol = CSymbol(parser, UpperSafe(varName.name));
			if (!CheckSymbol(symbol))
				symbol = { };
			return;

		case TParsedVarName::TState::StartIndexInitial:
		case TParsedVarName::TState::StartIndex:
		case TParsedVarName::TState::EndIndexInitial:
		case TParsedVarName::TState::EndIndex:
		case TParsedVarName::TState::Stop:
		case TParsedVarName::TState::Fail:
			return;

		default:
			ASSERT(false);
		}
	}

	std::unique_ptr<CCallable> CCallable::TryCreate(const string& token)
	{
		std::unique_ptr<CCallable> e(new CCallable(token));

		if (e->symbol.GetType() == CSymbol::Type::Unknown)
			return { };

		return e;
	}

	int CCallable::GetArgumentsCount() const
	{
		return symbol.GetSymbol()->ele;
	}

	CSymbol CCallable::GetArgumentDescription(int index) const
	{
		ASSERT(index >= 1 && index <= GetArgumentsCount());
		return CSymbol(parser, symbol.GetIndex() + index);
	}

	CSymbol CCallable::Call(const std::vector<std::unique_ptr<CArgument>>& arguments) const
	{
		ASSERT(GetArgumentsCount() == (int)arguments.size());
		
		auto scope = AssignTemp(parser->datastack.sptr, parser->datastack.sptr);
		auto curParserScope = AssignTemp(zCParser::cur_parser, this->symbol.GetParser());

		for (int i = 0; i < GetArgumentsCount(); i++)
			arguments[i]->PushAs(GetArgumentDescription(i + 1));

		if (symbol.GetType() == CSymbol::Type::ExternalFunc)
		{
			void* address = (void*)symbol.GetValue<int>(0);
			((int(__cdecl*)())address)();
		}
		else
			parser->DoStack(symbol.GetValue<int>(0));

		switch (symbol.GetSymbol()->offset)
		{
		case zPAR_TYPE_INT:
		{
			int result;
			parser->GetParameter(result);
			return CSymbol(result);
		}

		case zPAR_TYPE_FLOAT:
		{
			float result;
			parser->GetParameter(result);
			return CSymbol(result);
		}

		case zPAR_TYPE_STRING:
		{
			zSTRING result;
			parser->GetParameter(result);
			return CSymbol(A result);
		}

		case zPAR_TYPE_INSTANCE:
			return CSymbol(parser, parser->datastack.Pop());

		case zPAR_TYPE_VOID:
			return { };

		default:
			ASSERT(false);
		}
	}

#pragma endregion
}