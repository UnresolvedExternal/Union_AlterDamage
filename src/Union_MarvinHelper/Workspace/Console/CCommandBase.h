#pragma once

class CCommandBase
{
public:
	virtual std::vector<string> GetHints()
	{
		return std::vector<string>();
	}

	virtual bool TryExecute(string& output)
	{
		return false;
	}

	virtual ~CCommandBase()
	{

	}
};