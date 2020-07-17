namespace NAMESPACE
{
	extern CSubscription verifyPlugins;
	CSubscription verifyPlugins(TGameEvent::LoadEnd, []()
		{
			zCPar_Symbol* symbol = parser->GetSymbol(Z"VERIFYPLUGIN");
			if (!symbol || symbol->type != zPAR_TYPE_FUNC)
				return verifyPlugins.Reset();

			for (const CList<CPlugin>* list = CPlugin::GetPluginList().GetNext(); list; list = list->GetNext())
				if (list->GetData())
				{
					zSTRING name = list->GetData()->GetName().ToChar();
					parser->datastack.Push(reinterpret_cast<int>(&name));
					parser->datastack.Push(zPAR_TOK_PUSHVAR);
					parser->DoStack(symbol->single_intdata);
				}
		}, CHECK_THIS_ENGINE);
}