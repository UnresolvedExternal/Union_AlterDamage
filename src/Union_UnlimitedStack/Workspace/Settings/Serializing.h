namespace NAMESPACE
{
	template <class T>
	string StdToString(const T& value)
	{
		std::ostringstream out;
		out << value;
		return out.str().c_str();
	}

	template <class T>
	T StdFromString(const string& text)
	{
		std::istringstream in(text.GetVector());
		T value;
		in >> value;
		return value;
	}

	template <>
	string StdToString(const string& value)
	{
		return value;
	}

	template <>
	string StdFromString(const string& value)
	{
		return value;
	}
}