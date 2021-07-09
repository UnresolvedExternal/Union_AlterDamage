namespace NAMESPACE
{
	float pMouseX = 300, pMouseY = 300, dist = 0.0f;
	int vMouseX = 0, vMouseY = 0;
	int pCursorSize = 50;

	CSubscription trackMouse(ZSUB(Loop), []()
		{
			float x, y, w;
			zinput->GetMousePos(x, y, w);
			dist += w / 120.0f;

			pMouseX = CoerceInRange<float>(pMouseX + x * 3.0f, 0.0f, 0.0f, zrenderer->vid_xdim);
			pMouseY = CoerceInRange<float>(pMouseY + y * 3.0f, 0.0f, 0.0f, zrenderer->vid_ydim);

			vMouseX = screen->anx(pMouseX + 0.5f);
			vMouseY = screen->any(pMouseY + 0.5f);
			screen->PrintCXY(A vMouseX + " " + vMouseY + " " + dist);
		});

	CSubscription renderMouse(ZSUB(Loop), []()
		{
			auto camScope = AssignTemp(zCCamera::activeCam, zCCamera::activeCam);
			static oCItem* item = (oCItem*)ogame->GetGameWorld()->CreateVob_novt(zVOB_TYPE_ITEM, "ITFO_MILK");
			item->CreateVisual();

			zCVob* vob = new zCVob();
			vob->SetVisual(item->GetVisual());

			float dx = screen->anx(pCursorSize);
			float dy = screen->any(pCursorSize);

			zCView* view = new zCView(vMouseX - dx, vMouseY - dy, vMouseX + dx, vMouseY + dy);
			screen->InsertItem(view);
			view->Blit();

			static zCWorld* world = new zCWorld();
			world->m_bIsInventoryWorld = true;

			//item->RenderItem(world, view, -2.0f);
			auto scope = AssignTemp(*(int*)0x0089EBB4, 5000); // light range for G2A (for G1 - 0x0083AB90)
			world->GetBspTree()->bspTreeMode = zBSP_MODE_INDOOR;
			world->GetActiveSkyControler()->SetFillBackground(false);
			world->GetBspTree()->drawVobBBox3D = false;

			zCVob* camVob = new zCVob();
			camVob->SetPositionWorld(zVEC3(0, 0, dist));
			world->AddVob(camVob);

			world->AddVob(vob);
			vob->SetPositionWorld(zVEC3(0, 0, 0));
			vob->RotateLocalZ(dist);

			zCCamera cam;
			cam.connectedVob = camVob;
			cam.SetRenderTarget(view);
			cam.SetFarClipZ(10000.0f);

			{
				auto scope = AssignTemp(zCEventManager::disableEventManagers, 1);
				world->Render(cam);
			}

			world->RemoveVob(camVob);
			world->RemoveVob(vob);
			world->RemoveVobSubtree(vob);

			screen->RemoveItem(view);
			delete view;
			camVob->Release();
			vob->Release();
		});

	CSubscription testString(ZSUB(Entry), []()
		{
			zSTRING s;

			for (size_t i = 0; i < sizeof(s); i++)
				cmd << A i + ": " + *((byte*)& s + i) << endl;
		});
}