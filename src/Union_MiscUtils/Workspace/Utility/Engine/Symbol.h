namespace NAMESPACE
{
	class Symbol
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
			VarInt
		};

	private:
		Type type;
		zCParser* parser;
		int index;
		bool isGlobal;

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

		bool CheckRange(int start, int count) const
		{
			if (type == Type::Unknown || count < 0)
				return false;

			if (count == 0)
				return true;

			return start >= 0 && start + count <= GetValuesCount();
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

			default:
				isFail = true;
				break;
			}

			return isFail ? nullptr : address;
		}

	public:
		Symbol() :
			type{},
			parser{},
			index{ -1 },
			isGlobal{}
		{

		}

		Symbol(zCParser* parser, int index) :
			Symbol{ }
		{
			if (!parser || index < 0 || index >= parser->symtab.table.GetNum())
				return;

			this->parser = parser;
			this->type = GetType(parser->symtab.table[index]);
			this->index = index;
			this->isGlobal = !parser->symtab.table[index]->name.HasWord(".");
		}

		Symbol(zCParser* parser, const string& name) :
			Symbol{ parser, parser->GetIndex(name) }
		{

		}

		Symbol(const Symbol& right) :
			type{ right.type },
			parser{ right.parser },
			index{ right.index },
			isGlobal{ right.isGlobal }
		{

		}

		Symbol(Symbol&& right) :
			type{ right.type },
			parser{ right.parser },
			index{ right.index },
			isGlobal{ right.isGlobal }
		{
			right.type = Type::Unknown;
		}

		Symbol& operator=(const Symbol& right)
		{
			type = right.type;
			parser = right.parser;
			index = right.index;
			isGlobal = right.isGlobal;

			return *this;
		}

		Symbol& operator=(Symbol&& right)
		{
			type = right.type;
			parser = right.parser;
			index = right.index;
			isGlobal = right.isGlobal;
			right.type = Type::Unknown;

			return *this;
		}

		explicit operator bool() const
		{
			return type != Type::Unknown;
		}

		Type GetType() const { return type;	}
		bool IsGlobal() const {	return isGlobal; }
		int GetIndex() const { return index; }

		template <class T>
		bool IsType() const
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

		zCPar_Symbol* GetSymbol() const { return COA(parser, GetSymbol(index)); }
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
				return 1;

			default:
				return 0;
			}
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
	};
}
