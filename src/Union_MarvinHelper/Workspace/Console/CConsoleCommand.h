namespace NAMESPACE
{
	class CConsoleCommand : public CCommandBase
	{
	protected:
		CConsoleContext& context;
		std::vector<string> prefix;
		std::vector<string> args;

		string GetPrefix() const
		{
			if (prefix.empty())
				return { };

			string res = prefix.front();

			for (size_t i = 1; i < prefix.size(); i++)
			{
				res += " ";
				res += prefix[i];
			}

			return res;
		}

		bool IsPrefixMatched() const
		{
			if (context.GetCommand().tokens.size() < prefix.size())
				return false;

			for (size_t i = 0; i < prefix.size(); i++)
				if (!prefix[i].CompareI(context.GetCommand().tokens[i]))
					return false;

			return true;
		}

		static bool HasWord(const string& str, const string& word)
		{
			return word.IsEmpty() || str.HasWord(word);
		}

		static bool HasWordI(const string& str, const string& word)
		{
			return word.IsEmpty() || str.HasWordI(word);
		}

		bool CheckHint(const string& str) const
		{
			return str.Length() && HasWord(str, args.back());
		}

		bool CheckHintI(const string& str) const
		{
			return str.Length() && HasWordI(str, args.back());
		}

		virtual void AddHints(std::vector<string>& hints) { };
		virtual string Execute() { return { }; };

	public:
		CConsoleCommand(const string& prefix = A"", const string& description = A"") :
			context(CConsoleContext::GetInstance())
		{
			TParsedCommand prefixParser;
			prefixParser.Parse(prefix);
			this->prefix = prefixParser.tokens;

			if (this->prefix.size() && description.Length())
				zcon->Register(GetPrefix(), description);
		}

		virtual std::vector<string> GetHints() override final
		{
			if (!IsPrefixMatched())
				return { };

			if (context.GetCommand().GetWordNumTyping() < (int)prefix.size())
				return { };

			for (size_t i = prefix.size(); i < context.GetCommand().tokens.size(); i++)
				args.push_back(context.GetCommand().tokens[i]);

			if ((size_t)context.GetCommand().GetWordNumTyping() == context.GetCommand().tokens.size())
				args.push_back("");

			std::vector<string> hints;
			AddHints(hints);
			args.clear();
			return hints;
		}

		virtual bool TryExecute(string& output) override final
		{
			if (prefix.empty())
				return false;

			if (!IsPrefixMatched())
				return false;

			for (size_t i = prefix.size(); i < context.GetCommand().tokens.size(); i++)
				args.push_back(context.GetCommand().tokens[i]);

			output = Execute();
			args.clear();
			return output.Length();
		}
	};
}