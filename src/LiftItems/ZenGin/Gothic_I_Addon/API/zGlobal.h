// Supported with union (c) 2018 Union team

#ifndef __ZGLOBAL_H__VER1__
#define __ZGLOBAL_H__VER1__

namespace Gothic_I_Addon {
   extern zCEngine*& zengine;
   extern oCNpc*& player;
   extern zCObjectFactory*& zfactory;
   extern zCArchiverFactory* zarcFactory;
   extern zCOption*& zoptions;
   extern zCOption*& zgameoptions;
   extern zCParser* parser;
   extern zCTimer* ztimer;
   extern oCGame*& ogame;
   extern zCView*& screen;
   extern zCInput*& zinput;
   extern zCRenderer*& zrenderer;
   extern oCNpc*& stealnpc;
   extern CGameManager*& gameMan;
   extern zCLineCache* zlineCache;
   extern zERROR* zerr;
   extern zCSoundSystem*& zsound;
   extern zCMusicSystem*& zmusic;
   extern HINSTANCE& hInstApp;
   extern HDC& dcScreen;
   extern HICON& hIconApp;
   extern HWND& hWndApp;
} // namespace Gothic_II_Addon

#endif // __ZGLOBAL_H__VER1__