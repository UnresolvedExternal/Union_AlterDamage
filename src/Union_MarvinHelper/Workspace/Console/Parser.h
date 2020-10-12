#pragma once

struct TParsedCommand
{
	enum TState
	{
		Initial,
		Space,
		Word,
		String,
		StringBackslash,
		StringEnd,
		Fail
	};

	std::vector<string> tokens;
	TState state;

	int GetWordNumTyping() const
	{
		if (state == TState::Fail || state != TState::Initial && state != TState::Word && state != TState::Space)
			return -1;

		return tokens.size() - 1 + ((state == TState::Initial || state == TState::Space) ? 1 : 0);
	}

	string GetWordTyping() const
	{
		int num = GetWordNumTyping();
		if (num == -1 || num >= (int)tokens.size())
			return "";
		return tokens[tokens.size() - 1];
	}

	void Parse(const string& line)
	{
		tokens.clear();
		state = TState::Initial;
		string* token = nullptr;

		for (const char* chars = line.GetVector(); *chars; chars++)
		{
			const char ch = *chars;
			switch (state)
			{
			case TState::Initial:
				if (ch == '"')
				{
					state = TState::Fail;
					return;
				}
				if (ch == ' ')
				{
					state = TState::Space;
					break;
				}
				tokens.push_back(ch);
				token = &tokens.back();
				state = TState::Word;
				break;

			case TState::Space:
				if (ch == ' ')
					break;
				tokens.push_back(ch);
				token = &tokens.back();
				state = (ch == '"') ? TState::String : TState::Word;
				break;

			case TState::Word:
				if (ch == ' ')
					state = TState::Space;
				else
					*token += ch;
				break;

			case TState::String:
				if (ch == '\\')
				{
					state = TState::StringBackslash;
					break;
				}
				if (ch == '"')
					state = TState::StringEnd;
				*token += ch;
				break;

			case TState::StringBackslash:
				if (ch != '\\' && ch != '"')
				{
					state = TState::Fail;
					return;
				}
				state = TState::String;
				*token += ch;
				break;

			case TState::StringEnd:
				if (ch != ' ')
				{
					state = TState::Fail;
					return;
				}
				state = TState::Space;
				break;
			}
		}
	}

	string ToString() const
	{
		string s;

		for (const string& token : tokens)
		{
			if (s.Length())
				s += " ";
			s += token;
		}

		return s;
	}
};

struct TParsedVarName
{
	enum class TState
	{
		Initial,
		Name,
		StartIndexInitial,
		StartIndex,
		EndIndexInitial,
		EndIndex,
		Stop,
		Fail
	};

	TState state;
	string name;
	int startIndex;
	int endIndex;

	void Parse(const string& varName)
	{
		state = TState::Initial;
		name = "";
		startIndex = -1;
		endIndex = -1;

		for (int i = 0; i < (int)varName.Length() && state != TState::Fail; i++)
			PutNext(varName[i]);
	}

	bool IsCompleted() const
	{
		return state == TState::Name || state == TState::Stop;
	}

	bool IsIndexed() const
	{
		switch (state)
		{
		case TParsedVarName::TState::StartIndexInitial:
		case TParsedVarName::TState::StartIndex:
		case TParsedVarName::TState::EndIndexInitial:
		case TParsedVarName::TState::EndIndex:
		case TParsedVarName::TState::Stop:
			return true;

		default:
			return false;
		}
	}

	bool IsFail() const
	{
		return state == TState::Fail;
	}

	bool TryAddIndexedHints(std::vector<string>& hints, const std::pair<int, int>& maxRange, const int& maxCount) const
	{
		switch (state)
		{
		case TState::StartIndexInitial:
		{
			for (int i = maxRange.first; i <= maxRange.second; i++)
				hints.push_back(name + "(" + i + ")");
			
			return true;
		}

		case TState::StartIndex:
		{
			for (int i = maxRange.first; i <= maxRange.second; i++)
				if ((A i).StartWith(startIndex))
					hints.push_back(name + "(" + i + ")");
			
			return true;
		}

		case TState::EndIndexInitial:
		{
			int start = std::max(startIndex, maxRange.first);
			int end = std::min(start + maxCount - 1, maxRange.second);

			for (int i = start; i <= end; i++)
				hints.push_back(name + "(" + startIndex + "-" + i + ")");
			
			return true;
		}

		case TState::EndIndex:
		{
			int start = std::max(startIndex, maxRange.first);
			int end = std::min(start + maxCount - 1, maxRange.second);

			for (int i = start; i <= end; i++)
				if ((A i).StartWith(endIndex))
					hints.push_back(name + "(" + startIndex + "-" + i + ")");

			return true;
		}

		case TState::Stop:
		{
			if (startIndex >= maxRange.first && startIndex <= maxRange.second && endIndex >= startIndex && endIndex <= maxRange.second && endIndex - startIndex + 1 <= maxCount)
				hints.push_back(name + "(" + startIndex + "-" + endIndex + ")");
			
			return true;
		}

		default:
			return false;
		}
	}

	bool TryGetRange(int& start, int& count) const
	{
		switch (state)
		{
		case TState::Stop:
			start = startIndex;
			count = (endIndex == -1) ? 1 : (endIndex - startIndex + 1);
			return true;

		case TState::Fail:
		default:
			return false;
		}
	}

private:
	void PutNext(char ch)
	{
		switch (state)
		{
		case TState::Initial:
			if (ch == '(' || ch == ')')
				state = TState::Fail;
			else
			{
				name += ch;
				state = TState::Name;
			}
			break;

		case TState::Name:
			if (ch == ')')
				state = TState::Fail;
			else
				if (ch == '(')
					state = TState::StartIndexInitial;
				else
					name += ch;
			break;

		case TState::StartIndexInitial:
			if (ch < '0' || ch > '9')
				state = TState::Fail;
			else
			{
				startIndex = ch - '0';
				state = TState::StartIndex;
			}
			break;

		case TState::StartIndex:
			if (ch >= '0' && ch <= '9')
			{
				long long value = (long long)startIndex * 10 + (ch - '0');
				if (value > std::numeric_limits<int>::max())
					state = TState::Fail;
				else
					startIndex = (int)value;
			}
			else
				if (ch == '-')
					state = TState::EndIndexInitial;
				else
					if (ch == ')')
						state = TState::Stop;
					else
						state = TState::Fail;
			break;

		case TState::EndIndexInitial:
			if (ch < '0' || ch > '9')
				state = TState::Fail;
			else
			{
				endIndex = ch - '0';
				state = TState::EndIndex;
			}
			break;

		case TState::EndIndex:
			if (ch >= '0' && ch <= '9')
			{
				long long value = (long long)endIndex * 10 + (ch - '0');
				if (value > std::numeric_limits<int>::max())
					state = TState::Fail;
				else
					endIndex = (int)value;
			}
			else
				if (ch == ')')
					state = TState::Stop;
				else
					state = TState::Fail;
			break;

		case TState::Stop:
			state = TState::Fail;
			break;

		default:
			throw std::exception("Unknown state");
		}
	}
};

struct TParsedToken
{
private:
	enum class TState
	{
		Initial,
		StringBody,
		StringEnd,
		IntSigned,
		Int,
		Float,
		Error
	};

	TState state;
	double numberValue;
	string stringValue;
	TParsedVarName varValue;

	template <class T>
	void CheckNumber()
	{
		if (numberValue < std::numeric_limits<T>::lowest() || numberValue > std::numeric_limits<T>::max())
			state = TState::Error;
	}

public:
	bool IsInt() const
	{
		return state == TState::Int;
	}

	bool IsFloat() const
	{
		return state == TState::Float;
	}

	bool IsString() const
	{
		return state == TState::StringEnd;
	}

	bool IsVar() const
	{
		return state == TState::Error;
	}

	int GetInt() const
	{
		return (int)numberValue;
	}

	float GetFloat() const
	{
		return (float)numberValue;
	}

	const string& GetString() const
	{
		return stringValue;
	}

	TParsedVarName& GetVar()
	{
		return varValue;
	}

	const TParsedVarName& GetVar() const
	{
		return varValue;
	}

	void Parse(const string& token)
	{
		varValue.Parse(token);
		varValue.name.Lower();

		state = TState::Initial;
		numberValue = 0.0;
		double mult = 1.0;
		stringValue = "";
		int sign = 1;

		for (size_t i = 0; i <= token.Length(); i++)
		{
			const char ch = token[i];

			switch (state)
			{
			case TState::Initial:
				if (ch == '"')
					state = TState::StringBody;
				else
					if (ch == '+' || ch == '-')
					{
						state = TState::IntSigned;
						sign = (ch == '+') ? 1 : -1;
					}
					else
						if (isdigit(ch))
						{
							state = TState::Int;
							numberValue = ch - '0';
						}
						else
							state = TState::Error;
				break;

			case TState::StringBody:
				if (ch == '"')
					state = TState::StringEnd;
				else
					if (ch == '\0')
						state = TState::Error;
					else
						stringValue += ch;
				break;

			case TState::StringEnd:
				if (ch != '\0')
					state = TState::Error;
				break;

			case TState::IntSigned:
				if (ch == '.' || ch == '\0')
				{
					state = TState::Error;
					break;
				}
				state = TState::Int;
				// falldown

			case TState::Int:
				if (isdigit(ch))
				{
					numberValue = numberValue * 10 + (ch - '0');
					CheckNumber<float>();
				}
				else
					if (ch == '.')
						state = TState::Float;
					else
						if (ch == '\0')
						{
							numberValue *= sign;
							CheckNumber<int>();
						}
						else
							state = TState::Error;
				break;

			case TState::Float:
				if (isdigit(ch))
				{
					mult /= 10;
					numberValue += (ch - '0') * mult;
					CheckNumber<float>();
				}
				else
					if (ch == '\0')
					{
						if (mult == 1.0)
							state = TState::Error;
						else
						{
							numberValue *= sign;
							CheckNumber<float>();
						}
					}
					else
						state = TState::Error;
				break;

			default:
				return;
			}
		}
	}
};