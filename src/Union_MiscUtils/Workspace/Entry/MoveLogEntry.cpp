namespace NAMESPACE
{
	void __fastcall Hook_oCLogTopic_AddEntry(oCLogTopic*, zSTRING const&);
	COptionInvoke<void(__fastcall*)(oCLogTopic*, zSTRING const&), bool> Ivk_oCLogTopic_AddEntry(ZenDef<TInstance>(0x0072EA60, 0x0076D0B0, 0x0077A630, 0x00663870), &Hook_oCLogTopic_AddEntry, IvkEnabled(ENGINE), Settings::MoveLogEntry);
	void __fastcall Hook_oCLogTopic_AddEntry(oCLogTopic* _this, zSTRING const& a0)
	{
		Ivk_oCLogTopic_AddEntry(_this, a0);

		oCLogManager& manager = oCLogManager::GetLogManager();

		for (zCList<oCLogTopic>* prev = &manager.m_lstTopics; prev && prev->next; prev = prev->next)
			if (prev->next->data == _this)
			{
				zCList<oCLogTopic>* ele = prev->next;
				prev->next = ele->next;
				ele->next = manager.m_lstTopics.next;
				manager.m_lstTopics.next = ele;
				return;
			}
	}
}