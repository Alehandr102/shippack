// ============================================================================
//  ПЕРЕГОВОРЫ В МОРЕ (Speak) - интерфейс. Логика/расчёты: scripts\seaspeak.c.
//  Форма: RESOURCE\INI\INTERFACES\Speak.ini (ноды INFO_TEXT, ANSWERS,
//  RANSOM_ANSWERS, HELP, PORTRAIT_PICTURE, EXIT_BTN -> event:exitCancel).
//
//  УСТРОЙСТВО МЕНЮ: пункты собираются динамически в RebuildMainMenu() -
//  каждый пункт получает строковый id в aAnswerId[]; обработка - по id в
//  RecognizeAnswer(). Новая нода = AddAnswer("my_id", текст) + case "my_id".
//  Недоступные действия в меню просто не попадают (никаких серых пунктов).
//
//  Подменю подтверждения (RANSOM_ANSWERS) - ShowSubMenu(текст, да/нет),
//  смысл ответа задаёт sSubMode; обработка в RecognizeRansomAnswer().
//
//  Исход переговоров применяется на ВЫХОДЕ (IDoExit) по iSpeakFlag:
//  SPEAK_RES_NEUTRAL - враг откупился (Ship_Neutral), SPEAK_RES_TAX - мы
//  откупились (группа нейтралится), SPEAK_RES_ENEMY - переговоры провалены.
// ============================================================================

#define SPEAK_RES_NOTHING	0
#define SPEAK_RES_NEUTRAL	1
#define SPEAK_RES_ENEMY		2
#define SPEAK_RES_TAX		3
#define SPEAK_RES_CAPTURE	4

string totalInfo = "";

int iSpeakFlag = SPEAK_RES_NOTHING;

string sTarget = "";
int iWhoSpeak = -1;
ref rSpeakCharacter;
string sGlobalMode = "";

// главное меню (нод ANSWERS): id и количество пунктов
string aAnswerId[10];
int iAnswersCount = 0;
// подменю (нод RANSOM_ANSWERS): что значит ответ "да"
string sSubMode = "";
int iSubCount = 0;
// откуп: за всю эскадру (true) либо только за корабль собеседника (false)
bool bTaxAll = true;

// LEO: гейт приёма ответа при ВХОДЯЩЕМ сигнале (нам сигналят) — первые N секунд игнорим выбор,
// чтобы маш по кнопкам морского боя не выбрал ответ вслепую. Реальное время копится покадрово
// (Speak_FrameTick через событие "frame" — тикает даже при паузе SetTimeScale(0)).
int iSpeakInBlockMs = 0; // длительность гейта (мс), 0 = гейта нет (исходящий вызов / отключено)
int iSpeakInElapsed = 0; // накоплено реального времени с открытия интерфейса (мс)

void Speak_FrameTick()
{
	int d = iRealDeltaTime;
	if (d > 100) d = 100; // не засчитывать хитч загрузки интерфейса как «прошло время»
	iSpeakInElapsed = iSpeakInElapsed + d;
}
bool Speak_InputBlocked()
{
	if (iSpeakInBlockMs <= 0) return false;
	if (iSpeakInElapsed < iSpeakInBlockMs) return true;
	return false;
}

// LEO: пасхалка «ИСПАНЦЫ» — при ВХОДЯЩЕМ требовании дани (sMode "tax") от испанцев играем spaniards.ogg.
// 5% шанс на каждое такое требование (rand — полная случайность, НЕ dRand), один раз за всю игру
// (флаг pchar.easter.spaniards ставится только при срабатывании → дальше не повторяется).
void Speak_SpaniardsEasterEgg()
{
	if (sti(rSpeakCharacter.nation) != SPAIN) return;       // только испанцы
	if (CheckAttribute(pchar, "easter.spaniards")) return;  // уже сыграла за эту игру
	if (rand(100) < 5)
	{
		pchar.easter.spaniards = 1;
		PlaySound("Interface\spaniards.ogg");
	}
}

void InitInterface_RIS(string iniName, aref chr, int whospeak, string sMode)
{
	EngineLayersOffOn(true);
	SetTimeScale(0.0);
	sGlobalMode = sMode;
	rSpeakCharacter = CharacterFromID(chr.id);
	sTarget = chr.id;
	iWhoSpeak = whospeak;
	iSpeakFlag = SPEAK_RES_NOTHING;

	GameInterface.title = "titleSpeak";

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetEventHandler("InterfaceBreak","ProcessCancelExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);
	SetEventHandler("FTChange","FTChange",0);

	CalculateInfoData();
	SetInfoText(totalInfo);

	RebuildMainMenu();

	// режимы, когда корабль заговорил сам (входящий вызов из Speak_EnemyInitiative)
	switch (sMode)
	{
		case "ransom": // враг сам предлагает откупиться
			DemandRansom();
		break;
		case "tax": // враг требует дань: ультиматум, выбор корабль/эскадра - в меню
			TaxDemandIntro();
			Speak_SpaniardsEasterEgg(); // LEO: пасхалка ИСПАНЦЫ (spaniards.ogg) при требовании дани от испанцев
		break;
		case "surrender": // враг сам сдаёт корабль
			OfferSurrenderIncoming();
		break;
	}

	// заголовки: верхний - по инициатору (нам сигналят / мы сигналим), нижний - "Выбор ответного сигнала"
	if (iWhoSpeak == sti(pchar.index)) SetFormatedText("MAIN_WINDOW_CAPTION", XI_ConvertString("Speak_TitleOut"));
	else SetFormatedText("MAIN_WINDOW_CAPTION", XI_ConvertString("Speak_TitleIn"));
	SetFormatedText("MAIN_WINDOW_SUBCAPTION", XI_ConvertString("Speak_SubReply"));

	// LEO: задержка приёма ответа при ВХОДЯЩЕМ сигнале (нам сигналят, iWhoSpeak != ГГ). Использует
	// общую настройку задержки диалога (тоггл DialogDelay + слайдер), но с полом 1с — морской бой суматошный.
	iSpeakInBlockMs = 0;
	iSpeakInElapsed = 0;
	if (iWhoSpeak != sti(pchar.index) && sti(InterfaceStates.DialogDelay) != 0)
	{
		float fSpkDelay = stf(InterfaceStates.DialogInputDelay);
		if (fSpkDelay < 1.0) fSpkDelay = 1.0;
		float fSpkMs = fSpkDelay * 1000.0; // ВАЖНО: makeint(a*b) с арифметикой внутри валит скрипт-функцию - считаем в переменную
		iSpeakInBlockMs = makeint(fSpkMs);
		SetEventHandler("frame", "Speak_FrameTick", 1);
	}

	// фон-картинка отключён
	SetNodeUsing("MAIN_WINDOW_BACK_PICTURE", false);

	SetNewPicture("PORTRAIT_PICTURE", "interfaces\portraits\face_" + rSpeakCharacter.FaceId + ".tga");

	// карточка корабля собеседника: картинка (как в верфи) + имя/тип/класс текстом
	int iSpkShip = sti(rSpeakCharacter.ship.type);
	if (iSpkShip != SHIP_NOTUSED)
	{
		ref rSpkBase = GetRealShip(iSpkShip);
		SetNewPicture("SHIP_PIC", InterfaceStates.InterfaceShipString + rSpkBase.BaseName + ".dds");
		SetFormatedText("SHIP_INFO", XI_ConvertString(rSpkBase.BaseName) + " (" + XI_ConvertString("Speak_ShipClass") + " " + rSpkBase.Class + ")");
	}
	else
	{
		SetNewPicture("SHIP_PIC", "interfaces\blank_ship2.dds");
		SetFormatedText("SHIP_INFO", "");
	}

	// значок нации собеседника под портретом (пиклист NATIONS; для экзотики - прячем)
	int iSpkNation = sti(rSpeakCharacter.nation);
	string sNatPic = "";
	if (iSpkNation == ENGLAND) sNatPic = "England";
	if (iSpkNation == FRANCE)  sNatPic = "France";
	if (iSpkNation == HOLLAND) sNatPic = "Holland";
	if (iSpkNation == SPAIN)   sNatPic = "Spain";
	if (iSpkNation == PIRATE)  sNatPic = "Pirate";
	if (sNatPic != "")
	{
		SetNodeUsing("NATION_PIC", true);
		SetNewGroupPicture("NATION_PIC", "NATIONS", sNatPic);
	}
	else
	{
		SetNodeUsing("NATION_PIC", false);
	}

	ShowCaptainInfo();
	// нижняя плашка-подсказка (HELP) убрана: "Выберите тему" уже есть в заголовке нижней панели
}

// мораль команды собеседника под портретом: подсказка, дозрел ли он до контрибуции.
// Как на корабле ГГ: стандартное название градации (GetMoraleName), иконка (MORALE_SMALL)
// и бар (STATUSLINE4). Показываем ЖИВУЮ боевую мораль (seatime.tempmorale), если бой её
// уже потрепал; иначе - базовую Ship.Crew.Morale.
void ShowCaptainInfo()
{
	int iMorale = 45;
	if (CheckAttribute(rSpeakCharacter, "Ship.Crew.Morale")) iMorale = sti(rSpeakCharacter.Ship.Crew.Morale);
	if (CheckAttribute(rSpeakCharacter, "seatime.tempmorale")) iMorale = makeint(stf(rSpeakCharacter.seatime.tempmorale));
	if (iMorale < 0) iMorale = 0;
	if (iMorale > 100) iMorale = 100;
	SetFormatedText("CAPTAIN_INFO", XI_ConvertString("Speak_MoraleTitle") + " " + XI_ConvertString(GetMoraleName(iMorale)) + " (" + iMorale + ")");
	SetNewGroupPicture("MORALE_PIC", "MORALE_SMALL", GetMoraleGroupPicture(makefloat(iMorale)));
	GameInterface.StatusLine.MORALE_BAR.Max   = 100;
	GameInterface.StatusLine.MORALE_BAR.Min   = 0;
	GameInterface.StatusLine.MORALE_BAR.Value = iMorale;
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, "MORALE_BAR", 0);
}

// ---------------------------------------------------------------------------
//  ГЛАВНОЕ МЕНЮ
// ---------------------------------------------------------------------------

void AddAnswer(string sId, string sText)
{
	if (iAnswersCount >= 10) return;
	aAnswerId[iAnswersCount] = sId;
	iAnswersCount = iAnswersCount + 1;
	sText = " " + sText;
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"ANSWERS", 0,&sText);
}

void RebuildMainMenu()
{
	int i;
	// очистка строк нода и списка id
	SetFormatedText("ANSWERS", "");
	iAnswersCount = 0;

	int iRelation = GetRelation(sti(rSpeakCharacter.index), nMainCharacterIndex);

	if (iRelation != RELATION_ENEMY)
	{
		// мирный собеседник: новости и торговля
		if (!CheckAttribute(rSpeakCharacter, "speak.news")) AddAnswer("news", XI_ConvertString("Speak_news"));
		AddAnswer("trade", XI_ConvertString("Speak_trade"));
	}
	else
	{
		// враг: потребовать контрибуцию (платит он; попытки многоразовые - вдруг капитан дозрел) /
		// откупиться золотом (платим мы): от ЭТОГО корабля либо от всей эскадры разом, суммы в пунктах.
		// Квестовые охотники переговоров не ведут; исключение - ОЗГ и патруль контрафрахта (только откуп)
		bool bTaxAllowed = false;
		if (!Speak_IsQuestHunter(rSpeakCharacter))
		{
			// повтор требования - не раньше следующего игрового часа (иначе можно
			// закликать отказ до победного: внутри проверки морали есть рандом)
			bool bAskedThisHour = false;
			int iRansomHour = DateToInt(0) * 24 + GetHour();
			if (CheckAttribute(rSpeakCharacter, "speak.ransomhour"))
			{
				if (iRansomHour - sti(rSpeakCharacter.speak.ransomhour) < 1) bAskedThisHour = true;
			}
			if (!bAskedThisHour) AddAnswer("ransom", XI_ConvertString("Speak_ransom"));
			bTaxAllowed = true;

			// потребовать полной сдачи корабля (ролл по авторитету+обаянию, раз в игровой час)
			bool bSurrThisHour = false;
			int iSurrHour = DateToInt(0) * 24 + GetHour();
			if (CheckAttribute(rSpeakCharacter, "speak.surrenderhour"))
			{
				if (iSurrHour - sti(rSpeakCharacter.speak.surrenderhour) < 1) bSurrThisHour = true;
			}
			if (!bSurrThisHour) AddAnswer("surrender", XI_ConvertString("Speak_surrender"));
		}
		else
		{
			if (Speak_IsTaxOnly(rSpeakCharacter)) bTaxAllowed = true;
		}
		if (bTaxAllowed && CheckAttribute(rSpeakCharacter, "speak.tax")) bTaxAllowed = false; // от этого уже откупились
		if (bTaxAllowed)
		{
			if (Speak_GroupAliveCount(rSpeakCharacter) > 1)
			{
				// LEO: откуп от ОДНОГО корабля эскадры — только с перком «Вызывающий доверие» (Trustworthy); без перка — лишь вся эскадра разом
				if (CheckCharacterPerk(pchar, "Trustworthy"))
					AddAnswer("tax_ship", XI_ConvertString("Speak_tax_ship") + " (" + CalculateTaxShip(rSpeakCharacter) + ")");
				AddAnswer("tax_all", XI_ConvertString("Speak_tax_all") + " (" + CalculateTax(rSpeakCharacter) + ")");
			}
			else
			{
				AddAnswer("tax_all", XI_ConvertString("Speak_tax") + " (" + CalculateTax(rSpeakCharacter) + ")");
			}
		}
	}
	AddAnswer("cancel", XI_ConvertString("Speak_cancel"));

	SetNodeUsing("RANSOM_ANSWERS", false);
	SetNodeUsing("ANSWERS", true);
	SetCurrentNode("ANSWERS");
	PaintAnswers("ANSWERS", iAnswersCount, 0);
}

// перекраска строк меню: все белые, выбранная - жёлтая
void PaintAnswers(string sNode, int iCount, int iSelected)
{
	int i;
	int iColor;
	for (i = 0; i < iCount; i++)
	{
		iColor = argb(255,255,255,255);
		if (i == iSelected) iColor = argb(255,255,255,128);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,sNode, 8,i,iColor);
	}
}

void SetInfoText(string sText)
{
	SetFormatedText("INFO_TEXT", sText);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",5);
}

// приветствие по отношению к ГГ
void CalculateInfoData()
{
	int iRelation = GetRelation(sti(rSpeakCharacter.index), nMainCharacterindex);

	if (iWhoSpeak == sti(pchar.index))
	{
		if (iRelation == RELATION_ENEMY)  totalInfo = XI_ConvertString("Speak_1");
		if (iRelation == RELATION_FRIEND) totalInfo = XI_ConvertString("Speak_2");
		if (iRelation == RELATION_NEUTRAL) totalInfo = XI_ConvertString(GetSexPhrase("Speak_3", "Speak_3_woman")); // LEO: обращение к ГГ по полу — «Сэр»/«Мэм»
		if (iRelation == RELATION_ENEMY)
		{
			// квестовый охотник: на контракте, разговор короткий (ОЗГ и патруль контрафрахта -
			// исключение, им можно предложить золото)
			if (Speak_IsQuestHunter(rSpeakCharacter) && !Speak_IsTaxOnly(rSpeakCharacter)) totalInfo = XI_ConvertString("Speak_6");
		}
		else
		{
			if (CheckAttribute(rSpeakCharacter, "issmuggler")) totalInfo = XI_ConvertString("Speak_smg");
		}
	}
	else
	{
		totalInfo = XI_ConvertString("Speak_23");
	}
}

// ---------------------------------------------------------------------------
//  ОБРАБОТКА ВЫБОРА
// ---------------------------------------------------------------------------

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	if (Speak_InputBlocked()) return; // LEO: первые N сек входящего сигнала выбор ответа игнорируется (крестик/Esc работают)

	switch (nodName)
	{
		case "ANSWERS":
			if (comName=="activate" || comName=="click" || comName=="dblclick")
			{
				RecognizeAnswer();
			}
		break;
		case "RANSOM_ANSWERS":
			if (comName=="activate" || comName=="click")
			{
				RecognizeRansomAnswer();
			}
		break;
	}
}

void RecognizeAnswer()
{
	int iStep = SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"ANSWERS", 11);
	if (iStep < 0) return;
	if (iStep >= iAnswersCount) return;
	string sId = aAnswerId[iStep];

	switch (sId)
	{
		case "news":
			CalculateNews();
		break;
		case "trade":
			if (SeaTrade_RepTooLow()) SetInfoText(XI_ConvertString("Speak_TradeRepBad")); // LEO: плохая репутация ГГ -> капитан отказывает в торговле
			else Trade();
		break;
		case "ransom":
			DemandRansom();
		break;
		case "surrender":
			DemandSurrender();
		break;
		case "tax_ship":
			bTaxAll = false;
			OfferTax();
		break;
		case "tax_all":
			bTaxAll = true;
			OfferTax();
		break;
		case "cancel":
			ProcessCancelExit();
		break;
	}
}

// ---------------------------------------------------------------------------
//  ПОДМЕНЮ (да/нет либо одно "Ок")
// ---------------------------------------------------------------------------

void ShowSubMenu(string sText, bool bYesNo, string sMode)
{
	sSubMode = sMode;
	string sAnswerText = "";
	SetFormatedText("RANSOM_ANSWERS", sAnswerText);

	SetNodeUsing("ANSWERS", false);
	SetNodeUsing("RANSOM_ANSWERS", true);

	if (bYesNo)
	{
		sAnswerText = " " + XI_ConvertString("Yes");
		SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"RANSOM_ANSWERS", 0,&sAnswerText);
		sAnswerText = " " + XI_ConvertString("No");
		SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"RANSOM_ANSWERS", 0,&sAnswerText);
		iSubCount = 2;
	}
	else
	{
		sAnswerText = " " + XI_ConvertString("Cancel");
		SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"RANSOM_ANSWERS", 0,&sAnswerText);
		iSubCount = 1;
	}
	PaintAnswers("RANSOM_ANSWERS", iSubCount, 0);
	SetCurrentNode("RANSOM_ANSWERS");

	SetInfoText(sText);
}

void RecognizeRansomAnswer()
{
	int iStep = SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"RANSOM_ANSWERS", 11);
	bool bYes = false;
	if (iStep == 0 && iSubCount == 2) bYes = true; // "Да" есть только в двухкнопочном подменю

	if (bYes)
	{
		switch (sSubMode)
		{
			case "ransom_take": // враг откупается контрибуцией
				AgreeRansom();
			break;
			case "tax_pay": // мы платим за мирное расхождение
				OkTax();
			break;
			case "surrender_accept": // враг сам предложил сдаться - принимаем
				AcceptIncomingSurrender();
			break;
		}
	}
	else
	{
		BackToMainMenu();
	}
}

void BackToMainMenu()
{
	sSubMode = "";
	CalculateInfoData();
	SetInfoText(XI_ConvertString("Speak_4")); // "Есть что-нибудь ещё, что вы желали бы обсудить?"
	ShowCaptainInfo(); // мораль могла измениться по ходу разговора
	RebuildMainMenu();
}

// ---------------------------------------------------------------------------
//  НОВОСТИ
// ---------------------------------------------------------------------------

void CalculateNews()
{
	string sText = "";
	SetFormatedText("INFO_TEXT", "");
	DeleteAttribute(&GameInterface,"INFO_TEXT");

	int iColor = argb(255,255,192,255);

	if (!CheckAttribute(rSpeakCharacter, "speak.news"))
	{
		sText = XI_ConvertString("Speak_News1") + SelectRumourExSpecial("capitan", rSpeakCharacter);
		rSpeakCharacter.speak.news = 1; // новости - один раз за встречу
	}
	else
	{
		sText = XI_ConvertString("Speak_No_News");
	}

	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT", 0,&sText);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT", 8,-1,iColor);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",5);

	RebuildMainMenu(); // пункт "Последние новости" исчезает из меню
}

// ---------------------------------------------------------------------------
//  ТОРГОВЛЯ (магазин корабля; выход с переходом в интерфейс магазина)
// ---------------------------------------------------------------------------

void Trade()
{
	FillShipStore(rSpeakCharacter);
	pchar.shiptrade.character = rSpeakCharacter.id;
	IDoExit(RC_INTERFACE_SPEAK_EXIT_AND_TRADE, true);
}

// ---------------------------------------------------------------------------
//  КОНТРИБУЦИЯ: требуем с врага плату за то, что отпускаем его с миром
// ---------------------------------------------------------------------------

void DemandRansom()
{
	// попытки многоразовые: капитан мог дозреть по ходу боя (но за наглость без повода - реп-штраф)
	string sText = "";

	bool bAgree = false;
	if (sGlobalMode == "ransom") bAgree = true; // сам вышел на связь с предложением - мораль не переспрашиваем
	if (!bAgree) bAgree = CalculateForRansom(rSpeakCharacter);
	if (bAgree)
	{
		// мораль на пределе - готов платить
		int iSumm;
		if (sGlobalMode == "ransom") iSumm = CalculateSummSelf(rSpeakCharacter); // враг САМ предложил - от ЕГО положения (казна+отчаяние), не от сил эскадр
		else iSumm = CalculateSumm(rSpeakCharacter);                              // мы требуем контрибуцию - по соотношению сил
		sText = XI_ConvertString("Speak_11") + iSumm + XI_ConvertString("Speak_12");
		ShowSubMenu(sText, true, "ransom_take");
	}
	else
	{
		// держится крепко - посылает; переспросить можно через игровой час
		rSpeakCharacter.speak.ransomhour = DateToInt(0) * 24 + GetHour();
		sText = XI_ConvertString("Speak_6");
		if (rand(1) == 1) sText = XI_ConvertString("Speak_17");
		ShowSubMenu(sText, false, "");
		ChangeCharacterReputation(pchar, -1); // вымогательство не красит
	}
}

// ---------------------------------------------------------------------------
//  ПОЛНАЯ СДАЧА: требуем спустить флаг. Чистый ролл воли (авторитет+обаяние ГГ против
//  капитана, seaspeak.c). Удача - корабль сдаётся целиком с командой; капитана можно взять
//  в плен, НО не нанять офицером (сдался под давлением, а не примкнул добровольно).
// ---------------------------------------------------------------------------
void DemandSurrender()
{
	rSpeakCharacter.speak.surrenderhour = DateToInt(0) * 24 + GetHour(); // ролл раз в игровой час

	if (Speak_TryDemandSurrender(rSpeakCharacter))
	{
		// сдавшегося капитана нанять в офицеры МОЖНО (запрет только на ДУ/каперов фрахта)
		Ship_Surrender(sti(rSpeakCharacter.index)); // белый флаг + дрейф; грабёж откроется при сближении (AIShip.c)
		IDoExit(RC_INTERFACE_SPEAK_EXIT, true);
	}
	else
	{
		// воли не хватило - капитан дерзит; переспросить можно через игровой час.
		// Реплику подбираем по тому, требовали ли сдачи раньше: первый отказ - без намёка
		// на прошлый разговор, повторный - "я уже сказал" (иначе нелогично при первой попытке)
		string sText;
		if (!CheckAttribute(rSpeakCharacter, "speak.surrtried"))
		{
			int iNoVar = rand(2);
			if (iNoVar == 0) sText = XI_ConvertString("Speak_SurrNo1");
			else
			{
				if (iNoVar == 1) sText = XI_ConvertString("Speak_SurrNo2");
				else sText = XI_ConvertString("Speak_SurrNo3");
			}
		}
		else
		{
			if (rand(1) == 1) sText = XI_ConvertString("Speak_SurrNoAgain1");
			else sText = XI_ConvertString("Speak_SurrNoAgain2");
		}
		rSpeakCharacter.speak.surrtried = true; // дальше отказы уже "повторные"
		ShowSubMenu(sText, false, "");
	}
}

// враг САМ вышел на связь и предлагает сдать корабль (sMode "surrender" из Speak_EnemyInitiative).
// Подменю да/нет: принять капитуляцию или отказать и добить
void OfferSurrenderIncoming()
{
	ShowSubMenu(XI_ConvertString("Speak_20"), true, "surrender_accept"); // "Не стреляйте! Я сдаюсь!"
}

void AcceptIncomingSurrender()
{
	// сдавшегося капитана нанять в офицеры МОЖНО (запрет только на ДУ/каперов фрахта)
	Ship_Surrender(sti(rSpeakCharacter.index)); // белый флаг + дрейф; грабёж откроется при сближении (AIShip.c)
	IDoExit(RC_INTERFACE_SPEAK_EXIT, true);
}

void AgreeRansom()
{
	int iSumm;
	if (sGlobalMode == "ransom") iSumm = CalculateSummSelf(rSpeakCharacter); // враг САМ предложил - от ЕГО положения (совпадает с показанной в DemandRansom)
	else iSumm = CalculateSumm(rSpeakCharacter);

	rSpeakCharacter.money = sti(rSpeakCharacter.money) - iSumm;
	AddMoneyToCharacter(pchar, iSumm);
	rSpeakCharacter.speak.ransom.taked = 1;
	// контрибуция платится с казны ЭТОГО капитана - нейтралится только его корабль
	// (Ship_Neutral + SpeakTruce); остальная эскадра дерётся дальше, но рядовые теперь
	// могут сами выходить на связь и просить/требовать свой откуп
	SpeakTruce_ApplyShip(rSpeakCharacter);

	SetInfoText(XI_ConvertString("Speak_13")); // "Хорошо, вот деньги. Теперь я могу продолжить путь?!"
	sSubMode = "";
	RebuildMainMenu();
}

// ---------------------------------------------------------------------------
//  ОТКУП: мы платим врагу, чтобы разойтись миром (сумма - CalculateTax в seaspeak.c,
//  по соотношению боевой ценности эскадр)
// ---------------------------------------------------------------------------

// враг сам потребовал дань (входящий вызов): ультиматум с суммой за эскадру в INFO,
// дальше игрок сам выбирает в меню - откупиться кораблём, эскадрой или послать
void TaxDemandIntro()
{
	SetInfoText(XI_ConvertString("Speak_TaxDemand") + " " + CalculateTax(rSpeakCharacter) + GetConvertStrWithReplace("Variable_Speak_2", "Interface.txt", "#space#", " "));
}

void OfferTax()
{
	int iTax = CalculateTax(rSpeakCharacter);
	if (!bTaxAll) iTax = CalculateTaxShip(rSpeakCharacter);
	string sText = XI_ConvertString("Speak_21") + " " + iTax + GetConvertStrWithReplace("Variable_Speak_2", "Interface.txt", "#space#", " ");

	if (sti(pchar.money) >= iTax)
	{
		ShowSubMenu(sText, true, "tax_pay");
	}
	else
	{
		// денег не хватает - и собеседник это знает
		ShowSubMenu(sText + "\n" + XI_ConvertString("Speak_9"), false, "");
	}
}

void OkTax()
{
	int iTax = CalculateTax(rSpeakCharacter);
	if (!bTaxAll) iTax = CalculateTaxShip(rSpeakCharacter);
	AddMoneyToCharacter(pchar, -iTax);
	rSpeakCharacter.speak.tax = 1;

	// перемирие делает ВСЁ: НЕЙТРАЛИТЕТ (флаг SpeakTruce, держится периодикой ИИ), выстрелы
	// прощаются, задача атаки снята; до выхода на глобалку. Эскадра целиком либо один корабль
	if (bTaxAll) SpeakTruce_Apply(rSpeakCharacter);
	else SpeakTruce_ApplyShip(rSpeakCharacter);
	NationUpdate();
	DoQuestCheckDelay("NationUpdate", 1.5);

	SetInfoText(XI_ConvertString("Speak_TaxDone"));
	sSubMode = "";
	RebuildMainMenu();
}

// ---------------------------------------------------------------------------
//  ПОДСВЕТКА СТРОК (событие прокрутки строкового нода)
// ---------------------------------------------------------------------------

void FTChange()
{
	string sTemp = GetEventData();
	float fNewPos = GetEventData();
	int iStep = 0;
	float fStep = 0.0;

	if (sTemp == "ANSWERS")
	{
		fStep = (iAnswersCount - 1) * fNewPos;
		fStep = fStep + 0.5;
		iStep = sti(fStep);
		PaintAnswers("ANSWERS", iAnswersCount, iStep);
		SetCurrentNode("ANSWERS");
	}

	if (sTemp == "RANSOM_ANSWERS")
	{
		fStep = (iSubCount - 1) * fNewPos;
		fStep = fStep + 0.5;
		iStep = sti(fStep);
		PaintAnswers("RANSOM_ANSWERS", iSubCount, iStep);
		SetCurrentNode("RANSOM_ANSWERS");
	}
}

// ---------------------------------------------------------------------------
//  ВЫХОД
// ---------------------------------------------------------------------------

void ProcessCancelExit()
{
	if (iSpeakFlag == SPEAK_RES_CAPTURE) { Ship_Surrender(sti(rSpeakCharacter.index)); }
	IDoExit(RC_INTERFACE_SPEAK_EXIT, true);
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode, true);
}

void IDoExit(int exitCode, bool bCode)
{
	DelEventHandler("InterfaceBreak","ProcessCancelExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");
	DelEventHandler("FTChange","FTChange");
	DelEventHandler("frame","Speak_FrameTick"); // LEO: снять покадровый тик гейта ввода
	iSpeakInBlockMs = 0;

	interfaceResultCommand = exitCode;

	if (interfaceResultCommand != RC_INTERFACE_SPEAK_EXIT_AND_CAPTURE)
	{
		if (iSpeakFlag == SPEAK_RES_NEUTRAL) AnalizeSituations(rSpeakCharacter);
		if (iSpeakFlag == SPEAK_RES_TAX)     AnalizeSituationsTax(rSpeakCharacter);
		if (iSpeakFlag == SPEAK_RES_ENEMY)   AnalizeSituationsEnemy();
	}
	pchar.speakchr = rSpeakCharacter.id;
	EndCancelInterface(bCode);
	SetTimeScale(1.0);
}
