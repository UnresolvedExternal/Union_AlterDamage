#include "Misc.h"

bool Intersects(const tagRECT& a, const tagRECT& b)
{
	return (a.left <= b.right &&
		b.left <= a.right &&
		a.top <= b.bottom &&
		b.top <= a.bottom);
}

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
