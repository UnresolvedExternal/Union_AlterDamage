namespace NAMESPACE
{
	std::vector<CInvoke_zCWorld_VobRemovedFromWorld*>* CInvoke_zCWorld_VobRemovedFromWorld::hooks;
	CInvoke<void(__thiscall*)(zCWorld*, zCVob*)>* CInvoke_zCWorld_VobRemovedFromWorld::ivk;
	int CInvoke_zCWorld_VobRemovedFromWorld::attachedCount;

	void __fastcall CInvoke_zCWorld_VobRemovedFromWorld::Hook(zCWorld* _this, void* vtable, zCVob* vob)
	{
		for (int i = (*hooks).size() - 1; i >= 0; i--)
		{
			if ((*hooks)[i]->state == ATTACHED)
			{
				return (*hooks)[i]->handler(_this, vob);
			}
		}

		throw std::domain_error("");
	}

	void CInvoke_zCWorld_VobRemovedFromWorld::IncreaseAttachedCount(int delta)
	{
		attachedCount += delta;

		if (delta == 1 && attachedCount == 1)
		{
			ivk = new CInvoke<void(__thiscall*)(zCWorld*, zCVob*)>(ZCWORLD_VOBREMOVEDFROMWORLD_ADDRESS, &CInvoke_zCWorld_VobRemovedFromWorld::Hook);
		}

		if (delta == -1 && attachedCount == 0)
		{
			delete ivk;
		}
	}

	void CInvoke_zCWorld_VobRemovedFromWorld::Attach()
	{
		state = ATTACHED;
		IncreaseAttachedCount(1);
	}

	void CInvoke_zCWorld_VobRemovedFromWorld::Detach()
	{
		state = DETACHED;
		IncreaseAttachedCount(-1);
	}

	CInvoke_zCWorld_VobRemovedFromWorld::CInvoke_zCWorld_VobRemovedFromWorld():
		state(DETACHED),
		handler(nullptr)
	{
		hooks = hooks ? hooks : new std::vector<CInvoke_zCWorld_VobRemovedFromWorld*>();
		index = hooks->size();
		hooks->push_back(this);
	}

	CInvoke_zCWorld_VobRemovedFromWorld::CInvoke_zCWorld_VobRemovedFromWorld(const std::function<void(zCWorld*, zCVob*)> & handler):
		state(ATTACHED),
		handler(handler)
	{
		hooks = hooks ? hooks : new std::vector<CInvoke_zCWorld_VobRemovedFromWorld*>();
		index = hooks->size();
		hooks->push_back(this);
		IncreaseAttachedCount(1);
	}

	CInvoke_zCWorld_VobRemovedFromWorld::CInvoke_zCWorld_VobRemovedFromWorld(CInvoke_zCWorld_VobRemovedFromWorld && source):
		state(source.state),
		index(source.index),
		handler(std::move(source.handler))
	{
		source.state = MOVEDOUT;
	}

	void CInvoke_zCWorld_VobRemovedFromWorld::SetHandler(const std::function<void(zCWorld*, zCVob*)> & handler)
	{
		this->handler = handler;

		if (handler == nullptr && state == ATTACHED)
		{
			Detach();
		}

		if (handler != nullptr && state == DETACHED)
		{
			Attach();
		}
	}

	void CInvoke_zCWorld_VobRemovedFromWorld::operator ()(zCWorld * _this, zCVob * vob)
	{
		for (int i = index - 1; i >= 0; i--)
		{
			if ((*hooks)[i]->state == ATTACHED)
			{
				return (*hooks)[i]->handler(_this, vob);
			}
		}

		return (*ivk)(_this, vob);
	}

	CInvoke_zCWorld_VobRemovedFromWorld::~CInvoke_zCWorld_VobRemovedFromWorld()
	{
		if (state == MOVEDOUT)
		{
			return;
		}

		if (state == ATTACHED)
		{
			Detach();
		}

		for (int i = index + 1; i < (int)hooks->size(); i++)
		{
			(*hooks)[i]->index -= 1;
		}

		hooks->erase(hooks->begin() + index);

		if (hooks->empty())
		{
			delete hooks;
			hooks = nullptr;
		}
	}
}