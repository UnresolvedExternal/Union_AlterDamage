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

		void ConvertViewportFromParent(zVEC2& pos, zVEC2& size)
		{
			pos[0] += (float)GetVPosX() / 8192.0f * size[0];
			pos[1] += (float)GetVPosY() / 8192.0f * size[1];
			size[0] *= (float)GetVSizeX() / 8192.0f;
			size[1] *= (float)GetVSizeY() / 8192.0f;
		}

		void RenderImpl(zVEC2 pos, zVEC2 size)
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
				CSymbolHelper itemSymbol(parser, vars.item);
				ASSERT(itemSymbol.GetType() == CSymbolHelper::Type::Instance);
				ZOwner<oCItem> renderItem = (ZOwner<oCItem>)zfactory->CreateItem(vars.item);
				renderItem->RotateForInventory(1);
				renderItem->RenderItem(renderWorld.get(), view, -2.0f);
			}

			if (vars.text.Length())
			{
				view->SetFont(vars.font);

				zVEC2 textPos(0.0f);
				const zVEC2 textSize(view->FontSize(vars.text), view->FontY());

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

			CSymbolHelper func(parser, vars.proc);
			
			ASSERT(func.GetType() == CSymbolHelper::Type::Func);
			ASSERT(func.GetSymbol()->offset == zPAR_TYPE_INT);
			ASSERT(func.GetSymbol()->ele == 1);
			ASSERT(CSymbolHelper(parser, func.GetIndex() + 1).GetSymbol()->type == zPAR_TYPE_INSTANCE);

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
			view->Blit();

			std::vector<CScriptGui*> ancestors;
			for (CScriptGui* gui = parent; gui; gui = gui->parent)
				ancestors.push_back(gui);

			zVEC2 pos(0.0f, 0.0f), size(8192.0f, 8192.0f);
			for (auto it = ancestors.rbegin(); it != ancestors.rend(); it++)
				(*it)->ConvertViewportFromParent(pos, size);

			RenderImpl(pos, size);
			screen->RemoveItem(view);
			delete view;
		}

		static void RegisterAsParserClass(zCParser* parser, const string& className)
		{
			CSymbolHelper classSymbol(parser, className);

			if (classSymbol.GetType() == CSymbolHelper::Type::Class)
			{
				ASSERT(classSymbol.GetSymbol()->offset == sizeof(CScriptGui::vars));

				int deltaOffset = (int)NdrFieldOffset(CScriptGui, vars) - classSymbol.GetSymbol()->single_intdata;
				
				if (deltaOffset == 0)
					return;

				for (int i = 1; i <= (int)classSymbol.GetSymbol()->ele; i++)
					CSymbolHelper(parser, classSymbol.GetIndex() + i).GetSymbol()->offset += deltaOffset;
			}
		}

		~CScriptGui()
		{
			guis.erase(std::find(guis.begin(), guis.end(), this));
		}
	};

	std::vector<CScriptGui*> CScriptGui::guis;
	zCView* CScriptGui::view;

	std::vector<int> perframes;
	std::vector<std::unique_ptr<CScriptGui>> guis;
	int guisUsed = 0;
	bool initialized;
	std::vector<CScriptGui*> renderQueue;

	CSubscription init(ZSUB(LoadBegin), []()
		{
			if (initialized)
				return;

			CScriptGui::RegisterAsParserClass(parser, "C_GUI");

			for (int i = 0; i < parser->symtab.GetNumInList(); i++)
			{
				CSymbolHelper symbol(parser, i);
				if (symbol.GetType() == CSymbolHelper::Type::Func && (A symbol.GetSymbol()->name).EndWith("_GUIPERFRAME"))
					if (symbol.GetSymbol()->ele == 0)
						perframes.push_back(symbol.GetValue<int>(0));
			}

			initialized = true;
		});

	CSubscription deleteGuis(ZSUB(Exit), []()
		{
			guis.clear();
		});

	CSubscription loop(ZSUB(Loop), []()
		{
			if (ogame->singleStep)
				return;

			TScope<int> sequelFix;
			zCPar_Symbol* instance_help = parser->GetSymbol(parser->instance_help);
			if (instance_help)
				sequelFix = TScope<int>(instance_help->offset);

			for (int address : perframes)
				parser->DoStack(address);
		});

	void __cdecl Hook__oCarsten_PerFrame();
	CInvoke<void(__cdecl*)()> Ivk__oCarsten_PerFrame(ZenDef<TInstance>(0x00479E80, 0x00483F60, 0x004801E0, 0x004816C0), &Hook__oCarsten_PerFrame, IvkEnabled(ENGINE));
	void __cdecl Hook__oCarsten_PerFrame()
	{
		Ivk__oCarsten_PerFrame();

		for (CScriptGui* gui : renderQueue)
			gui->Render();

		renderQueue.clear();
		CScriptGui::ResetAll();
		guisUsed = 0;
	}

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

		CSymbolHelper ctor(parser, ctorIndex);
		ASSERT(ctor.GetType() == CSymbolHelper::Type::Instance || ctor.GetType() == CSymbolHelper::Type::Prototype);

		CScriptGui* gui = GetGui();
		gui->SetParent(parent);

		{
			auto scope = AssignTemp(ctor.GetSymbol()->offset, (int)gui);
			zCPar_Symbol::SetUseInstance(ctor.GetSymbol());
			parser->DoStack(ctor.GetSymbol()->single_intdata);
		}

		parser->SetReturn((void*)gui);
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
		CParserScope scope(parser);
		CScriptGui* gui;
		ZPOP(gui);
		renderQueue.push_back(gui);
		return false;
	}

	int __cdecl GUI_Anx()
	{
		CParserScope scope(parser);
		CScriptGui* gui;
		int x;
		ZPOP(gui, x);
		parser->SetReturn(gui ? (int)round(gui->Anx(x)) : screen->anx(x));
		return false;
	}

	int __cdecl GUI_Any()
	{
		CParserScope scope(parser);
		CScriptGui* gui;
		int y;
		ZPOP(gui, y);
		parser->SetReturn(gui ? (int)round(gui->Any(y)) : screen->any(y));
		return false;
	}

	int __cdecl GUI_Nax()
	{
		CParserScope scope(parser);
		CScriptGui* gui;
		int x;
		ZPOP(gui, x);
		parser->SetReturn(gui ? (int)round(gui->Nax(x)) : screen->nax(x));
		return false;
	}

	int __cdecl GUI_Nay()
	{
		CParserScope scope(parser);
		CScriptGui* gui;
		int y;
		ZPOP(gui, y);
		parser->SetReturn(gui ? (int)round(gui->Nay(y)) : screen->nay(y));
		return false;
	}

	ZEXTERNAL(GUI_Create, CScriptGui*, CScriptGui*, unsigned);
	ZEXTERNAL(GUI_Proc, void, CScriptGui*);
	ZEXTERNAL(GUI_Draw, void, CScriptGui*);

	ZEXTERNAL(GUI_Anx, int, CScriptGui*, int);
	ZEXTERNAL(GUI_Any, int, CScriptGui*, int);
	ZEXTERNAL(GUI_Nax, int, CScriptGui*, int);
	ZEXTERNAL(GUI_Nay, int, CScriptGui*, int);
}