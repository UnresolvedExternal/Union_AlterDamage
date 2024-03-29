namespace NAMESPACE
{
	const char* zMDL_NODE_NAME_HEAD = "BIP01 HEAD";

	std::unordered_map<zCVob*, zTMat_Group> groundMatGroups;
	std::unordered_set<zCAIPlayer*> waterRingAis;

	CSubscription clearGroundMatGroups(ZSUB(PreLoop), []()
		{
			groundMatGroups.clear();
			waterRingAis.clear();
		});

	string ToString(const zMAT4& e)
	{
		string text;

		for (int y = 0; y < 4; y++)
		{
			text += "\r\n";

			for (int x = 0; x < 4; x++)
			{
				if (x)
					text += " ";

				string elem = e[y][x];

				while (elem.Length() < 9)
					elem = " " + elem;

				text += elem;
			}
		}

		return text;
	}

	string ToString(const zVEC3& e)
	{
		string text;

		for (int x = 0; x < 3; x++)
		{
			if (x)
				text += " ";

			string elem = e[x];
			
			while (elem.Length() < 9)
				elem = " " + elem;

			text += elem;
		}

		return text;
	}
	
	zCProgMeshProto::zCSubMesh* zCProgMeshProto::TraceRayExt(const zVEC3& rayOrigin, const zVEC3& ray, const int traceFlags, zTTraceRayReport& report)
	{
		report.foundHit = FALSE;
		report.foundPoly = nullptr;
		zCProgMeshProto::zCSubMesh* foundSubMesh = nullptr;

		float bestAlpha = std::numeric_limits<float>::max();
		int	bestTriIndex = 0;
		zTPlane* bestPlane = nullptr;

		for (int s = 0; s < numSubMeshes; s++)
		{
			zCSubMesh* subMesh = &(subMeshList[s]);

			if (subMesh->triPlaneList.GetNum() <= 0) 
				continue;

			const zCMaterial* mat = subMesh->material;
			
			if (mat->matGroup == zMAT_GROUP_WATER) 
			{
				if (!(traceFlags & zTRACERAY_POLY_TEST_WATER))
					continue;
			}
			else 
			{
				if (traceFlags & zTRACERAY_POLY_IGNORE_TRANSP) 
					if (mat->rndAlphaBlendFunc != zRND_ALPHA_FUNC_NONE || mat->texture && mat->texture->hasAlpha)
						continue;
			}

			if (mat->noCollDet)
				continue;

			for (int i = 0; i < subMesh->triList.GetNum(); i++)
			{
				bool hit;
				zVEC3 inters;
				float alpha;

				if (traceFlags & zTRACERAY_POLY_2SIDED)
					hit = CheckRayPolyIntersection2Sided(subMesh, i, rayOrigin, ray, inters, alpha);
				else
					hit = CheckRayPolyIntersection(subMesh, i, rayOrigin, ray, inters, alpha);
				
				if (!hit)
					continue;
				
				if (!(traceFlags & zTRACERAY_FIRSTHIT) && alpha >= bestAlpha)
					continue;

				foundSubMesh = subMesh;
				bestTriIndex = i;
				bestPlane = &(subMesh->triPlaneList[subMesh->triPlaneIndexList[bestTriIndex]]);
				report.foundIntersection = inters;
				report.foundHit = true;
				alpha = std::min(alpha, bestAlpha);

				if (traceFlags & zTRACERAY_FIRSTHIT)
					break;
			}
		}

		if (report.foundHit && (traceFlags & zTRACERAY_POLY_NORMAL))
			report.foundPolyNormal = bestPlane->normal;

		return foundSubMesh;
	}

	zTMat_Group CalcGroundMatGroup(zCVob* vob)
	{
		zTMat_Group groundMat = COA(vob, groundPoly, material, matGroup);
		zCWorld* world = COA(vob, homeWorld);

		if (!world)
			return groundMat;

		int rayFlags = zTRACERAY_STAT_POLY | zTRACERAY_POLY_TEST_WATER | zTRACERAY_VOB_IGNORE_CHARACTER | zTRACERAY_VOB_IGNORE_NO_CD_DYN;
		rayFlags |= ZENDEF(0, 0, 0, zTRACERAY_VOB_IGNORE_PROJECTILES);

		player->SetFallDownHeight(1000.0f);
		if (!world->TraceRayNearestHit(vob->GetPositionWorld(), zVEC3(0, -20000, 0), vob, rayFlags))
			return groundMat;

		const zTTraceRayReport& report = world->traceRayReport;

		if (report.foundPoly)
			return COA(report.foundPoly, material, matGroup);

		if (!report.foundVob)
			return groundMat;

		zCVisual* visual = report.foundVob->GetVisual();
		zCProgMeshProto* mesh = COA(visual, CastTo<zCProgMeshProto>());

		if (!mesh)
			mesh = COA(visual, CastTo<zCMorphMesh>(), morphMesh);

		if (!mesh)
			return zMAT_GROUP_UNDEF;

		zMAT4 trafoWorldToObj = report.foundVob->trafoObjToWorld.InverseLinTrafo();
		zVEC3 pos = trafoWorldToObj * vob->GetPositionWorld();
		zVEC3 ray = report.foundVob->trafoObjToWorld.Transpose() * zVEC3(0, -20000, 0);
		zTTraceRayReport dummyReport;
		zCProgMeshProto::zCSubMesh* subMesh = mesh->TraceRayExt(pos, ray, rayFlags, dummyReport);
	
		return COA(subMesh, material, matGroup);
	}

	zTMat_Group CalcGroundMatGroup_Cached(zCVob* vob)
	{
		auto it = groundMatGroups.find(vob);

		if (groundMatGroups.find(vob) != groundMatGroups.end())
			return it->second;

		return groundMatGroups[vob] = CalcGroundMatGroup(vob);
	}

	void zCModel::HandleSound_Union(zCModelAniActive* ani, zCModelAniEvent* actEvent, int actFrame)
	{
		zCSoundFX* sound = actEvent->soundList[0];

		if (!sound)
			return;

#if ENGINE >= Engine_G2
		if (homeVob->GetCharacterClass() == zCVob::zVOB_CHAR_CLASS_PC)
			sound->canBeCachedOut = false;
#endif

		int slot = (actEvent->value2 == 0) ? zSND_SLOT_NONE : 1;
		zCSoundSystem::zTSound3DParams	params;
		params.SetDefaults();
		params.radius = actEvent->value1;
		int sndHandle = zsound->PlaySound3D(sound, homeVob, slot, &params);

#if ENGINE >= Engine_G2
		if (sndHandle >= 0)
			m_listOfVoiceHandles.Insert(sndHandle);
#endif
	}

	void zCModel::HandleSoundGround_Union(zCModelAniActive* ani, zCModelAniEvent* actEvent, int actFrame)
	{
		if (!homeVob)
			return;

		oCNpc* npc = homeVob->CastTo<oCNpc>();
		zTMat_Group matGroup = (COA3(npc, human_ai, waterLevel) > 0) ? zMAT_GROUP_WATER : CalcGroundMatGroup_Cached(homeVob);
		zCSoundFX* sound = actEvent->soundList[(int)matGroup];

		if (!sound)
			return;

#if ENGINE >= Engine_G2
		if (homeVob->GetCharacterClass() == zCVob::zVOB_CHAR_CLASS_PC)
			sound->canBeCachedOut = false;
#endif

		if (sound->CacheIn(0.5f) != zRES_CACHED_IN)
			return;

		int slot = (actEvent->value2 == 0) ? zSND_SLOT_NONE : 1;
		zCSoundSystem::zTSound3DParams	params;
		params.SetDefaults();
		params.radius = actEvent->value1;
		zsound->PlaySound3D(sound, homeVob, slot, &params);

		zCAIPlayer* ai = COA2(npc, human_ai);

		if (!ai || matGroup != zMAT_GROUP_WATER)
			return;

		const zTBBox3D& protoBox = this->bbox3DLocalFixed;
		const float scalar = ((protoBox.maxs[VX] - protoBox.mins[VX]) + (protoBox.maxs[VZ] - protoBox.mins[VZ])) * 0.5F;
		const float modelSizeScale = scalar * 0.0225178f;

		if (ai->velocityLen2 > 200 * 200 * modelSizeScale)
		{
			zCParticleFX* partFX = zNEW(zCParticleFX);
			zCVob* vobFX = zNEW(zCVob);
			vobFX->dontWriteIntoArchive = true;
			vobFX->SetCollDet(false);
			vobFX->BeginMovement();
			float waterY = static_cast<zCCollObjectCharacter*>(homeVob->GetCollisionObject())->m_oSpatialState.m_fWaterY;
			vobFX->SetPositionWorld(zVEC3(ai->centerPos[VX], waterY, ai->centerPos[VZ]));
			vobFX->SetVisual(partFX);
			partFX->SetAndStartEmitter("PFX_WATERSPLASH", false);
			homeVob->GetHomeWorld()->AddVob(vobFX);
			vobFX->EndMovement(ZENDEF(, , , true));
			vobFX->Release();
			partFX->Release();
		}

		waterRingAis.insert(ai);
	}

	void zCModel::HandleStartPfx_Union(zCModelAniActive* ani, zCModelAniEvent* actEvent, int actFrame)
	{
		if (!homeVob)
			return;

		zCParticleEmitter* emitter = actEvent->pfxEmitterList[0];
		float handle = actEvent->value1;

		if (!emitter || emitter->ppsIsLooping && !handle)
			return;

		zCVob* pfxVob = zNEW(zCVob);
		zCParticleFX* pfx = zNEW(zCParticleFX);
		pfxVob->dontWriteIntoArchive = true;
		pfxVob->SetCollDet(false);
		pfxVob->SetPositionWorld(zVEC3(0));
		zCModelNodeInst* mnode = SearchNode(actEvent->string[0]);

		if (mnode)
		{
			if (actEvent->value2)
			{
				zCTree<zCVob>* vobNode = homeVob->GetHomeWorld()->AddVobAsChild(pfxVob, homeVob->globalVobTreeNode);
				AttachChildVobToNode(vobNode->GetData(), mnode);
				pfxVob->SetVisual(pfx);
				pfx->SetAndStartEmitter(emitter, false);
			}
			else
			{
				pfxVob->SetTrafoObjToWorld(homeVob->trafoObjToWorld * GetTrafoNodeToModel(mnode));
				pfxVob->SetVisual(pfx);
				pfx->SetAndStartEmitter(emitter, false);
				homeVob->GetHomeWorld()->AddVob(pfxVob);
			}
		}
		else
		{
			pfxVob->SetPositionWorld(homeVob->GetPositionWorld());
			pfxVob->SetVisual(pfx);
			pfx->SetAndStartEmitter(emitter, false);
			homeVob->GetHomeWorld()->AddVob(pfxVob);
		}

		if (handle > 0 && !emitter->isOneShotFX)
		{
			for (int i = 0; i < startedVobFX.GetNum(); i++)
				if (startedVobFX[i].vobFXHandle == handle)
					goto skipThis;

			zTMdl_StartedVobFX entry;
			entry.vob = pfxVob;
			entry.vobFXHandle = handle;
			startedVobFX.Insert(entry);
			entry.vob->AddRef();

		skipThis:;
		}

		zRELEASE(pfxVob);
		zRELEASE(pfx);
	}

	void zCModel::HandleStopPfx_Union(zCModelAniActive* ani, zCModelAniEvent* actEvent, int actFrame)
	{
		float handle = actEvent->value1;

		for (int i = 0; i < startedVobFX.GetNum(); i++)
		{
			if (startedVobFX[i].vobFXHandle != handle)
				continue;

			zCVob* vobFX = startedVobFX[i].vob;

			if (vobFX && vobFX->visual)
				if (vobFX->visual->_GetClassDef() == zCParticleFX::classDef)
					static_cast<zCParticleFX*>(vobFX->visual)->StopEmitterOutput();

			RemoveChildVobFromNode(vobFX);
			vobFX->Release();
			startedVobFX.RemoveIndex(i);
			break;
		}
	}

	void zCModel::HandleSwapMesh_Union(zCModelAniActive* ani, zCModelAniEvent* actEvent, int actFrame)
	{
		zCModelNodeInst* node1 = SearchNode(actEvent->string[0]);
		zCModelNodeInst* node2 = SearchNode(actEvent->string[1]);

		if (node1 && node2)
			std::swap(node1->nodeVisual, node2->nodeVisual);
	}

	void zCModel::HandleStartAni_Union(zCModelAniActive* ani, zCModelAniEvent* actEvent, int actFrame)
	{
		zCModelNodeInst* node = actEvent->string[1].IsEmpty() ? SearchNode(zMDL_NODE_NAME_HEAD) : SearchNode(actEvent->string[1]);

		if (!node)
			return;

		zCMorphMesh* morphMesh = zDYNAMIC_CAST<zCMorphMesh>(node->nodeVisual);

		if (morphMesh)
			morphMesh->StartAni(actEvent->string[0], actEvent->value1, actEvent->value2);
	}

	void zCModel::HandleTremor_Union(zCModelAniActive* ani, zCModelAniEvent* actEvent, int actFrame)
	{
		if (!homeVob || !zCCamera::activeCam)
			return;

		zCCamera::activeCam->AddTremor
		(
			homeVob->GetPositionWorld(),
			actEvent->value1,		// radiusSquare
			actEvent->value2,		// timeMSec
			zVEC3(actEvent->value3, actEvent->value4, actEvent->value3)// amplitude
		);
	}

	FASTHOOK(zCModel, DoAniEvents);

	void zCModel::DoAniEvents_Union(zCModelAniActive* ani)
	{
		if (!homeVob || !homeVob->GetHomeWorld())
			return;

		zCModelAni* protoAni = ani->protoAni;

		if (protoAni->numAniEvents <= 0)
			return;

		int actFrame = (int)ani->actFrame;

		while (ani->actAniEvent < protoAni->numAniEvents && ani->actAniEvent >= 0 && ani->advanceDir)
		{
			zCModelAniEvent* actEvent = &(protoAni->aniEvents[ani->actAniEvent]);

			if (ani->advanceDir == +1)
			{
				if (actEvent->frameNr > actFrame)
					break;
			}
			else
				if (actEvent->frameNr < actFrame)
					break;

			occuredAniEvents.Insert(actEvent);

			switch (actEvent->aniEventType)
			{
			case zMDL_EVENT_SOUND: HandleSound_Union(ani, actEvent, actFrame); break;
			case zMDL_EVENT_SOUND_GRND:	HandleSoundGround_Union(ani, actEvent, actFrame); break;
			case zMDL_EVENT_PFX: HandleStartPfx_Union(ani, actEvent, actFrame);	break;
			case zMDL_EVENT_PFX_STOP: HandleStopPfx_Union(ani, actEvent, actFrame); break;
			case zMDL_EVENT_SWAPMESH: HandleSwapMesh_Union(ani, actEvent, actFrame); break;
			case zMDL_EVENT_MM_STARTANI: HandleStartAni_Union(ani, actEvent, actFrame); break;
			case zMDL_EVENT_CAM_TREMOR: HandleTremor_Union(ani, actEvent, actFrame); break;
			}

			ani->actAniEvent += ani->advanceDir;
		}
	}

	zSTRING GetLandPfxName(zTMat_Group matGroup)
	{
		static CSymbol func(parser, (A"C_GetLandPfxName").Upper());

		if (func)
			return CallParser<zSTRING>(zCParser::GetParser(), func.GetIndex(), static_cast<int>(matGroup));

		static zCPar_Symbol* waterSplash = parserParticleFX->GetSymbol("PFX_WATERSPLASH");

		if (waterSplash && matGroup == zMAT_GROUP_WATER)
			return waterSplash->name;

		return "PFX_DUST";
	}

	TScope<byte> ForceWaterRing()
	{
		byte* address = (byte*)ZENDEF(0x004FF2CC, 0x005127AE, 0x0050C028, 0x0050EDDC);
		static TMemUnlocker unlocker(address, 1, true);
		return AssignTemp(*address, (byte)0x74);
	}

	void __fastcall Hook_zCAIPlayer_UpdateEffects(zCAIPlayer*, void*);
	CInvoke<void(__thiscall*)(zCAIPlayer*)> Ivk_zCAIPlayer_UpdateEffects(ZenDef<TInstance>(0x004FEE20, 0x005122B0, 0x0050BBE0, 0x0050E990), &Hook_zCAIPlayer_UpdateEffects, IvkEnabled(ENGINE));
	void __fastcall Hook_zCAIPlayer_UpdateEffects(zCAIPlayer* _this, void* vtable)
	{
		TScope<void*> scope;
		zSTRING pfxName;

		TScope<byte> waterRingScope;

		if (_this->vob && _this->startFXModelLandDust)
		{
			void** address = (void**)ZENDEF(0x004FF1B1, 0x00512677, 0x0050BF0D, 0x0050ECBF);
			static TMemUnlocker unlocker(address, 4, true);
			zTMat_Group matGroup = (_this->waterLevel > 0) ? zMAT_GROUP_WATER : CalcGroundMatGroup_Cached(_this->vob);
			pfxName = GetLandPfxName(matGroup);
			scope = AssignTemp(*address, (void*)pfxName.ToChar());

			if (matGroup == zMAT_GROUP_WATER)
				waterRingScope = ForceWaterRing();
		}
		else
			if (waterRingAis.find(_this) != waterRingAis.end())
				waterRingScope = ForceWaterRing();

		Ivk_zCAIPlayer_UpdateEffects(_this);
	}
}
