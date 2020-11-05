#include <unordered_set>

namespace NAMESPACE
{
#define ADDKEY(key) \
	nameToKey.insert(std::make_pair(A #key, key)); \
	keyToName.insert(std::make_pair(key, A #key));

	class CKeyHelper
	{
	private:
		std::unordered_multimap<string, int, CStringHasher, CStringEqualityComparer> nameToKey;
		std::unordered_map<int, string> keyToName;
		std::unordered_set<int> modifierKeys;

		void AddModifiers()
		{
			modifierKeys.insert(KEY_LSHIFT);
			modifierKeys.insert(KEY_RSHIFT);
			modifierKeys.insert(KEY_LALT);
			modifierKeys.insert(KEY_RALT);
			modifierKeys.insert(KEY_LCONTROL);
			modifierKeys.insert(KEY_RCONTROL);
		}

		void AddBasicKeys()
		{
			ADDKEY(KEY_ESCAPE)
			ADDKEY(KEY_1)
			ADDKEY(KEY_2)
			ADDKEY(KEY_3)
			ADDKEY(KEY_4)
			ADDKEY(KEY_5)
			ADDKEY(KEY_6)
			ADDKEY(KEY_7)
			ADDKEY(KEY_8)
			ADDKEY(KEY_9)
			ADDKEY(KEY_0)
			ADDKEY(KEY_MINUS)
			ADDKEY(KEY_EQUALS)
			ADDKEY(KEY_BACK)
			ADDKEY(KEY_TAB)
			ADDKEY(KEY_Q)
			ADDKEY(KEY_W)
			ADDKEY(KEY_E)
			ADDKEY(KEY_R)
			ADDKEY(KEY_T)
			ADDKEY(KEY_Y)
			ADDKEY(KEY_U)
			ADDKEY(KEY_I)
			ADDKEY(KEY_O)
			ADDKEY(KEY_P)
			ADDKEY(KEY_LBRACKET)
			ADDKEY(KEY_RBRACKET)
			ADDKEY(KEY_RETURN)
			ADDKEY(KEY_LCONTROL)
			ADDKEY(KEY_A)
			ADDKEY(KEY_S)
			ADDKEY(KEY_D)
			ADDKEY(KEY_F)
			ADDKEY(KEY_G)
			ADDKEY(KEY_H)
			ADDKEY(KEY_J)
			ADDKEY(KEY_K)
			ADDKEY(KEY_L)
			ADDKEY(KEY_SEMICOLON)
			ADDKEY(KEY_APOSTROPHE)
			ADDKEY(KEY_GRAVE)
			ADDKEY(KEY_LSHIFT)
			ADDKEY(KEY_BACKSLASH)
			ADDKEY(KEY_Z)
			ADDKEY(KEY_X)
			ADDKEY(KEY_C)
			ADDKEY(KEY_V)
			ADDKEY(KEY_B)
			ADDKEY(KEY_N)
			ADDKEY(KEY_M)
			ADDKEY(KEY_COMMA)
			ADDKEY(KEY_PERIOD)
			ADDKEY(KEY_SLASH)
			ADDKEY(KEY_RSHIFT)
			ADDKEY(KEY_MULTIPLY)
			ADDKEY(KEY_LMENU)
			ADDKEY(KEY_SPACE)
			ADDKEY(KEY_CAPITAL)
			ADDKEY(KEY_F1)
			ADDKEY(KEY_F2)
			ADDKEY(KEY_F3)
			ADDKEY(KEY_F4)
			ADDKEY(KEY_F5)
			ADDKEY(KEY_F6)
			ADDKEY(KEY_F7)
			ADDKEY(KEY_F8)
			ADDKEY(KEY_F9)
			ADDKEY(KEY_F10)
			ADDKEY(KEY_NUMLOCK)
			ADDKEY(KEY_SCROLL)
			ADDKEY(KEY_NUMPAD7)
			ADDKEY(KEY_NUMPAD8)
			ADDKEY(KEY_NUMPAD9)
			ADDKEY(KEY_SUBTRACT)
			ADDKEY(KEY_NUMPAD4)
			ADDKEY(KEY_NUMPAD5)
			ADDKEY(KEY_NUMPAD6)
			ADDKEY(KEY_ADD)
			ADDKEY(KEY_NUMPAD1)
			ADDKEY(KEY_NUMPAD2)
			ADDKEY(KEY_NUMPAD3)
			ADDKEY(KEY_NUMPAD0)
			ADDKEY(KEY_DECIMAL)
			ADDKEY(KEY_OEM_102)
			ADDKEY(KEY_F11)
			ADDKEY(KEY_F12)
			ADDKEY(KEY_F13)
			ADDKEY(KEY_F14)
			ADDKEY(KEY_F15)
			ADDKEY(KEY_KANA)
			ADDKEY(KEY_ABNT_C1)
			ADDKEY(KEY_CONVERT)
			ADDKEY(KEY_NOCONVERT)
			ADDKEY(KEY_YEN)
			ADDKEY(KEY_ABNT_C2)
			ADDKEY(KEY_NUMPADEQUALS)
			ADDKEY(KEY_PREVTRACK)
			ADDKEY(KEY_AT)
			ADDKEY(KEY_COLON)
			ADDKEY(KEY_UNDERLINE)
			ADDKEY(KEY_KANJI)
			ADDKEY(KEY_STOP)
			ADDKEY(KEY_AX)
			ADDKEY(KEY_UNLABELED)
			ADDKEY(KEY_NEXTTRACK)
			ADDKEY(KEY_NUMPADENTER)
			ADDKEY(KEY_RCONTROL)
			ADDKEY(KEY_MUTE)
			ADDKEY(KEY_CALCULATOR)
			ADDKEY(KEY_PLAYPAUSE)
			ADDKEY(KEY_MEDIASTOP)
			ADDKEY(KEY_VOLUMEDOWN)
			ADDKEY(KEY_VOLUMEUP)
			ADDKEY(KEY_WEBHOME)
			ADDKEY(KEY_NUMPADCOMMA)
			ADDKEY(KEY_DIVIDE)
			ADDKEY(KEY_SYSRQ)
			ADDKEY(KEY_RMENU)
			ADDKEY(KEY_PAUSE)
			ADDKEY(KEY_HOME)
			ADDKEY(KEY_UP)
			ADDKEY(KEY_PRIOR)
			ADDKEY(KEY_LEFT)
			ADDKEY(KEY_RIGHT)
			ADDKEY(KEY_END)
			ADDKEY(KEY_DOWN)
			ADDKEY(KEY_NEXT)
			ADDKEY(KEY_INSERT)
			ADDKEY(KEY_DELETE)
			ADDKEY(KEY_LWIN)
			ADDKEY(KEY_RWIN)
			ADDKEY(KEY_APPS)
			ADDKEY(KEY_POWER)
			ADDKEY(KEY_SLEEP)
			ADDKEY(KEY_WAKE)
			ADDKEY(KEY_WEBSEARCH)
			ADDKEY(KEY_WEBFAVORITES)
			ADDKEY(KEY_WEBREFRESH)
			ADDKEY(KEY_WEBSTOP)
			ADDKEY(KEY_WEBFORWARD)
			ADDKEY(KEY_WEBBACK)
			ADDKEY(KEY_MYCOMPUTER)
			ADDKEY(KEY_MAIL)
			ADDKEY(KEY_MEDIASELECT)
		}

		void AddExtendedKeys()
		{
			ADDKEY(KEY_BACKSPACE)
			ADDKEY(KEY_NUMPADSTAR)
			ADDKEY(KEY_LALT)
			ADDKEY(KEY_CAPSLOCK)
			ADDKEY(KEY_NUMPADMINUS)
			ADDKEY(KEY_NUMPADPLUS)
			ADDKEY(KEY_NUMPADPERIOD)
			ADDKEY(KEY_NUMPADSLASH)
			ADDKEY(KEY_RALT)
			ADDKEY(KEY_UPARROW)
			ADDKEY(KEY_PGUP)
			ADDKEY(KEY_LEFTARROW)
			ADDKEY(KEY_RIGHTARROW)
			ADDKEY(KEY_DOWNARROW)
			ADDKEY(KEY_PGDN)

			ADDKEY(KEY_CIRCUMFLEX)

			//ADDKEY(MAX_KEYS)
			//ADDKEY(MAX_KEYS_AND_CODES)
			//ADDKEY(KEY_NOT_HERE)
			ADDKEY(KEY_RELEASE)

			//ADDKEY(MIN_MOUSE_BTNS_AND_CODES)

			ADDKEY(MOUSE_DX)
			ADDKEY(MOUSE_DY)
			ADDKEY(MOUSE_UP)
			ADDKEY(MOUSE_DOWN)
			ADDKEY(MOUSE_LEFT)
			ADDKEY(MOUSE_RIGHT)
			ADDKEY(MOUSE_WHEELUP)
			ADDKEY(MOUSE_WHEELDOWN)
			ADDKEY(MOUSE_BUTTONLEFT)
			ADDKEY(MOUSE_BUTTONRIGHT)
			ADDKEY(MOUSE_BUTTONMID)
			ADDKEY(MOUSE_XBUTTON1)
			ADDKEY(MOUSE_XBUTTON2)
			ADDKEY(MOUSE_XBUTTON3)
			ADDKEY(MOUSE_XBUTTON4)
			ADDKEY(MOUSE_XBUTTON5)

			//ADDKEY(MAX_MOUSE_BTNS_AND_CODES)
			//ADDKEY(MIN_MOUSE_BTNS)
			//ADDKEY(MAX_MOUSE_BTNS)
			//ADDKEY(MIN_JOY_BTNS_AND_CODES)
			ADDKEY(JOY_LEFT)
			ADDKEY(JOY_RIGHT)
			ADDKEY(JOY_UP)
			ADDKEY(JOY_DOWN)
			ADDKEY(THROTTLE_UP)
			ADDKEY(THROTTLE_DOWN)
			ADDKEY(RUDDER_LEFT)
			ADDKEY(RUDDER_RIGHT)
			ADDKEY(JOY_POV_LEFT)
			ADDKEY(JOY_POV_RIGHT)
			ADDKEY(JOY_POV_UP)
			ADDKEY(JOY_POV_DOWN)
			ADDKEY(JOY_BUTTON_1)
			ADDKEY(JOY_BUTTON_2)
			ADDKEY(JOY_BUTTON_3)
			ADDKEY(JOY_BUTTON_4)
			ADDKEY(JOY_BUTTON_5)
			ADDKEY(JOY_BUTTON_6)
			ADDKEY(JOY_BUTTON_7)
			ADDKEY(JOY_BUTTON_8)
			ADDKEY(JOY_BUTTON_9)
			ADDKEY(JOY_BUTTON_10)
			ADDKEY(JOY_BUTTON_11)
			ADDKEY(JOY_BUTTON_12)
			ADDKEY(JOY_BUTTON_13)
			ADDKEY(JOY_BUTTON_14)
			ADDKEY(JOY_BUTTON_15)
			ADDKEY(JOY_BUTTON_16)
			ADDKEY(JOY_BUTTON_17)
			ADDKEY(JOY_BUTTON_18)
			ADDKEY(JOY_BUTTON_19)
			ADDKEY(JOY_BUTTON_20)
			ADDKEY(JOY_BUTTON_21)
			ADDKEY(JOY_BUTTON_22)
			ADDKEY(JOY_BUTTON_23)
			ADDKEY(JOY_BUTTON_24)
			ADDKEY(JOY_BUTTON_25)
			ADDKEY(JOY_BUTTON_26)
			ADDKEY(JOY_BUTTON_27)
			ADDKEY(JOY_BUTTON_28)
			ADDKEY(JOY_BUTTON_29)
			ADDKEY(JOY_BUTTON_30)
			ADDKEY(JOY_BUTTON_31)
			ADDKEY(JOY_BUTTON_32)
		}

		CKeyHelper()
		{
			AddModifiers();
			AddBasicKeys();
			AddExtendedKeys();
		}

	public:
		bool IsModifier(const int& key) const
		{
			return modifierKeys.find(key) != end(modifierKeys);
		}

		string KeyToName(const int& key) const
		{
			auto it = keyToName.find(key);
			if (it != end(keyToName))
				return it->second;
			return "";
		}

		int NameToKey(const string& name) const
		{
			auto it = nameToKey.find((A name).Upper());
			if (it != end(nameToKey))
				return it->second;
			return 0;
		}

		static CKeyHelper& GetInstance()
		{
			static CKeyHelper keyHelper;
			return keyHelper;
		}
	};

#undef ADDKEY
	}