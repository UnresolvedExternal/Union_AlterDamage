namespace NAMESPACE
{
	extern CSubscription skipNoise;
	CSubscription skipNoise(TGameEvent::Loop, []()
		{
			if (!settings.allowSkipNoise)
				return skipNoise.Reset();

			for (zCEventMessage* message : player->GetEM(false)->messageList)
			{
				oCMsgConversation* csMsg = message->CastTo<oCMsgConversation>();
				if (csMsg && !csMsg->IsDeleted() && csMsg->subType == oCMsgConversation::EV_PLAYANISOUND)
				{
					if (!zinput->GetMouseButtonToggledRight() && !zinput->GetToggled(GAME_EXIT))
						return;

					zCView* view = ogame->array_view[oCGame::oEGameDialogView::GAME_VIEW_NOISE];
					if (view)
						view->Close();

					csMsg->f_no = 0.0f;
					break;
				}
			}
		}, CHECK_THIS_ENGINE);
}