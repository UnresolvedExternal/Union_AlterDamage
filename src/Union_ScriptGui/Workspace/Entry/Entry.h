namespace NAMESPACE
{
	class CScriptGui
	{
	private:
		static const int USER_MAX = 16;

		struct
		{
			int ints[USER_MAX];
			zSTRING strings[USER_MAX];

			int vPosX;
			int vPosY;
			int vSizeX;
			int vSizeY;

			zSTRING texture;
			int item;
			zSTRING text;
			int opacity;

			zSTRING font;
			int fontR;
			int fontG;
			int fontB;

			int hTextAlign;
			int vTextAlign;

			int proc;
		} vars;

		static zCView* view;
		static std::vector<CScriptGui*> guis;

		CScriptGui* parent;
		std::vector<CScriptGui*> childs;

		void ResetParserVars()
		{
			for (int i = 0; i < USER_MAX; i++)
			{
				vars.ints[i] = 0;
				vars.strings[i] = "";
			}

			vars.vPosX = 0;
			vars.vPosY = 0;
			vars.vSizeX = 8192;
			vars.vSizeY = 8192;

			vars.texture = "";
			vars.item = 0;
			vars.text = "";
			vars.opacity = 255;

			vars.font = screen->GetFontName();

			vars.fontR = 255;
			vars.fontG = 255;
			vars.fontB = 255;

			vars.hTextAlign = -1;
			vars.vTextAlign = -1;
		}

		int GetVPosX() const { return CoerceInRange(vars.vPosX, 0, 0, 8192); }
		int GetVPosY() const { return CoerceInRange(vars.vPosY, 0, 0, 8192); }
		int GetVSizeX() const { return CoerceInRange(vars.vSizeX, 0, 0, 8192); }
		int GetVSizeY() const { return CoerceInRange(vars.vSizeY, 0, 0, 8192); }
		int GetOpacity() const { return CoerceInRange(vars.opacity, 0, 0, 255); }
		zCOLOR GetFontColor() const { return zCOLOR(CoerceInRange(vars.fontR, 0, 0, 255), CoerceInRange(vars.fontG, 0, 0, 255), CoerceInRange(vars.fontB, 0, 0, 255), GetOpacity()); }

		void ConvertViewportFromParent(zVEC2& pos, zVEC2& size) const
		{
			pos[0] += (float)GetVPosX() / 8192.0f * size[0];
			pos[1] += (float)GetVPosY() / 8192.0f * size[1];
			size[0] *= (float)GetVSizeX() / 8192.0f;
			size[1] *= (float)GetVSizeY() / 8192.0f;
		}

		void RenderImpl(zVEC2 pos, zVEC2 size) const
		{
			static ZOwner<zCWorld> renderWorld = (ZOwner<zCWorld>)new zCWorld();
			static CSubscription deleteRenderWorld(TGameEvent::Exit, []() { renderWorld.reset(); });

			ConvertViewportFromParent(pos, size);
			view->SetPos(round(pos[0]), round(pos[1]));
			view->SetSize(round(size[0]), round(size[1]));

			view->InsertBack(vars.texture);
			
			view->alpha = GetOpacity();
			view->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);

			if (view->backTex)
			{
				view->Blit();
				view->InsertBack("");
			}

			if (vars.item)
			{
				CSymbol itemSymbol(parser, vars.item);
				ASSERT(itemSymbol.GetType() == CSymbol::Type::Instance);
				ZOwner<oCItem> renderItem = (ZOwner<oCItem>)zfactory->CreateItem(vars.item);
				renderItem->RotateForInventory(1);
				renderItem->RenderItem(renderWorld.get(), view, -2.0f);
			}

			if (vars.text.Length())
			{
				view->SetFont(vars.font);

				zVEC2 textPos(0.0f);
				const zVEC2 textSize(view->FontSize(const_cast<zSTRING&>(vars.text)), view->FontY());

				if (vars.hTextAlign == 0)
					textPos[0] = CoerceInRange(size[0] / 2.0f - textSize[0] / 2.0f, textSize[0], 0.0f, 8192.0f);
				else
					if (vars.hTextAlign > 0)
						textPos[0] = CoerceInRange(8192.0f, textSize[0], 0.0f, 8192.0f);

				if (vars.vTextAlign == 0)
					textPos[1] = CoerceInRange(size[1] / 2.0f - textSize[1] / 2.0f, textSize[1], 0.0f, 8192.0f);
				else
					if (vars.vTextAlign > 0)
						textPos[1] = CoerceInRange(8192.0f, textSize[1], 0.0f, 8192.0f);

				zrenderer->SetTextureStageState(0, zRND_TSS_COLOROP, zRND_TOP_BLENDDIFFUSEALPHA); // fixes bold font
				view->PrintTimed(textPos[0] + 0.5f, textPos[1] + 0.5f, vars.text, -2.0f, &GetFontColor());
				view->BlitText();
				view->ClrPrintwin();
			}

			for (CScriptGui* gui : childs)
				gui->RenderImpl(pos, size);
		}

		CScriptGui* GetRoot()
		{
			CScriptGui* node = this;
			while (parent)
				node = node->parent;
			return node;
		}

	public:
		CScriptGui() :
			parent(nullptr)
		{
			guis.push_back(this);
			ResetParserVars();
		}

		static void ResetAll()
		{
			for (CScriptGui* gui : guis)
			{
				gui->parent = nullptr;
				gui->childs.clear();
				gui->ResetParserVars();
			}
		}

		void SetParent(CScriptGui* newParent)
		{
			if (parent == newParent)
				return;

			if (parent)
				parent->childs.erase(std::find(parent->childs.begin(), parent->childs.end(), this));

			parent = newParent;
			parent->childs.push_back(this);
		}

		void GetParentBounds(zVEC2& vPos, zVEC2& vSize) const
		{
			std::vector<CScriptGui*> ancestors;
			for (CScriptGui* gui = parent; gui; gui = gui->parent)
				ancestors.push_back(gui);

			vPos = zVEC2(0.0f, 0.0f);
			vSize = zVEC2(8192.0f, 8192.0f);

			for (auto it = ancestors.rbegin(); it != ancestors.rend(); it++)
				(*it)->ConvertViewportFromParent(vPos, vSize);
		}

		bool IsIn(const zVEC2& point, bool ignoreChilds) const
		{
			zVEC2 pos, size;
			GetParentBounds(pos, size);
			ConvertViewportFromParent(pos, size);

			if (point[0] >= pos[0] && point[0] <= pos[0] + size[0] && point[1] >= pos[1] && point[1] <= pos[1] + size[1])
			{
				if (ignoreChilds)
					return true;

				for (auto it = childs.begin(); it != childs.end(); it++)
				{
					zVEC2 childMin(pos), childMax(size);
					(*it)->ConvertViewportFromParent(childMin, childMax);
					childMax += childMin;

					if (point[0] >= childMin[0] && point[0] <= childMax[0] && point[1] >= childMin[1] && point[1] <= childMax[1])
						return false;
				}

				return true;
			}

			return false;
		}
		
		float Anx(float x)
		{
			ASSERT(screen->psizex);

			float size = (float)x * 8192.0f / (float)screen->psizex;

			for (CScriptGui* gui = this; gui; gui = gui->parent)
			{
				ASSERT(gui->GetVSizeX());
				size *= 8192.0f / (float)gui->GetVSizeX();
			}

			return size;
		}

		float Any(float y)
		{
			ASSERT(screen->psizey);

			float size = (float)y * 8192.0f / (float)screen->psizey;

			for (CScriptGui* gui = this; gui; gui = gui->parent)
			{
				ASSERT(gui->GetVSizeY());
				size *= 8192.0f / (float)gui->GetVSizeY();
			}

			return size;
		}

		float Nax(float x)
		{
			ASSERT(screen->psizex);

			float size = (float)x * (float)screen->psizex / 8192.0f;

			for (CScriptGui* gui = this; gui; gui = gui->parent)
			{
				ASSERT(gui->GetVSizeX());
				size *= 8192.0f / (float)gui->GetVSizeX();
			}

			return size;
		}

		float Nay(float y)
		{
			ASSERT(screen->psizey);

			float size = (float)y * (float)screen->psizey / 8192.0f;

			for (CScriptGui* gui = this; gui; gui = gui->parent)
			{
				ASSERT(gui->GetVSizeY());
				size *= 8192.0f / (float)gui->GetVSizeY();
			}

			return size;
		}

		bool Proc() const
		{
			for (int i = (int)childs.size() - 1; i >= 0; i--)
				if (childs[i]->Proc())
					return true;

			if (!vars.proc)
				return false;

			CSymbol func(parser, vars.proc);
			
			ASSERT(func.GetType() == CSymbol::Type::Func);
			ASSERT(func.GetSymbol()->offset == zPAR_TYPE_INT);
			ASSERT(func.GetSymbol()->ele == 1);
			ASSERT(CSymbol(parser, func.GetIndex() + 1).GetSymbol()->type == zPAR_TYPE_INSTANCE);

			CParserScope scope(parser);
			parser->SetReturn((void*)this);
			parser->DoStack(func.GetValue<int>(0));
			
			int result;
			parser->GetParameter(result);

			return result;
		}

		void Render()
		{
			view = new zCView();
			screen->InsertItem(view);

			zVEC2 pos, size;
			GetParentBounds(pos, size);

			RenderImpl(pos, size);
			screen->RemoveItem(view);
			delete view;
		}

		static void RegisterAsParserClass(zCParser* parser, const string& className)
		{
			CSymbol classSymbol(parser, className);

			if (classSymbol.GetType() == CSymbol::Type::Class)
			{
				ASSERT(classSymbol.GetSymbol()->offset == sizeof(CScriptGui::vars));

				int deltaOffset = (int)NdrFieldOffset(CScriptGui, vars) - classSymbol.GetSymbol()->single_intdata;
				
				if (deltaOffset == 0)
					return;

				for (int i = 1; i <= (int)classSymbol.GetSymbol()->ele; i++)
					CSymbol(parser, classSymbol.GetIndex() + i).GetSymbol()->offset += deltaOffset;
			}
		}

		~CScriptGui()
		{
			guis.erase(std::find(guis.begin(), guis.end(), this));
		}
	};

	class CMouseDraw
	{
	private:
		zVEC2 pPos;
		zVEC2 pSize;
		string texture;
		bool moved;
		bool enabled;

		zVEC2 GetDrawPos() const
		{
			zVEC2 pos(round(pPos[0]) - pSize[0] / 2.0f, round(pPos[1]) - pSize[1] / 2.0f);
			return zVEC2(round(pos[0]), round(pos[1]));
		}

		void GetDrawViewport(int& px, int& py, int& sx, int& sy) const
		{
			zVEC2 pos = GetDrawPos();

			px = CoerceInRange<int>(pos[0], 0, 0, screen->psizex - 1);
			py = CoerceInRange<int>(pos[1], 0, 0, screen->psizey - 1);
			sx = CoerceInRange<int>(pSize[0], px, 1, screen->psizex);
			sy = CoerceInRange<int>(pSize[1], py, 1, screen->psizey);
		}

		void GetDrawMappings(zVEC2& start, zVEC2& end) const
		{
			zVEC2 drawPos = GetDrawPos();

			int vpPosX, vpPosY, vpSizeX, vpSizeY;
			GetDrawViewport(vpPosX, vpPosY, vpSizeX, vpSizeY);

			start[0] = (vpPosX - drawPos[0]) / pSize[0];
			start[1] = (vpPosY - drawPos[1]) / pSize[1];

			end[0] = start[0] + vpSizeX / pSize[0];
			end[1] = start[1] + vpSizeY / pSize[1];
		}

		void Move()
		{
			if (moved)
				return;

			float dx, dy, dw;
			zinput->GetMousePos(dx, dy, dw);

			pPos[0] += dx * Settings::MouseScale;
			pPos[1] += dy * Settings::MouseScale;

			pPos[0] = CoerceInRange<float>(pPos[0], 0, 0, screen->psizex - 1);
			pPos[1] = CoerceInRange<float>(pPos[1], 0, 0, screen->psizey - 1);
			
			moved = true;
		}

	public:
		CMouseDraw() :
			pPos(0),
			moved(false),
			enabled(false)
		{

		}

		void SetEnabled(bool enabled)
		{
			if (enabled == this->enabled)
				return;

			this->enabled = enabled;
			
			if (enabled)
				return Move();

			moved = false;
		}

		bool GetEnabled() const
		{
			return enabled;
		}

		void SetTexture(const string& texture)
		{
			this->texture = texture;
		}

		void SetPixelSize(int px, int py)
		{
			ASSERT(px > 0 && py > 0);
			pSize = zVEC2(px, py);
		}

		void SetPos(int vx, int vy)
		{
			pPos[0] = CoerceInRange(vx / 8192.0f * (screen->psizex - 1), 0.0f, 0.0f, screen->psizex - 1.0f);
			pPos[1] = CoerceInRange(vy / 8192.0f * (screen->psizey - 1), 0.0f, 0.0f, screen->psizey - 1.0f);
		}

		void GetPos(int& vx, int& vy) const
		{
			vx = round(pPos[0]) / screen->psizex * 8192.0f;
			vy = round(pPos[1]) / screen->psizey * 8192.0f;
		}

		void Draw()
		{
			if (!enabled || texture.IsEmpty())
				return;

			zSTRING name = texture;
			ZOwner<zCTexture> texture = (ZOwner<zCTexture>)zCTexture::Load(name, 0);
			if (!texture)
				return;

			auto oldZWrite = zrenderer->GetZBufferWriteEnabled();
			auto oldZCompare = zrenderer->GetZBufferCompare();
			auto oldBlend = zrenderer->GetAlphaBlendFunc();
			auto oldBillerp = zrenderer->GetBilerpFilterEnabled();

			zrenderer->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
			zrenderer->SetBilerpFilterEnabled(true);
			zrenderer->SetZBufferWriteEnabled(true);
			zrenderer->SetZBufferCompare(zRND_ZBUFFER_CMP_ALWAYS);

			const float farZ = zCCamera::activeCam ? zCCamera::activeCam->farClipZ - 1 : 65534.0f;

			int vpPosX, vpPosY, vpSizeX, vpSizeY;
			GetDrawViewport(vpPosX, vpPosY, vpSizeX, vpSizeY);
			zrenderer->SetViewport(vpPosX, vpPosY, vpSizeX, vpSizeY);

			zVEC2 uv1, uv2;
			GetDrawMappings(uv1, uv2);

			zrenderer->DrawTile(texture.get(), zVEC2(vpPosX, vpPosY), zVEC2(vpPosX + vpSizeX, vpPosY + vpSizeY), farZ, uv1, uv2, zCOLOR(255, 255, 255));
			
			zrenderer->SetZBufferWriteEnabled(oldZWrite);
			zrenderer->SetZBufferCompare(oldZCompare);
			zrenderer->SetAlphaBlendFunc(oldBlend);
			zrenderer->SetBilerpFilterEnabled(oldBillerp);
		}
	};

	std::vector<CScriptGui*> CScriptGui::guis;
	zCView* CScriptGui::view;

	std::vector<int> perframes;
	std::vector<std::unique_ptr<CScriptGui>> guis;
	int guisUsed = 0;
	bool initialized;
	std::vector<CScriptGui*> renderQueue;
	CMouseDraw mouse;

	CSubscription deleteGuis(ZSUB(Exit), []() { guis.clear(); });

	CSubscription init(ZSUB(LoadBegin), []()
		{
			if (initialized)
				return;

			CScriptGui::RegisterAsParserClass(parser, "C_GUI");

			for (int i = 0; i < parser->symtab.GetNumInList(); i++)
			{
				CSymbol symbol(parser, i);
				if (symbol.GetType() == CSymbol::Type::Func && (A symbol.GetSymbol()->name).EndWith("_GUIPERFRAME"))
					if (symbol.GetSymbol()->ele == 0)
						perframes.push_back(symbol.GetValue<int>(0));
			}

			initialized = true;
		});

	CSubscription callPerframes(ZSUB(PreLoop), []()
		{
			for (int address : perframes)
				parser->DoStack(address);
		});

	CSubscription draw(ZSUB(PostLoop), []()
		{
			if (!ogame || ogame->m_bWorldEntered)
				return;

			for (CScriptGui* gui : renderQueue)
				gui->Render();

			renderQueue.clear();
			CScriptGui::ResetAll();
			guisUsed = 0;

			mouse.Draw();
			mouse.SetEnabled(false);
		});

	CScriptGui* GetGui()
	{
		if (guisUsed == (int)guis.size())
			guis.push_back(std::make_unique<CScriptGui>());

		return guis[guisUsed++].get();
	}

	int __cdecl GUI_Create()
	{
		CParserScope scope(parser);

		CScriptGui* parent;
		int ctorIndex;
		ZPOP(parent, ctorIndex);

		CSymbol ctor(parser, ctorIndex);
		ASSERT(ctor.GetType() == CSymbol::Type::Instance || ctor.GetType() == CSymbol::Type::Prototype);

		CScriptGui* gui = GetGui();
		gui->SetParent(parent);

		{
			auto scope = AssignTemp(ctor.GetSymbol()->offset, (int)gui);
			zCPar_Symbol::SetUseInstance(ctor.GetSymbol());
			parser->DoStack(ctor.GetSymbol()->single_intdata);
		}

		ZPUSH(gui);
		return false;
	}

	int __cdecl GUI_Proc()
	{
		CParserScope scope(parser);

		CScriptGui* gui;
		ZPOP(gui);
		gui->Proc();

		return false;
	}

	int __cdecl GUI_Draw()
	{
		CScriptGui* gui;
		ZPOP(gui);
		renderQueue.push_back(gui);
		return false;
	}

	int __cdecl GUI_Anx()
	{
		CScriptGui* gui;
		int x;
		ZPOP(gui, x);
		ZPUSH(gui ? (int)round(gui->Anx(x)) : screen->anx(x));
		return false;
	}

	int __cdecl GUI_Any()
	{
		CScriptGui* gui;
		int y;
		ZPOP(gui, y);
		ZPUSH(gui ? (int)round(gui->Any(y)) : screen->any(y));
		return false;
	}

	int __cdecl GUI_Nax()
	{
		CScriptGui* gui;
		int x;
		ZPOP(gui, x);
		ZPUSH(gui ? (int)round(gui->Nax(x)) : screen->nax(x));
		return false;
	}

	int __cdecl GUI_Nay()
	{
		CScriptGui* gui;
		int y;
		ZPOP(gui, y);
		ZPUSH(gui ? (int)round(gui->Nay(y)) : screen->nay(y));
		return false;
	}

	int __cdecl GUI_EnableMouse()
	{
		string texture;
		int sizeX, sizeY;
		ZPOP(texture, sizeX, sizeY);

		mouse.SetEnabled(true);
		mouse.SetTexture(texture);
		mouse.SetPixelSize(sizeX, sizeY);

		return false;
	}

	int __cdecl GUI_IsMouseIn()
	{
		CScriptGui* gui;
		bool ignoreChilds;
		ZPOP(gui, ignoreChilds);

		if (mouse.GetEnabled())
		{
			int vx, vy;
			mouse.GetPos(vx, vy);
			ZPUSH(!gui || gui->IsIn(zVEC2(vx, vy), ignoreChilds));
		}
		else
			ZPUSH(false);

		return false;
	}

	int __cdecl GUI_SetMousePos()
	{
		int x, y;
		ZPOP(x, y);
		mouse.SetPos(x, y);
		return false;
	}

	int __cdecl GUI_GetMouseX()
	{
		int vx, vy;
		mouse.GetPos(vx, vy);
		ZPUSH(vx);
		return false;
	}

	int __cdecl GUI_GetMouseY()
	{
		int vx, vy;
		mouse.GetPos(vx, vy);
		ZPUSH(vy);
		return false;
	}

	ZEXTERNAL(GUI_Create, CScriptGui*, CScriptGui*, unsigned);
	ZEXTERNAL(GUI_Proc, void, CScriptGui*);
	ZEXTERNAL(GUI_Draw, void, CScriptGui*);

	ZEXTERNAL(GUI_Anx, int, CScriptGui*, int);
	ZEXTERNAL(GUI_Any, int, CScriptGui*, int);
	ZEXTERNAL(GUI_Nax, int, CScriptGui*, int);
	ZEXTERNAL(GUI_Nay, int, CScriptGui*, int);

	ZEXTERNAL(GUI_EnableMouse, void, string, int, int);
	ZEXTERNAL(GUI_IsMouseIn, bool, CScriptGui*, bool);
	ZEXTERNAL(GUI_SetMousePos, void, int, int);
	ZEXTERNAL(GUI_GetMouseX, int);
	ZEXTERNAL(GUI_GetMouseY, int);
}