#include "Misc.h"

#undef min
#undef max

typedef void(*TParseExternalScript)(string parser, string script);
TParseExternalScript parseExternalScript;

void ParseExternalScript(const string& parser, const string& path)
{
	if (!parseExternalScript)
	{
		for (const CList<CPlugin>* pluginElem = CPlugin::GetPluginList().GetNext(); true; pluginElem = pluginElem->GetNext())
		{
			const CPlugin* plugin = pluginElem->GetData();

			if (plugin->GetName().HasWordI("ZPARSEREXTENDER"))
			{
				HPLUGIN module = plugin->GetModule();
				parseExternalScript = (TParseExternalScript)GetProcAddress(module, "ParseExternalScript");
				break;
			}
		}
	}

	parseExternalScript(parser, path);
}

bool TryParse(const string& line, int& number)
{
	if (!line.Length())
		return false;

	int sign = 1;
	int i = 0;

	if (line[0] == '+')
	{
		sign = 1;
		i = 1;
	}
	else if (line[0] == '-')
	{
		sign = -1;
		i = 1;
	}

	long long value = 0;
	for (i; i < (int)line.Length(); i++)
	{
		if (line[i] < '0' || line[i] > '9')
			return false;

		value = value * 10 + (line[i] - '0');
		if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max())
			return false;
	}

	number = value;
	return true;
}

bool TryParse(const string& line, float& number)
{
	if (!line.Length())
		return false;

	int sign = 1;
	int i = 0;

	if (line[0] == '+')
	{
		sign = 1;
		i = 1;
	}
	else if (line[0] == '-')
	{
		sign = -1;
		i = 1;
	}

	bool point = false;
	double power = 1;
	double value = 0;

	for (i; i < (int)line.Length(); i++)
	{
		if (point)
			power /= 10;

		if (line[i] == '.')
			if (point)
				return false;
			else
			{
				point = true;
				continue;
			}

		if (line[i] < '0' || line[i] > '9')
			return false;

		value = value * 10 + (line[i] - '0') * power;

		if (value < std::numeric_limits<float>::min() || value > std::numeric_limits<float>::max())
			return false;
	}

	number = (float)value;
	return true;
}

bool TryParse(const string& line, string& text)
{
	if ((int)line.Length() < 2 || line.First() != '"' || line.Last() != '"')
		return false;

	text = "";
	bool slash = false;

	for (int i = 1; i < (int)line.Length() - 1; i++)
		if (slash)
			if (line[i] == '\\' || line[i] == '"')
				text += line[i];
			else
				return false;
		else
			if (line[i] == '\\')
			{
				slash = true;
				continue;
			}
			else
				if (line[i] == '"')
					return false;
				else
					text += line[i];

	return !slash;
}