META
{
	Parser = Menu;
	After = zUnionMenu.d;
	Namespace = MarvinHelper;
};

const int HEADLINE_Y = 550;
const int DY = 550;
const int TEXT_X = 450;
const int TEXT_Y = 2200;
const int CHOICE_X = 5500;
const int CHOICE_DIMX = 2200;
const int SLIDER_Y = 1500;

prototype C_MENU_ITEM_TEXT(C_MENU_ITEM)
{
	C_MENU_ITEM_DEF();
	fontname = MENU_FONT_SMALL;
	flags = flags | IT_EFFECTS_NEXT;
	posx = TEXT_X;
	posy = TEXT_Y;
	onselaction[0] = SEL_ACTION_UNDEF;
};

prototype C_MENU_ITEM_CHOICE(C_MENU_ITEM)
{
	C_MENU_ITEM_DEF();
	backpic = MENU_CHOICE_BACK_PIC;
	type = MENU_ITEM_CHOICEBOX;
	fontname = MENU_FONT_SMALL;
	flags = flags & ~IT_SELECTABLE;
    flags = flags | IT_TXT_CENTER;
	posx = CHOICE_X;
	posy = TEXT_Y;
	dimx = CHOICE_DIMX;
	onchgsetoptionsection = "Union_MarvinHelper";
	
	text[0] = Str_GetLocalizedString(
		"нет|да", 
		"no|yes", 
		"Nein|Ja", 
		"nie|tak"
	);
};

prototype C_MENU_ITEM_SLIDER(C_MENU_ITEM)
{
	C_MENU_ITEM_DEF();
	backpic = MENU_SLIDER_BACK_PIC;
	type = MENU_ITEM_SLIDER;
	userstring[0] = MENU_SLIDER_POS_PIC;
	userfloat[0] = 10.0;
	posx = CHOICE_X;
	posy = SLIDER_Y;
	dimx = CHOICE_DIMX;
	flags = flags & ~IT_SELECTABLE;
	onchgsetoptionsection = "Union_MarvinHelper";
};

instance :MenuItem_Union_Auto_MarvinHelper(C_MENU_ITEM_UNION_DEF)
{
	text[0] = "MarvinHelper";
	
	text[1] = Str_GetLocalizedString(
		"Настроить MarvinHelper",
		"Configure MarvinHelper",
		"Configure MarvinHelper",
		"Configure MarvinHelper"
	);
	
	onselaction[0] = SEL_ACTION_STARTMENU;
	onselaction_s[0] = "MarvinHelper:Menu_Options";
};

instance Menu_Options(C_MENU_DEF)
{
	backpic = "MENU_INGAME.TGA";
    items[0] = "";
    flags = flags | MENU_SHOW_INFO;
	Menu_SearchItems("MarvinHelper:MenuItem_*");
};

instance MenuItem_HeadLine(C_MENU_ITEM_TEXT)
{
	fontname = MENU_FONT_DEFAULT;
	flags = flags & ~IT_SELECTABLE;
    flags = flags | IT_TXT_CENTER;
	posx = 0;
	posy = HEADLINE_Y;
	dimx = 8192;
	
	text[0] = "MarvinHelper";
};

// instance MenuItem_ConsoleKeepHistoryCase_Text(C_MENU_ITEM_TEXT)
// {
	// posy += 0 * DY;
	
	// text[0] = Str_GetLocalizedString(
		// "Сохранять регистр символов команд", 
		// "Preserve letter case in command history", 
		// "Preserve letter case in command history", 
		// "Preserve letter case in command history"
	// );
	
	// text[1] = Str_GetLocalizedString(
		// "После изменения желателен перезапуск", 
		// "Restart desired after change", 
		// "Restart desired after change",
		// "Restart desired after change"
	// );
// };

// instance MenuItem_ConsoleKeepHistoryCase_Choice(C_MENU_ITEM_CHOICE)
// {
	// posy += 0 * DY;
	// onchgsetoption = "ConsoleKeepHistoryCase";
	// flags = flags | IT_NEEDS_RESTART;
// };

instance MenuItem_ExecuteStartupScript_Text(C_MENU_ITEM_TEXT)
{
	posy += 0 * DY;
	
	text[0] = Str_GetLocalizedString(
		"Выполнить скрипт при первой загрузке", 
		"Execute script after first load", 
		"Execute script after first load", 
		"Execute script after first load"
	);
};

instance MenuItem_ExecuteStartupScript_Choice(C_MENU_ITEM_CHOICE)
{
	posy += 0 * DY;
	onchgsetoption = "ExecuteStartupScript";
};

instance MenuItem_ActivateMarvinMode_Text(C_MENU_ITEM_TEXT)
{
	posy += 1 * DY;
	
	text[0] = Str_GetLocalizedString(
		"Включить тестовый режим сразу", 
		"Enable test mode after first load", 
		"Enable test mode after first load", 
		"Enable test mode after first load"
	);
};

instance MenuItem_ActivateMarvinMode_Choice(C_MENU_ITEM_CHOICE)
{
	posy += 1 * DY;
	onchgsetoption = "ActivateMarvinMode";
};

instance MenuItem_ConsoleFreezePlayer_Text(C_MENU_ITEM_TEXT)
{
	posy += 2 * DY;
	
	text[0] = Str_GetLocalizedString(
		"Заморозка игрока при открытой консоли", 
		"Freeze player when console is open", 
		"Freeze player when console is open", 
		"Freeze player when console is open"
	);
};

instance MenuItem_ConsoleFreezePlayer_Choice(C_MENU_ITEM_CHOICE)
{
	posy += 2 * DY;
	onchgsetoption = "ConsoleFreezePlayer";
};

instance MenuItem_ConsoleShowHints_Text(C_MENU_ITEM_TEXT)
{
	posy += 3 * DY;
	
	text[0] = Str_GetLocalizedString(
		"Отображать подсказки консоли", 
		"Show console hints", 
		"Show console hints", 
		"Show console hints"
	);
};

instance MenuItem_ConsoleShowHints_Choice(C_MENU_ITEM_CHOICE)
{
	posy += 3 * DY;
	onchgsetoption = "ConsoleShowHints";
};

instance MenuItem_ConsoleEditItemAmount_Text(C_MENU_ITEM_TEXT)
{
	posy += 4 * DY;

	text[0] = Str_GetLocalizedString(
		"Редактировать количество предметов", 
		"Add item amount for edit command", 
		"Add item amount for edit command", 
		"Add item amount for edit command"
	);
};

instance MenuItem_ConsoleEditItemAmount_Choice(C_MENU_ITEM_CHOICE)
{
	posy += 4 * DY;
	onchgsetoption = "ConsoleEditItemAmount";
};

instance MenuItem_ActivateParserExtenderExternals_Text(C_MENU_ITEM_TEXT)
{
	posy += 5 * DY;

	text[0] = Str_GetLocalizedString(
		"Активировать функции zParserExtender", 
		"Activate zParserExtender functions", 
		"Activate zParserExtender functions", 
		"Activate zParserExtender functions"
	);
};

instance MenuItem_ActivateParserExtenderExternals_Choice(C_MENU_ITEM_CHOICE)
{
	posy += 5 * DY;
	onchgsetoption = "ActivateParserExtenderExternals";
};

instance MenuItem_Back(C_MENU_ITEM_DEF)
{
	Union_MenuItem_Back();
};