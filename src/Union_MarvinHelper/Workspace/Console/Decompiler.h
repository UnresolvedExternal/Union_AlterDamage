// HELL CODE!!! DO NOT READ!!!

namespace NAMESPACE
{
	inline void AssertDec(bool result, int line, const char* expr)
	{
		if (result)
			return;

		string message = A line + ": " + expr;
		throw std::exception(message.GetVector());
	}

#define ASSERT_DEC(cond) AssertDec((cond), __LINE__, #cond);

	enum class TInstructionType
	{
		PushInt = zPAR_TOK_PUSHINT,
		PushFloat = zPAR_TOK_FLOAT,
		PushVar = zPAR_TOK_PUSHVAR,
		PushArrayVar = zPAR_TOK_PUSHVAR + zPAR_TOK_FLAGARRAY,
		PushInst = zPAR_TOK_PUSHINST,

		AssignString = zPAR_TOK_ASSIGNSTR,
		AssignStringPtr = zPAR_TOK_ASSIGNSTRP,
		AssignFunc = zPAR_TOK_ASSIGNFUNC,
		AssignFloat = zPAR_TOK_ASSIGNFLOAT,
		AssignInt = zPAR_OP_IS,
		AssignPlus = zPAR_OP_ISPLUS,
		AssignMinus = zPAR_OP_ISMINUS,
		AssignMul = zPAR_OP_ISMUL,
		AssignDiv = zPAR_OP_ISDIV,
		AssignInst = zPAR_TOK_ASSIGNINST,

		Plus = zPAR_OP_PLUS,
		Minus = zPAR_OP_MINUS,
		Mul = zPAR_OP_MUL,
		Div = zPAR_OP_DIV,
		Mod = zPAR_OP_MOD,
		ShiftLeft = zPAR_OP_SHIFTL,
		ShiftRight = zPAR_OP_SHIFTR,

		BitOr = zPAR_OP_OR,
		BitAnd = zPAR_OP_AND,
		BitNot = zPAR_OP_UN_NEG,

		Less = zPAR_OP_LOWER,
		More = zPAR_OP_HIGHER,
		LessEqual = zPAR_OP_LOWER_EQ,
		Equal = zPAR_OP_EQUAL,
		NotEqual = zPAR_OP_NOTEQUAL,
		MoreEqual = zPAR_OP_HIGHER_EQ,

		LogOr = zPAR_OP_LOG_OR,
		LogAnd = zPAR_OP_LOG_AND,
		LogNot = zPAR_OP_UN_NOT,

		UnaryMinus = zPAR_OP_UN_MINUS,
		UnaryPlus = zPAR_OP_UN_PLUS,

		Call = zPAR_TOK_CALL,
		CallExternal = zPAR_TOK_CALLEXTERN,

		Return = zPAR_TOK_RET,
		Jump = zPAR_TOK_JUMP,
		JumpFalse = zPAR_TOK_JUMPF,

		SetInstance = zPAR_TOK_SETINSTANCE,
	};

	struct TInstruction
	{
		int address;
		int size;
		TInstructionType token;
		int argument;
		byte index;
		int arity;
		int prio;
		bool ret;

		void Read(zCParser* parser, int address, bool isReturn)
		{
			this->address = address;
			this->token = (TInstructionType)parser->stack.stack[address++];

			switch (this->token)
			{
			case TInstructionType::PushInt:
				this->size = 5;
				this->argument = *(int*)&parser->stack.stack[address];
				this->index = 0;
				this->arity = 0;
				this->prio = 99;
				this->ret = true;
				break;

			case TInstructionType::PushVar:
				this->size = 5;
				this->argument = *(int*)&parser->stack.stack[address];
				this->index = 0;
				this->arity = 0;
				this->prio = 99;
				this->ret = true;
				break;

			case TInstructionType::PushArrayVar:
				this->size = 6;
				this->argument = *(int*)&parser->stack.stack[address];
				this->index = *(byte*)&parser->stack.stack[address + 4];
				this->arity = 0;
				this->prio = 99;
				this->ret = true;
				break;

			case TInstructionType::PushInst:
				this->size = 5;
				this->argument = *(int*)&parser->stack.stack[address];
				this->index = 0;
				this->arity = 0;
				this->prio = 99;
				this->ret = true;
				break;

			case TInstructionType::AssignString:
			case TInstructionType::AssignStringPtr:
			case TInstructionType::AssignFunc:
			case TInstructionType::AssignFloat:
			case TInstructionType::AssignInt:
			case TInstructionType::AssignPlus:
			case TInstructionType::AssignMinus:
			case TInstructionType::AssignMul:
			case TInstructionType::AssignDiv:
			case TInstructionType::AssignInst:
				this->size = 1;
				this->argument = 0;
				this->index = 0;
				this->arity = 2;
				this->prio = 0;
				this->ret = false;
				break;

			case TInstructionType::Plus:
			case TInstructionType::Minus:
			case TInstructionType::ShiftLeft:
			case TInstructionType::ShiftRight:
			case TInstructionType::BitOr:
			case TInstructionType::BitAnd:
			case TInstructionType::LogOr:
				this->size = 1;
				this->argument = 0;
				this->index = 0;
				this->arity = 2;
				this->prio = 2;
				this->ret = true;
				break;

			case TInstructionType::Mul:
			case TInstructionType::Div:
			case TInstructionType::Mod:
			case TInstructionType::LogAnd:
				this->size = 1;
				this->argument = 0;
				this->index = 0;
				this->arity = 2;
				this->prio = 3;
				this->ret = true;
				break;
			
			case TInstructionType::Less:
			case TInstructionType::More:
			case TInstructionType::LessEqual:
			case TInstructionType::Equal:
			case TInstructionType::NotEqual:
			case TInstructionType::MoreEqual:
				this->size = 1;
				this->argument = 0;
				this->index = 0;
				this->arity = 2;
				this->prio = 1;
				this->ret = true;
				break;

			case TInstructionType::BitNot:
			case TInstructionType::LogNot:
				this->size = 1;
				this->argument = 0;
				this->index = 0;
				this->arity = 1;
				this->prio = 4;
				this->ret = true;
				break;

			case TInstructionType::UnaryMinus:
			case TInstructionType::UnaryPlus:
				this->size = 1;
				this->argument = 0;
				this->index = 0;
				this->arity = 1;
				this->prio = 1;
				this->ret = true;
				break;

			case TInstructionType::Call:
			{
				this->size = 5;
				this->argument = *(int*)&parser->stack.stack[address];
				this->index = 0;
				zCPar_Symbol* func = parser->SearchFuncWithStartAddress(this->argument);
				ASSERT_DEC(func);
				this->arity = func->ele;
				this->prio = 99;
				this->ret = func->HasFlag(zPAR_FLAG_RETURN);
				break;
			}

			case TInstructionType::CallExternal:
				this->size = 5;
				this->argument = *(int*)&parser->stack.stack[address];
				this->index = 0;
				this->arity = parser->GetSymbol(this->argument)->ele;
				this->prio = 99;
				this->ret = parser->GetSymbol(this->argument)->HasFlag(zPAR_FLAG_RETURN);
				break;

			case TInstructionType::Return:
				this->size = 1;
				this->argument = 0;
				this->index = 0;
				this->arity = isReturn;
				this->prio = 0;
				this->ret = false;
				break;

			case TInstructionType::Jump:
				this->size = 5;
				this->argument = *(int*)&parser->stack.stack[address];
				this->index = 0;
				this->arity = 0;
				this->prio = 0;
				this->ret = false;
				break;

			case TInstructionType::JumpFalse:
				this->size = 5;
				this->argument = *(int*)&parser->stack.stack[address];
				this->index = 0;
				this->arity = 1;
				this->prio = 0;
				this->ret = true;
				break;

			case TInstructionType::SetInstance:
				this->size = 5;
				this->argument = *(int*)&parser->stack.stack[address];
				this->index = 0;
				this->arity = 0;
				this->prio = 0;
				this->ret = false;
				break;

			default:
				ASSERT_DEC(false);
				break;
			}
		}

		std::string ToString()
		{
			std::ostringstream out;
			out << std::setw(8) << address;

#define TOKEN_NAME(name) case TInstructionType::name: out << std::setw(24) << #name; break;

			switch (token)
			{
			TOKEN_NAME(PushInt)
			TOKEN_NAME(PushFloat)
			TOKEN_NAME(PushVar)
			TOKEN_NAME(PushArrayVar)
			TOKEN_NAME(PushInst)
			TOKEN_NAME(AssignString)
			TOKEN_NAME(AssignStringPtr)
			TOKEN_NAME(AssignFunc)
			TOKEN_NAME(AssignFloat)
			TOKEN_NAME(AssignInt)
			TOKEN_NAME(AssignPlus)
			TOKEN_NAME(AssignMinus)
			TOKEN_NAME(AssignMul)
			TOKEN_NAME(AssignDiv)
			TOKEN_NAME(AssignInst)
			TOKEN_NAME(Plus)
			TOKEN_NAME(Minus)
			TOKEN_NAME(Mul)
			TOKEN_NAME(Div)
			TOKEN_NAME(Mod)
			TOKEN_NAME(ShiftLeft)
			TOKEN_NAME(ShiftRight)
			TOKEN_NAME(BitOr)
			TOKEN_NAME(BitAnd)
			TOKEN_NAME(BitNot)
			TOKEN_NAME(Less)
			TOKEN_NAME(More)
			TOKEN_NAME(LessEqual)
			TOKEN_NAME(Equal)
			TOKEN_NAME(NotEqual)
			TOKEN_NAME(MoreEqual)
			TOKEN_NAME(LogOr)
			TOKEN_NAME(LogAnd)
			TOKEN_NAME(LogNot)
			TOKEN_NAME(UnaryMinus)
			TOKEN_NAME(UnaryPlus)
			TOKEN_NAME(Call)
			TOKEN_NAME(CallExternal)
			TOKEN_NAME(Return)
			TOKEN_NAME(Jump)
			TOKEN_NAME(JumpFalse)
			TOKEN_NAME(SetInstance)
			default: ASSERT(false); break;
			}

#undef TOKEN_NAME

			out << std::setw(16) << argument;
			out << std::setw(8) << static_cast<int>(index);

			return out.str();
		}
	};

	enum class TNodeType
	{
		Unknown,
		Function,
		If,
		While,
		Group,
		DummyGroup,
		Instruction
	};

	class CAstNode
	{
	public:
		CAstNode* parent;
		std::vector<std::unique_ptr<CAstNode>> childs;

		TNodeType type;
		TInstruction instruction;
		int addressLimit;

		CAstNode()
		{
			parent = nullptr;
			type = TNodeType::Unknown;
			addressLimit = std::numeric_limits<int>::max();
		}

		bool HasValidInstruction()
		{
			switch (type)
			{
			case TNodeType::Instruction:
				return true;

			case TNodeType::If:
			case TNodeType::While:
			case TNodeType::Group:
			case TNodeType::DummyGroup:
				return false;

			default:
				ASSERT_DEC(false);
				return false;
			}
		}

		int GetMinAddress()
		{
			int address = std::numeric_limits<int>::max();
			for (const auto& child : childs)
				address = std::min(address, child->GetMinAddress());

			if (HasValidInstruction())
				address = std::min(address, instruction.address);
			
			return address;
		}

		int GetMaxAddress()
		{
			int address = std::numeric_limits<int>::min();
			for (const auto& child : childs)
				address = std::max(address, child->GetMinAddress());

			if (HasValidInstruction())
				address = std::max(address, instruction.address + instruction.size - 1);

			return address;
		}

		std::unique_ptr<CAstNode> PopChild()
		{
			for (int i = (int)childs.size() - 1; i >= 0; i--)
			{
				if (childs[i]->type == TNodeType::Instruction && childs[i]->instruction.ret)
				{
					auto child = std::move(childs[i]);
					childs.erase(childs.begin() + i);
					child->parent = nullptr;
					return child;
				}
			}

			return { };
		}

		std::unique_ptr<CAstNode>& CreateChild(int arity = 0)
		{
			std::vector<std::unique_ptr<CAstNode>> operands;

			for (int i = 0; i < arity; i++)
				operands.push_back(PopChild());

			childs.push_back(std::make_unique<CAstNode>());
			childs.back()->parent = this;
			childs.back()->addressLimit = std::min(childs.back()->addressLimit, this->addressLimit);

			for (auto& operand : operands)
				childs.back()->AddChild(std::move(operand));

			return childs.back();
		}

		std::unique_ptr<CAstNode>& AddChild(std::unique_ptr<CAstNode>&& child)
		{
			ASSERT_DEC(child != nullptr);
			childs.push_back(std::move(child));
			childs.back()->parent = this;
			childs.back()->addressLimit = std::min(childs.back()->addressLimit, this->addressLimit);
			return childs.back();
		}

		void LimitAddress(int limit)
		{
			if (limit < this->addressLimit)
			{
				this->addressLimit = limit;
				for (auto& child : childs)
					child->LimitAddress(limit);
			}
		}

		bool IsTopLevelGroup()
		{
			return type == TNodeType::Group && parent->type == TNodeType::Function;
		}

		int GetNesting()
		{
			int nesting = 0;

			for (CAstNode* node = parent; node; node = node->parent)
				if (node->type == TNodeType::Group)
					nesting += 1;

			return nesting;
		}
	};

	void BuildAstImpl(const CSymbol& func, CAstNode* opGroup, int& address)
	{
		while (address < opGroup->addressLimit)
		{
			TInstruction instruction;
			instruction.Read(func.GetParser(), address, func.GetSymbol()->HasFlag(zPAR_FLAG_RETURN));
			address += instruction.size;

			switch (instruction.token)
			{
			case TInstructionType::Jump:
			{
				ASSERT_DEC(opGroup->parent);
				ASSERT_DEC(opGroup->parent->type == TNodeType::If || opGroup->parent->type == TNodeType::While);

				if (instruction.argument == opGroup->parent->childs.at(0)->GetMinAddress())
				{
					opGroup->parent->type = TNodeType::While;
					break;
				}

				if (instruction.argument >= opGroup->parent->addressLimit && opGroup->parent->childs.size() == 2)
				{
					ASSERT_DEC(opGroup->parent->type == TNodeType::If);
					opGroup->parent->addressLimit = std::max(opGroup->parent->addressLimit, instruction.argument);
					break;
				}

				ASSERT_DEC(false);
				break;
			}

			case TInstructionType::JumpFalse:
			{
				opGroup->parent->addressLimit = std::max(opGroup->parent->addressLimit, instruction.argument);

				auto& ifNode = opGroup->CreateChild(1);
				ifNode->type = TNodeType::If;
				ifNode->LimitAddress(instruction.argument);

				auto& thenGroup = ifNode->CreateChild();
				thenGroup->type = TNodeType::Group;
				BuildAstImpl(func, thenGroup.get(), address);

				if (address < ifNode->addressLimit)
				{
					ASSERT_DEC(ifNode->type == TNodeType::If);
					auto& elseGroup = ifNode->CreateChild();
					elseGroup->type = TNodeType::Group;
					BuildAstImpl(func, elseGroup.get(), address);
				}

				break;
			}

			case TInstructionType::Return:
			{
				std::unique_ptr<CAstNode> arg;
				if (instruction.arity)
					arg = std::move(opGroup->PopChild());

				if (!arg && opGroup->IsTopLevelGroup())
					return;

				auto& retNode = opGroup->CreateChild();
				retNode->type = TNodeType::Instruction;
				retNode->instruction = instruction;

				if (arg)
					retNode->AddChild(std::move(arg));

				if (opGroup->IsTopLevelGroup())
					return;

				break;
			}

			default:
			{
				auto& opNode = opGroup->CreateChild(instruction.arity);
				opNode->instruction = instruction;
				opNode->type = TNodeType::Instruction;
				break;
			}

			}
		}
	}

	void RemovePops(CAstNode* root)
	{
		if (!root || !root->childs.size() || root->childs.front()->type != TNodeType::Group)
			return;

		CAstNode * opGroup = root->childs.front().get();

		for (size_t i = 0; i < opGroup->childs.size(); )
		{
			auto& child = opGroup->childs.at(i);

			if (child->type == TNodeType::Instruction && child->childs.size() == 2)
			{
				switch (child->instruction.token)
				{
				case TInstructionType::AssignInt:
				case TInstructionType::AssignInst:
				case TInstructionType::AssignFunc:
				case TInstructionType::AssignString:
				case TInstructionType::AssignFloat:
				{
					auto& left = child->childs.front();
					auto& right = child->childs.back();

					if
						(
							left->type == right->type &&
							left->type == TNodeType::Instruction &&
							(left->instruction.token == TInstructionType::PushVar || left->instruction.token == TInstructionType::PushInst) &&
							(right->instruction.token == TInstructionType::PushVar || right->instruction.token == TInstructionType::PushInst) &&
							left->instruction.argument == right->instruction.argument
							)
					{
						opGroup->childs.erase(opGroup->childs.begin() + i);
						break;
					}

					i += 1;
					break;
				}

				default:
					i += 1;
					break;
				}
			}
			else
				i += 1;
		}
	}

	void RemoveBaseCall(CAstNode* root, zCPar_Symbol* base)
	{
		if (!base)
			return;

		if (!root || !root->childs.size() || root->childs.front()->type != TNodeType::Group)
			return;

		auto& opGroup = root->childs.front();
		if (opGroup->childs.empty())
			return;

		auto& opCall = opGroup->childs.front();
		if (opCall->type != TNodeType::Instruction || opCall->instruction.token != TInstructionType::Call)
			return;

		if (opCall->instruction.argument != base->single_intdata)
			return;

		opGroup->childs.erase(opGroup->childs.begin());
	}

	std::unique_ptr<CAstNode> BuildAst(const CSymbol& func)
	{
		ASSERT_DEC(func.GetType() == CSymbol::Type::Func || func.GetType() == CSymbol::Type::Prototype || func.GetType() == CSymbol::Type::Instance);

		auto root = std::make_unique<CAstNode>();
		root->type = TNodeType::Function;

		auto& opGroup = root->CreateChild();
		opGroup->type = TNodeType::Group;

		int address = func.GetValue<int>(0);

		for (int i = 0; i < (int)func.GetSymbol()->ele; i++)
		{
			auto& opPush = opGroup->CreateChild();
			opPush->type = TNodeType::Instruction;

			opPush->instruction.address = address;
			opPush->instruction.argument = func.GetIndex() + i + 1;
			opPush->instruction.arity = 0;
			opPush->instruction.index = 0;
			opPush->instruction.size = 0;
			opPush->instruction.ret = true;
			opPush->instruction.token = TInstructionType::PushVar;

			CSymbol::Type type = CSymbol(func.GetParser(), func.GetIndex() + i).GetType();
			if (type == CSymbol::Type::Instance || type == CSymbol::Type::VarInstance || type == CSymbol::Type::Prototype)
				opPush->instruction.token = TInstructionType::PushInst;
		}

		BuildAstImpl(func, opGroup.get(), address);
		RemovePops(root.get());
		RemoveBaseCall(root.get(), func.GetSymbol()->parent);
		return std::move(root);
	}

	std::string TypeToString(int type)
	{
		switch (type)
		{
		case zPAR_TYPE_VOID:
			return "void";
		case zPAR_TYPE_INT:
			return "int";
		case zPAR_TYPE_FLOAT:
			return "float";
		case zPAR_TYPE_FUNC:
			return "func";
		case zPAR_TYPE_INSTANCE:
			return "instance";
		case zPAR_TYPE_STRING:
			return "string";

		default:
			ASSERT_DEC(false);
			return "";
		}
	}

	std::string GetShortName(const CSymbol& symbol)
	{
		zSTRING& name = symbol.GetSymbol()->name;

		if (symbol.IsGlobal())
			return name.ToChar();

		int start = name.Search(".", 0, true);
		return (A name).Copy(start, name.Length() - start).GetVector();
	}

	void EmitTabs(std::ostream& out, CAstNode* node)
	{
		out << std::string(node->GetNesting(), '\t');
	}

	void EmitSignature(std::ostream& out, const CSymbol& symbol)
	{
		switch (symbol.GetType())
		{
		case CSymbol::Type::Func:
			out << "func " << TypeToString(symbol.GetSymbol()->offset) << " " << symbol.GetSymbol()->name.ToChar() << "(";

			for (int i = 0; i < (int)symbol.GetSymbol()->ele; i++)
			{
				if (i)
					out << ", ";

				EmitSignature(out, CSymbol(symbol.GetParser(), symbol.GetIndex() + i + 1));
			}

			out << ")";
			break;

		case CSymbol::Type::Prototype:
		{
			out << "prototype " << symbol.GetSymbol()->name.ToChar() << "(";
			zCPar_Symbol* base = symbol.GetSymbol()->GetParent();
			if (base)
				out << base->name.ToChar();
			out << ")";
			break;
		}

		case CSymbol::Type::Instance:
		{
			out << "instance " << symbol.GetSymbol()->name.ToChar() << "(";
			zCPar_Symbol* base = symbol.GetSymbol()->GetParent();
			if (base)
				out << base->name.ToChar();
			out << ")";
			break;
		}

		case CSymbol::Type::VarFloat:
		case CSymbol::Type::VarInt:
		case CSymbol::Type::VarString:
			out << "var " << TypeToString(symbol.GetSymbol()->type) << " " << GetShortName(symbol);
			break;

		case CSymbol::Type::VarInstance:
		{
			out << "var ";

			int base = symbol.GetParser()->GetBaseClass(symbol.GetSymbol());

			if (base == -1)
				out << "instance ";
			else
				out << symbol.GetParser()->GetSymbol(base)->name.ToChar() << " ";

			out << GetShortName(symbol);
			break;
		}

		default:
			ASSERT_DEC(false);
			break;
		}
	}

	void EmitCode(std::ostream& out, const CSymbol& func, CAstNode* node, CSymbol& inst);

	void EmitBinary(std::ostream& out, const CSymbol& func, CAstNode* node, CSymbol& inst, const std::string& op, bool brace = false)
	{
		if (brace)
			out << "(";

		EmitCode(out, func, node->childs.at(0).get(), inst);
		out << " " << op << " ";
		EmitCode(out, func, node->childs.at(1).get(), inst);

		if (brace)
			out << ")";
	}

	void EmitCode(std::ostream& out, const CSymbol& func, CAstNode* node, CSymbol& inst)
	{
		bool suppressInstruction = false;
		bool brace = false;

		switch (node->type)
		{
		case TNodeType::Function:
			EmitSignature(out, func);
			out << std::endl;
			EmitCode(out, func, node->childs.at(0).get(), inst);
			break;

		case TNodeType::Group:
			EmitTabs(out, node);
			out << "{" << std::endl;

			if (node->IsTopLevelGroup())
			{
				bool any = false;
				for (CSymbol local(func.GetParser(), func.GetIndex() + func.GetSymbol()->ele + 1); local.GetSymbol(); local = CSymbol(local.GetParser(), local.GetIndex() + 1))
				{
					if (!(A local.GetSymbol()->name).StartWith(func.GetSymbol()->name + "."))
						break;

					EmitTabs(out, node);
					out << '\t';

					EmitSignature(out, local);
					out << ";" << std::endl;
					any = true;
				}

				if (any)
					out << std::endl;
			}

			for (auto& child : node->childs)
				EmitCode(out, func, child.get(), inst);

			EmitTabs(out, node);
			out << "}";
			break;

		case TNodeType::DummyGroup:
			for (auto& child : node->childs)
				EmitCode(out, func, child.get(), inst);
			break;

		case TNodeType::While:
			EmitTabs(out, node);
			out << "while ";
			EmitCode(out, func, node->childs.at(0).get(), inst);
			out << std::endl;
			EmitCode(out, func, node->childs.at(1).get(), inst);
			break;

		case TNodeType::If:
			if (node->parent->type != TNodeType::DummyGroup)
				EmitTabs(out, node);

			out << "if ";
			EmitCode(out, func, node->childs.at(0).get(), inst);
			out << std::endl;
			EmitCode(out, func, node->childs.at(1).get(), inst);

			if (node->childs.size() == 3)
			{
				if (node->childs.at(2)->childs.size() == 1 && node->childs.at(2)->childs.front()->type == TNodeType::If)
				{
					node->childs.at(2)->type = TNodeType::DummyGroup;
					out << " else ";
					EmitCode(out, func, node->childs.at(2).get(), inst);
					break;
				}

				out << std::endl;
				EmitTabs(out, node);
				out << "else" << std::endl;
				EmitCode(out, func, node->childs.at(2).get(), inst);
			}

			break;

		case TNodeType::Instruction:
			if (node->parent->type == TNodeType::Group && (node->type != TNodeType::Instruction || node->instruction.token != TInstructionType::Return))
				if (node->instruction.token != TInstructionType::SetInstance)
					EmitTabs(out, node);

			brace = node->parent->type == TNodeType::Instruction;
			brace = brace && node->parent->instruction.prio > node->instruction.prio;
			brace = brace && node->parent->instruction.token != TInstructionType::Call && node->parent->instruction.token != TInstructionType::CallExternal;

			switch (node->instruction.token)
			{
			case TInstructionType::PushFloat:
				out << std::fixed << reinterpret_cast<const float&>(node->instruction.argument);
				break;

			case TInstructionType::PushInt:
				if (node->parent->type == TNodeType::Instruction && node->parent->instruction.token == TInstructionType::AssignFloat)
				{
					out << std::fixed << reinterpret_cast<const float&>(node->instruction.argument);
					break;
				}

				if (node->parent->type == TNodeType::Instruction && node->parent->instruction.token == TInstructionType::AssignFunc)
				{
					out << GetShortName(CSymbol(func.GetParser(), node->instruction.argument));
					break;
				}

				out << node->instruction.argument;
				break;

			case TInstructionType::PushInst:
			case TInstructionType::PushVar:
			{
				CSymbol var(func.GetParser(), node->instruction.argument);

				if (var.GetSymbol() && var.GetType() == CSymbol::Type::VarString && var.GetSymbol()->name.First() == (char)255)
				{
					out << "\"" << var.GetValue<string>(0) << "\"";
					break;
				}

				if (var.GetSymbol()->HasFlag(zPAR_FLAG_CLASSVAR) && inst.GetIndex() != func.GetIndex())
					out << GetShortName(inst) << ".";

				if (var.GetIndex() == func.GetIndex() && (func.GetType() == CSymbol::Type::Prototype || func.GetType() == CSymbol::Type::Instance))
					out << "SELF";
				else
					out << GetShortName(var);

				if (var.GetSymbol()->ele > 1)
					out << "[0]";

				break;
			}

			case TInstructionType::PushArrayVar:
			{
				CSymbol var(func.GetParser(), node->instruction.argument);

				if (var.GetSymbol()->HasFlag(zPAR_FLAG_CLASSVAR) && inst.GetIndex() != func.GetIndex())
					out << GetShortName(inst) << ".";

				out << GetShortName(var) << "[" << (int)node->instruction.index << "]";
				break;
			}

			case TInstructionType::AssignString:
			case TInstructionType::AssignFloat:
			case TInstructionType::AssignFunc:
			case TInstructionType::AssignInt:
			case TInstructionType::AssignInst:
				EmitBinary(out, func, node, inst, "=");
				break;

			case TInstructionType::AssignStringPtr:
				out << "^";
				EmitCode(out, func, node->childs.at(0).get(), inst);
				out << " = ";
				out << "^";
				EmitCode(out, func, node->childs.at(1).get(), inst);
				break;

			case TInstructionType::AssignPlus:
				EmitBinary(out, func, node, inst, "+=");
				break;

			case TInstructionType::AssignMinus:
				EmitBinary(out, func, node, inst, "-=");
				break;

			case TInstructionType::AssignMul:
				EmitBinary(out, func, node, inst, "*=");
				break;

			case TInstructionType::AssignDiv:
				EmitBinary(out, func, node, inst, "/=");
				break;

			case TInstructionType::Plus:
			case TInstructionType::Minus:
			case TInstructionType::Mul:
			case TInstructionType::Div:
			case TInstructionType::Mod:
			case TInstructionType::ShiftLeft:
			case TInstructionType::ShiftRight:
			case TInstructionType::BitOr:
			case TInstructionType::BitAnd:
			case TInstructionType::Less:
			case TInstructionType::More:
			case TInstructionType::LessEqual:
			case TInstructionType::Equal:
			case TInstructionType::NotEqual:
			case TInstructionType::MoreEqual:
			case TInstructionType::LogOr:
			case TInstructionType::LogAnd:
			{
				zSTRING op;
				parser->GetOperatorString((int)node->instruction.token, op);
				op.Replace(" ", "");
				EmitBinary(out, func, node, inst, op.ToChar(), brace);
				break;
			}

			case TInstructionType::BitNot:
			case TInstructionType::LogNot:
			case TInstructionType::UnaryMinus:
			case TInstructionType::UnaryPlus:
			{
				zSTRING op;
				parser->GetOperatorString((int)node->instruction.token, op);
				op.Replace(" ", "");
				if (brace)
					out << "(";
				out << op.ToChar();
				EmitCode(out, func, node->childs.front().get(), inst);
				if (brace)
					out << ")";
				break;
			}

			case TInstructionType::Call:
			case TInstructionType::CallExternal:
			{
				CSymbol callee;

				if (node->instruction.token == TInstructionType::Call)
					callee = CSymbol(func.GetParser(), func.GetParser()->SearchFuncWithStartAddress(node->instruction.argument)->name);
				else
					callee = CSymbol(func.GetParser(), node->instruction.argument);

				out << callee.GetSymbol()->name.ToChar() << "(";

				for (size_t i = 0; i < node->childs.size(); i++)
				{
					if (i)
						out << ", ";

					auto& arg = node->childs.at(node->childs.size() - i - 1);

					if 
					(
						CSymbol(callee.GetParser(), callee.GetIndex() + i + 1).GetType() == CSymbol::Type::VarFloat &&
						arg->type == TNodeType::Instruction &&
						arg->instruction.token == TInstructionType::PushInt
					)
					{
						arg->instruction.token = TInstructionType::PushFloat;
					}

					EmitCode(out, func, arg.get(), inst);
				}

				out << ")";

				if (callee.GetSymbol()->name.Compare("AI_OUTPUT") && node->childs.size())
				{
					auto& name = node->childs.at(0);
					if (name->type == TNodeType::Instruction && name->instruction.token == TInstructionType::PushVar)
					{
						CSymbol var(func.GetParser(), name->instruction.argument);
						if (var.GetType() == CSymbol::Type::VarString && var.GetSymbol()->name.First() == (char)255)
						{
							zSTRING dia = var.GetValue<string>(0);
							int ouNum = ogame->GetCutsceneManager()->LibValidateOU(dia);
							if (ouNum > 0)
							{
								suppressInstruction = true;
								out << ";\t//";
								auto cs = (ZOwner<zCCSCutsceneContext>)ogame->GetGameWorld()->csPlayer->ownerManager->CreateNewOutputUnit(ouNum);
								zTCSBlockPosition pos = cs->GetFirstBlockPosition();
								auto msg = cs->GetNextMessage(pos);
								while (msg)
								{
									auto convMsg = dynamic_cast<oCMsgConversation*>(msg);
									if (convMsg && convMsg->subType == oCMsgConversation::EV_PLAYANISOUND)
									{
										out << convMsg->text.ToChar() << std::endl;
										break;
									}
									msg = cs->GetNextMessage(pos);
								}
							}
						}
					}
				}

				break;
			}

			case TInstructionType::Return:
				if (node->childs.empty() && node->parent->IsTopLevelGroup())
				{
					suppressInstruction = true;
					break;
				}

				EmitTabs(out, node);
				out << "return";
				
				if (node->childs.size())
				{
					out << " ";
					EmitCode(out, func, node->childs.front().get(), inst);
				}

				break;

			case TInstructionType::Jump:
				ASSERT_DEC(false);
				break;

			case TInstructionType::JumpFalse:
				EmitCode(out, func, node->childs.front().get(), inst);
				break;

			case TInstructionType::SetInstance:
				inst = CSymbol(func.GetParser(), node->instruction.argument);
				suppressInstruction = true;
				break;

			default:
				ASSERT_DEC(false);
				break;
			}

			break;

		default:
			ASSERT_DEC(false);
			break;
		}

		if (suppressInstruction)
			return;

		if (!node->parent || node->parent->type == TNodeType::Group)
			out << ";" << std::endl;
	}
}