namespace NAMESPACE
{
	class CSymbol
	{
	public:
		enum class Type
		{
			Unknown,
			ClassVar,
			Class,
			ExternalVar,
			ExternalFunc,
			Instance,
			Prototype,
			Func,
			VarInstance,
			VarString,
			VarFloat,
			VarInt,
			DummyString,
			DummyFloat,
			DummyInt
		};

	private:
		Type type;
		zCParser* parser;
		int index;
		bool isGlobal;

		union
		{
			int dummyInt;
			float dummyFloat;
			zSTRING* dummyString;
		};

		static Type GetType(const zCPar_Symbol* symbol)
		{
			if (!symbol)
				return Type::Unknown;

			const int type = symbol->type;
			const bool isClassVar = symbol->flags & zPAR_FLAG_CLASSVAR;
			const bool isExternal = symbol->flags & zPAR_FLAG_EXTERNAL;
			const bool isConst = symbol->flags & zPAR_FLAG_CONST;

			if (isClassVar)
				return Type::ClassVar;

			if (type == zPAR_TYPE_CLASS)
				return Type::Class;

			if (type != zPAR_TYPE_FUNC && isExternal)
				return Type::ExternalVar;

			if (type == zPAR_TYPE_FUNC && isExternal)
				return Type::ExternalFunc;

			if (type == zPAR_TYPE_INSTANCE && isConst)
				return Type::Instance;

			if (type == zPAR_TYPE_PROTOTYPE)
				return Type::Prototype;

			if (type == zPAR_TYPE_FUNC && isConst)
				return Type::Func;

			if (type == zPAR_TYPE_INSTANCE)
				return Type::VarInstance;

			if (type == zPAR_TYPE_FUNC || type == zPAR_TYPE_INT)
				return Type::VarInt;

			if (type == zPAR_TYPE_FLOAT)
				return Type::VarFloat;

			if (type == zPAR_TYPE_STRING)
				return Type::VarString;

			return Type::Unknown;
		}

		byte* TryGetValueAddress(int index, bool& isInt, bool& isFloat, bool& isString) const
		{
			isInt = isFloat = isString = false;
			bool isFail = false;

			if (!CheckRange(index, 1) || !CheckBaseAddress())
				isFail = true;

			zCPar_Symbol* symbol = GetSymbol();
			size_t size = sizeof(int);

			if (symbol && symbol->type == zPAR_TYPE_STRING)
				size = sizeof(zSTRING);

			byte* address = nullptr;

			switch (type)
			{
			case Type::ClassVar:
			case Type::ExternalVar:
				address = (type == Type::ClassVar) ? (byte*)zCPar_Symbol::instance_adr : (byte*)symbol->adr;
				address += (type == Type::ClassVar) ? symbol->offset : 0;
				address += size * index;

				if (symbol->type == zPAR_TYPE_INT || symbol->type == zPAR_TYPE_FUNC || symbol->type == zPAR_TYPE_INSTANCE)
				{
					isInt = true;
					break;
				}

				if (symbol->type == zPAR_TYPE_FLOAT)
				{
					isFloat = true;
					break;
				}

				if (symbol->type == zPAR_TYPE_STRING)
				{
					isString = true;
					break;
				}

				isFail = true;
				break;

			case Type::ExternalFunc:
			case Type::Instance:
			case Type::Prototype:
			case Type::Func:
				isInt = true;
				address = (byte*)&symbol->single_intdata;
				break;

			case Type::VarInstance:
				isInt = true;
				address = (byte*)&symbol->offset;
				break;

			case Type::VarString:
				isString = true;
				address = (byte*)&symbol->stringdata[index];
				break;

			case Type::VarFloat:
				isFloat = true;
				address = (symbol->ele == 1) ? (byte*)&symbol->single_floatdata : (byte*)&symbol->floatdata[index];
				break;

			case Type::VarInt:
				isInt = true;
				address = (symbol->ele == 1) ? (byte*)&symbol->single_intdata : (byte*)&symbol->intdata[index];
				break;

			case Type::DummyString:
				isString = true;
				address = (byte*)dummyString;
				break;

			case Type::DummyFloat:
				isFloat = true;
				address = (byte*)&dummyFloat;
				break;

			case Type::DummyInt:
				isInt = true;
				address = (byte*)&dummyInt;
				break;

			default:
				isFail = true;
				break;
			}

			return isFail ? nullptr : address;
		}

	public:
		CSymbol() :
			type{ },
			parser{ },
			index{ -1 },
			dummyString{ },
			isGlobal{ }
		{

		}

		CSymbol(zCParser* parser, int index) :
			CSymbol{ }
		{
			if (!parser || index < 0 || index >= parser->symtab.table.GetNum())
				return;

			this->parser = parser;
			this->type = GetType(parser->symtab.table[index]);
			this->index = index;
			this->isGlobal = !parser->symtab.table[index]->name.HasWord(".");
		}

		CSymbol(zCParser* parser, const string& name) :
			CSymbol{ parser, parser->GetIndex(name) }
		{

		}

		CSymbol(int dummyInt) :
			CSymbol{ }
		{
			this->type = Type::DummyInt;
			this->dummyInt = dummyInt;
		}

		CSymbol(float dummyFloat) :
			CSymbol{ }
		{
			this->type = Type::DummyFloat;
			this->dummyFloat = dummyFloat;
		}

		CSymbol(const string& dummyString) :
			CSymbol{ }
		{
			this->type = Type::DummyString;
			this->dummyString = new zSTRING(dummyString);
		}

		CSymbol(const CSymbol& right) :
			type{ right.type },
			parser{ right.parser },
			index{ right.index },
			dummyInt{ right.dummyInt },
			isGlobal{ right.isGlobal }
		{
			if (type == Type::DummyString)
				dummyString = new zSTRING(*right.dummyString);
		}

		CSymbol(CSymbol&& right) :
			type{ right.type },
			parser{ right.parser },
			index{ right.index },
			dummyInt{ right.dummyInt },
			isGlobal{ right.isGlobal }
		{
			right.type = Type::Unknown;
		}

		CSymbol& operator=(const CSymbol& right)
		{
			this->~CSymbol();

			type = right.type;
			parser = right.parser;
			index = right.index;
			dummyInt = right.dummyInt;
			isGlobal = right.isGlobal;

			if (type == Type::DummyString)
				dummyString = new zSTRING(*right.dummyString);

			return *this;
		}

		CSymbol& operator=(CSymbol&& right)
		{
			this->~CSymbol();

			type = right.type;
			parser = right.parser;
			index = right.index;
			dummyInt = right.dummyInt;
			isGlobal = right.isGlobal;
			right.type = Type::Unknown;

			return *this;
		}

		~CSymbol()
		{
			if (type == Type::DummyString)
				delete dummyString;
		}

		Type GetType() const { return type;	}
		bool IsGlobal() const {	return isGlobal; }
		int GetIndex() const { return index; }

		bool IsSpecialInstance() const
		{
			if (type != Type::Instance && type != Type::VarInstance)
				return false;

			if (parser != ::NAMESPACE::parser)
				return false;

			const zSTRING& name = (GetSymbol()->name);
			return name == "ITEM" || name == "HERO" || name == "SELF" || name == "OTHER" || name == "VICTIM";
		}

		template <class T>
		bool IsValueType() const
		{
			static_assert(std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, string>, "Bad type");

			bool isInt, isFloat, isString;
			TryGetValueAddress(0, isInt, isFloat, isString);

			if (isInt && std::is_same_v<T, int>)
				return true;

			if (isFloat && std::is_same_v<T, float>)
				return true;

			if (isString && std::is_same_v<T, string>)
				return true;

			return false;
		}

		bool CheckBaseAddress() const
		{
			if (type == Type::Unknown)
				return false;

			if (type == Type::ClassVar && !zCPar_Symbol::instance_adr)
				return false;

			if (type == Type::ExternalVar && GetSymbol()->type == zPAR_TYPE_INSTANCE)
				return true;

			if (type == Type::ExternalVar && !GetSymbol()->adr)
				return false;

			return true;
		}

		zCPar_Symbol* GetSymbol() const { return COA2(parser, GetSymbol(index)); }
		zCParser* GetParser() const	{ return parser; }

		int GetValuesCount() const
		{
			switch (type)
			{
			case Type::Class:
				return 0;

			case Type::ClassVar:
				if (GetSymbol()->type == zPAR_TYPE_INSTANCE)
					return 0;
				// fall down else

			case Type::ExternalVar:
			case Type::VarString:
			case Type::VarFloat:
			case Type::VarInt: 
				return (int)GetSymbol()->ele;

			case Type::ExternalFunc:
			case Type::Instance:
			case Type::Prototype:
			case Type::Func:
			case Type::VarInstance:
			case Type::DummyString:
			case Type::DummyFloat:
			case Type::DummyInt: 
				return 1;

			default:
				return 0;
			}
		}

		bool CheckRange(int start, int count) const
		{
			if (type == Type::Unknown || count < 0)
				return false;

			if (count == 0)
				return true;

			return start >= 0 && start + count <= GetValuesCount();
		}

		template <class T>
		T GetValue(int index) const
		{
			static_assert(std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, string>, "Bad type");

			bool isInt, isFloat, isString;
			byte* address = TryGetValueAddress(index, isInt, isFloat, isString);

			if constexpr (std::is_same_v<T, string>)
				return *reinterpret_cast<zSTRING*>(address);
			else
				return *reinterpret_cast<T*>(address);
		}

		template <class T>
		void SetValue(int index, const T& value)
		{
			static_assert(std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, string>, "Bad type");

			bool isInt, isFloat, isString;
			byte* address = TryGetValueAddress(index, isInt, isFloat, isString);

			if constexpr (std::is_same_v<T, string>)
				*reinterpret_cast<zSTRING*>(address) = value.GetVector();
			else
				*reinterpret_cast<T*>(address) = value;
		}

		bool TryPushAsParam(zCParser* target, int index, const CSymbol& param) const
		{
			zCPar_Symbol* paramSym = param.GetSymbol();
			
			if (!paramSym || !target || !CheckRange(index, 1))
				return false;

			bool isInt, isFloat, isString;
			byte* address = TryGetValueAddress(index, isInt, isFloat, isString); 
			if (!address)
				return false;

			switch (type)
			{
			case Type::ClassVar:
			case Type::ExternalVar:			
			case Type::VarString:
			case Type::VarFloat:
			case Type::VarInt:
			case Type::DummyString:
			case Type::DummyFloat:
			case Type::DummyInt:
				if (paramSym->type == zPAR_TYPE_STRING)
				{
					if (!isString)
						return false;

					target->SetReturn(*reinterpret_cast<zSTRING*>(address));
					return true;
				}

				if (paramSym->type == zPAR_TYPE_INT || paramSym->type == zPAR_TYPE_FUNC)
				{
					if (!isInt)
						return false;

					target->SetReturn(*reinterpret_cast<int*>(address));
					return true;
				}

				if (paramSym->type == zPAR_TYPE_FLOAT)
				{
					if (isInt)
					{
						target->SetReturn(static_cast<float>(*reinterpret_cast<int*>(address)));
						return true;
					}

					if (isFloat)
					{
						target->SetReturn(*reinterpret_cast<float*>(address));
						return true;
					}

					return false;
				}

				return false;

			case Type::Instance:
			case Type::VarInstance:
				if (parser != target)
					return false;

				if (paramSym->type == zPAR_TYPE_INT || paramSym->type == zPAR_TYPE_FUNC)
				{
					target->SetReturn(this->index);
					return true;
				}

				if (paramSym->type != zPAR_TYPE_INSTANCE)
					return false;

				target->datastack.Push(this->index);
				return true;

			case Type::Func:
				if (parser != target || paramSym->type != zPAR_TYPE_FUNC && paramSym->type != zPAR_TYPE_INT)
					return false;

				target->datastack.Push(this->index);
				return true;

			default:
				return false;
			}
		}

		bool CanPushAsParam(zCParser* target, int index, const CSymbol& param) const
		{
			if (!target)
				return false;

			auto scope = AssignTemp(target->datastack.sptr, target->datastack.sptr);
			return TryPushAsParam(target, index, param);
		}

		explicit operator bool() const
		{
			return GetType() != Type::Unknown;
		}
	};
}
