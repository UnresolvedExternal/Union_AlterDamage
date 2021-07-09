namespace NAMESPACE
{
	template <class T>
	class CVectorOption : public std::vector<T>
	{
	public:
		CVectorOption() = default;
		CVectorOption(const CVectorOption&) = default;
		CVectorOption(CVectorOption&&) = default;
		CVectorOption& operator=(const CVectorOption&) = default;
		CVectorOption& operator=(CVectorOption&&) = default;

		CVectorOption(const std::initializer_list<T>& values)
		{
			this->reserve(values.size());

			for (const T& e : values)
				this->push_back(e);
		}

		bool operator==(const CVectorOption& right) const
		{
			if (size() != right.size())
				return false;

			for (size_t i = 0; i < size(); i++)
				if ((*this)[i] != right[i])
					return false;

			return true;
		}

		bool operator!=(const CVectorOption& right) const
		{
			return !((*this) == right);
		}
	};

	template <class T>
	std::ostream& operator<<(std::ostream& out, const CVectorOption<T>& values)
	{
		for (size_t i = 0; i < values.size(); i++)
		{
			if (i)
				out << "|";

			out << StdToString<T>(values[i]).GetVector();
		}

		return out;
	}

	template <class T>
	std::istream& operator>>(std::istream& in, CVectorOption<T>& value)
	{
		value.clear();

		if (!in)
			return in;

		std::string text;
		std::getline(in, text);
		string token;

		for (size_t i = 0; i <= text.length(); i++)
		{
			const char ch = text[i];

			switch (ch)
			{
			case '|':
			case '\0':
				if (token.Length())
					value.push_back(StdFromString<T>(token));

				token.Clear();
				break;

			default:
				token += ch;
				break;
			}
		}

		return in;
	}
}