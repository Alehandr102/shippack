
void ProcessDialogEvent()
{
	ref NPChar, sld, chr, sldGirl;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		case "TohmasPetit_SelfTalkStart":
			dialog.text = "Времени прошло достаточно, нужно выдвигаться.";
			link.l1 = "";
			link.l1.go = "TohmasPetit_SelfTalkEnd";
		break;
		
		case "TohmasPetit_SelfTalkEnd":
			DialogExit();
			LAi_SetPlayerType(PChar);
			PChar.dialog.filename = "MainHero_dialog.c";
			PChar.Dialog.CurrentNode = "TalkSelf_Main"; 
		break;
		
		case "TohmasPetitBandit_1":
			dialog.text = "Ха-ха! Наш Томас снова привёл "+GetSexPhrase("очередного дурачка","очередную дурочку")+"! Ну что, капитан, раскошеливайся, если шкура дорога.";
			link.l1 = "Что это значит?";
			link.l1.go = "TohmasPetitBandit_2";
		break;
		
		case "TohmasPetitBandit_2":
			dialog.text = "Томас и его подружка получают свою долю за каждого приведённого дурака. И ты далеко не перв"+GetSexPhrase("ый","ая")+", кто прибежал"+GetSexPhrase("","а")+" на выручку нашему общему знакомому.";
			link.l1 = "Надо же, а с виду приличный человек, но оказался очередным пройдохой. А я повел"+GetSexPhrase("ся как дурак.","ась как дура.");
			link.l1.go = "TohmasPetitBandit_2_1"; 
		break;
		
		case "TohmasPetitBandit_2_1":
			dialog.text = "Ладно, капитан, поболтали и хватит. Теперь плати, 40 000 пиастров будет достаточно, иначе нам придётся забрать деньги с твоего трупа!";
			link.l1 = "Ну что притих, Томас? Твои новые друзья ждут, когда я начну платить. Скажешь что-нибудь в своё оправдание?";
			link.l1.go = "TohmasPetitBandit_ToThomas"; 
		break;

		case "TohmasPetitBandit_ToThomas":
			DialogExit();
			chr = CharacterFromID("TohmasPetit");
			chr.dialog.filename = "Quest\IP\Thomas\all.c"; 
			chr.dialog.currentnode = "TohmasPetitBandit_3"; 
			LAi_SetActorType(chr);
			LAi_ActorDialog(chr, pchar, "", 1.5, 0);
		break;

		case "TohmasPetitBandit_3":
			dialog.text = "Капитан... Вы уж извините, но против четверых у вас шансов нет. Отдайте им кошелёк, не доводите до греха! Жизнь-то дороже пиастров.";
			link.l1 = "Четверо оборванцев и один трусливый наводчик? Невелики у вас шансы. Доставайте сабли, псы, сейчас я пущу вас на корм червям!";
			link.l1.go = "TohmasPetitBandit_4"; 
			if(sti(pchar.money)>=40000){
				link.l2 = "Твоя правда, Томас... Твоя жизнь, может, и гроша не стоит, а моя мне дорога, я заплачу.";
				link.l2.go = "TohmasPetitBandit_pay"; 
			}
		break;
		
		case "TohmasPetitBandit_pay":
			DialogExit();
			chr = CharacterFromID("TohmasPetit_bandit4");
			chr.dialog.filename = "Quest\IP\Thomas\all.c"; 
			chr.dialog.currentnode = "TohmasPetit_PayBanditTalk1"; 
			LAi_SetActorType(chr);
			LAi_ActorDialog(chr, pchar, "", 1.5, 0);
		break;	
		
		case "TohmasPetit_PayBanditTalk1":
			dialog.text = "Это правильно, капитан. Томас, забирай свою девку и вали отсюда, свою долю получите как обычно на старом месте.";
			link.l1 = "Правильно, беги, трус. Но знай, я всё равно тебя из-под земли достану!";
			link.l1.go = "TohmasPetit_PayTomasInit";
		break;
		
		case "TohmasPetit_PayTomasInit":
			DialogExit();
			chr = CharacterFromID("TohmasPetit");
			chr.dialog.filename = "Quest\IP\Thomas\all.c"; 
			chr.dialog.currentnode = "TohmasPetit_PayTomasTalk"; 
			LAi_SetActorType(chr);
			LAi_ActorDialog(chr, pchar, "", 1.5, 0);
		break;
		
		case "TohmasPetit_PayTomasTalk":
			dialog.text = "Капитан, не нужно мне угрожать. В городе меня знают нужные люди, у нас солидная крыша, так что вам ещё придётся хорошенько побегать, чтобы нас найти! Прощайте.";
			link.l1 = "Поживем, увидим.";
			link.l1.go = "TohmasPetit_PayTomasExit";
		break;
		
		case "TohmasPetit_PayTomasExit":
			DialogExit();
			chr = CharacterFromID("TohmasPetit");
			LAi_SetActorType(chr);
			LAi_ActorRunToLocation(chr, "reload", "reload3_back", "none", "", "", "", 2);
			chr = CharacterFromID("TohmasPetit_girl");
			LAi_SetActorType(chr);
			LAi_ActorRunToLocation(chr, "reload", "reload3_back", "none", "", "", "", 2);
			chr = CharacterFromID("TohmasPetit_bandit4");
			chr.dialog.filename = "Quest\IP\Thomas\all.c"; 
			chr.dialog.currentnode = "TohmasPetit_PayBanditStall1"; 
			LAi_SetActorType(chr);
			LAi_ActorDialog(chr, pchar, "", 1.0, 0);
			AddMoneyToCharacter(pchar, -40000); // Борода: в диалоге озвучено 40к, списывалось 30к - рассинхрон
		break;
		
		case "TohmasPetit_PayBanditStall1":
			dialog.text = "Давай сюда мешок! Приятно иметь дело с благоразумными людьми. А ты, капитан, я смотрю, совсем не любишь риск, раз так легко "+GetSexPhrase("расстался","рассталась")+" с 40 000 пиастров?";
			link.l1 = "Я ценю свою жизнь выше пиастров. Забирайте деньги и расходимся.";
			link.l1.go = "TohmasPetit_PayBanditStall2";
		break;
		
		case "TohmasPetit_PayBanditStall2":
			dialog.text = "Ха-ха! Слышали, парни? "+GetSexPhrase("Он","Она")+" ценит жизнь! А Томас-то молодец. Какую схему провернул! Наплёл про похищенную невесту, бандитов в джунглях... и ты, гроза морей, "+GetSexPhrase("повёлся как мальчишка","повелась как соплячка")+"!";
			link.l1 = "Каждый может ошибиться. Хватит зубы скалить.";
			link.l1.go = "TohmasPetit_PayBanditStall3";
		break;

		case "TohmasPetit_PayBanditStall3":
			dialog.text = "Да ладно тебе, не дуйся! Зато сегодня весь бордель и все таверны в городе будут гулять за твой счёт. Мы выпьем за твоё здоровье лучшего рома, честное слово!";
			link.l1 = "Смеётся тот, кто смеётся последним.";
			link.l1.go = "TohmasPetit_PayBanditStall4";
		break;

		case "TohmasPetit_PayBanditStall4":
			dialog.text = "Угрожаешь? Ну-ну. Смотри не лопни от злости, капитан. Деньги у нас, пушки у нас, а ты уходишь отсюда с пустыми карманами. Считай это платой за хороший урок.";
			link.l1 = "Я ухожу. Наш разговор окончен.";
			link.l1.go = "TohmasPetit_PayBanditStall5";
		break;

		case "TohmasPetit_PayBanditStall5":
			dialog.text = "Уходи-уходи, да поживее. Бывай, "+GetSexPhrase("доверчивый","доверчивая")+" капитан!";
			link.l1 = "Чтоб вам ром поперёк горла встал, мрази.";
			link.l1.go = "TohmasPetit_PayFinal";
		break;

		case "TohmasPetit_PayFinal":
			DialogExit();
			AddQuestRecord("TohmasPetit", "TrapWithPay");
			AddQuestUserData("TohmasPetit", "sSex", GetSexPhrase("л","ла")); // LEO: пол ГГ - "угодил/угодила"
			AddDialogExitQuest("TohmasPetit_Pay");
		break;
		
		case "TohmasPetitBandit_4":
			dialog.text = "О Боже! Стража! Стража!!! Убивают!!! Помогите!!!";
			link.l1 = "(достать саблю и атаковать бандитов)";
			link.l1.go = "TohmasPetit_BeforeFightRecord";
		break;

		case "TohmasPetit_BeforeFightRecord":
			DialogExit();
			AddQuestRecord("TohmasPetit", "TrapWithFight");
			AddDialogExitQuest("TohmasPetit_Fight");
		break;	
		
		case "TohmasPetitGirl_1":
			dialog.text	= "На помощь! Убийца! Сюда! Патруль!!! Скорее!!!";
			link.l1 = "Патруль? Раньше нужно было звать на помощь. Теперь ты в безопасности.";
			link.l1.go = "Exit";
		break;

		case "TohmasPetitSoldier_1":
			dialog.text = "Что здесь произошло?! Капитан, немедленно сложите оружие.";
			link.l1 = "Офицер, это была самооборона! Эти бандиты напали на меня!";
			link.l1.go = "TohmasPetitSoldier_2";
		break;
		
		case "TohmasPetitSoldier_2":
			dialog.text = "Четверо трупов и один капитан с окровавленной саблей. Не очень-то похоже на самооборону.";
			link.l1 = "Меня подставили.";
			link.l1.go = "TohmasPetitSoldier_3";
		break;
		
		case "TohmasPetitSoldier_3":	
			dialog.text = "Томас, ты был здесь. Я хочу услышать, что скажешь ты.";
			link.l1 = "...";
			link.l1.go = "TohmasPetitSoldier_CallThomas";
		break;
		
		case "TohmasPetitSoldier_CallThomas":
			DialogExit();
			chr = CharacterFromID("TohmasPetit");
			chr.dialog.filename = "Quest\IP\Thomas\all.c"; 
			chr.dialog.currentnode = "TohmasPetitSoldier_4"; 
			LAi_SetActorType(chr);
			LAi_ActorDialog(chr, pchar, "", 1.5, 0);
		break;

		case "TohmasPetitSoldier_4":
			dialog.text = "Мы хотели прогуляться за городом с моей невестой, я нанял этих людей как охранников, а "+GetSexPhrase("этот монстр выскочил и начал","эта сумасшедшая выскочила и начала")+" рубить их как скот!";
			link.l1 = "Ах ты крыса... Офицер, это всё ложь.";
			link.l1.go = "TohmasPetitThomas_ToSoldier";
		break;
		
		case "TohmasPetitThomas_ToSoldier":
			DialogExit();
			sld = CharacterFromID("TohmasPetit_soldier1");
			sld.dialog.filename = "Quest\IP\Thomas\all.c";
			sld.dialog.currentnode = "TohmasPetitSoldier_5";
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", 1.5, 0);
		break;
		
		case "TohmasPetitSoldier_5":
			dialog.text = "Ложь, говоришь? Давайте узнаем у девушки её версию произошедшего. Девушка, расскажите, что произошло.";
			link.l1 = "...";
			link.l1.go = "TohmasPetitSoldier_ToBride";
		break;

		case "TohmasPetitSoldier_ToBride":
			DialogExit();
			sldGirl = CharacterFromID("TohmasPetit_girl");
			sldGirl.dialog.filename = "Quest\IP\Thomas\all.c";
			sldGirl.dialog.currentnode = "TohmasPetitSoldier_6";
			LAi_SetActorType(sldGirl);
			LAi_ActorDialog(sldGirl, pchar, "", 1.5, 0);
		break;
		
		case "TohmasPetitSoldier_6":
			dialog.text = "Офицер, слава Богу, что вы прибыли так скоро, я думала, что эт"+GetSexPhrase("от монстр","а сумасшедшая")+" ограбит и убьёт меня!";
			link.l1 = "Офицер, всё не так. Меня подставили.";
			link.l1.go = "TohmasPetit_BrideToOfficer";
		break;
		
		case "TohmasPetit_BrideToOfficer":
			DialogExit();
			sld = CharacterFromID("TohmasPetit_soldier1");
			sld.dialog.filename = "Quest\IP\Thomas\all.c";
			sld.dialog.currentnode = "TohmasPetitSoldier_7";
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", 1.5, 0);
		break;

		case "TohmasPetitSoldier_7":
			PlaySound("Kopcapkz\Voices\Soldiers\Soldier_arrest_03.ogg");
			dialog.text = "Молчать! Мы нашли тебя с окровавленным оружием в окружении нескольких трупов, которые раньше были гражданами Бас-Тера. Этот факт, а также свидетельства двух горожан против слова капитана-бродяги перевешивают всё. Сдавай оружие и следуй за нами, пока я не приказал расстрелять тебя на месте.";
			link.l1 = "Попробуйте меня взять, если сможете.";
			link.l1.go = "TohmasPetitSoldier_fight";
			link.l2 = "Хорошо, сержант. Я подчиняюсь. Идемте.";
			link.l2.go = "TohmasPetitSoldier_prison";
		break;
		
		case "TohmasPetitSoldier_fight":
			DialogExit();
			AddDialogExitQuest("TohmasPetit_soldiers_fight");
		break;

		case "TohmasPetitSoldier_prison": 
			ref rOfficer = CharacterFromID("TohmasPetit_soldier1");
			aref arItems, officerItems;
			ref rItem;
			string sName;
			int i, iItemsNum, iSavedMoney;
			int iPotion1 = 0;
			int iPotion2 = 0;
			int iPotion3 = 0;
			int iPotion4 = 0;
			
			RemoveCharacterEquip(pchar, BLADE_ITEM_TYPE);
			RemoveCharacterEquip(pchar, GUN_ITEM_TYPE);
			RemoveCharacterEquip(pchar, SUIT_ITEM_TYPE);
			RemoveCharacterEquip(pchar, SPYGLASS_ITEM_TYPE);
			iSavedMoney = sti(pchar.money);
			
			iPotion1 = GetCharacterItem(pchar, "potion1"); 
			iPotion2 = GetCharacterItem(pchar, "potion2");
			iPotion3 = GetCharacterItem(pchar, "potion3"); 
			iPotion4 = GetCharacterItem(pchar, "potion4"); 

			makearef(arItems, pchar.items);
			makearef(officerItems, rOfficer.items);
			iItemsNum = GetAttributesNum(arItems);
			for(i = 0; i < iItemsNum; i++)
			{
				sName = GetAttributeName(GetAttributeN(arItems, i));

				if (sName == "unarmed") continue;
				// зелья сохраняются отдельно (остаются у ГГ), поэтому в хранилище их не кладём, иначе задвоятся
				if (sName == "potion1" || sName == "potion2" || sName == "potion3" || sName == "potion4") continue;

				// квестовые предметы (тотемы, порошок мумии, атлас и т.д.) RemoveAllCharacterItems НЕ удаляет,
				// поэтому их нельзя класть в хранилище/копировать офицеру — иначе они задвоятся и останутся у ГГ
				rItem = ItemsFromID(sName);
				if (CheckAttribute(rItem, "ItemType") && rItem.ItemType == "QUESTITEMS") continue;

				// сохраняем ВСЁ остальное имущество ГГ (оружие, броня, карты, пули, провизия, драгоценности и т.д.),
				// чтобы при освобождении/обыске трупа вернуть всё, а не только оружие и броню
				pchar.QuestTemp.ArrestStorage.(sName) = pchar.items.(sName);
			}
			// Борода: только ЗАПОМИНАЕМ надетый патент - по этой метке офицер отдельно давит на потерю
			// званий, а при выкупе документ вернётся. Сам предмет изымает TohmasPetitSoldier_go_prison:
			// здесь, внутри диалога и сразу после RemoveAllCharacterItems, TakeItemFromCharacter молча
			// не срабатывал (в трассировке патент оставался в инвентаре).
			if (isMainCharacterPatented()) pchar.QuestTemp.ArrestPatent = Items[sti(pchar.EquipedPatentId)].id;
			RemoveAllCharacterItems(pchar, true);
			pchar.money = iSavedMoney;
			if (iPotion1 > 0) TakeNItems(pchar, "potion1", iPotion1);
			if (iPotion2 > 0) TakeNItems(pchar, "potion2", iPotion2);
			if (iPotion3 > 0) TakeNItems(pchar, "potion3", iPotion3);
			if (iPotion4 > 0) TakeNItems(pchar, "potion4", iPotion4);
			if (CheckAttribute(pchar, "QuestTemp.ArrestStorage"))
			{
				makearef(arItems, pchar.QuestTemp.ArrestStorage);
				makearef(officerItems, rOfficer.items);
				iItemsNum = GetAttributesNum(arItems);

				for(i = 0; i < iItemsNum; i++)
				{
					sName = GetAttributeName(GetAttributeN(arItems, i));
					officerItems.(sName) = pchar.QuestTemp.ArrestStorage.(sName);
				}
				// ArrestStorage намеренно НЕ удаляется: он хранит точный список вещей ГГ
				// для мирного возврата (выкуп), чтобы не отдать игроку экипировку самого офицера.
				// Копия в инвентаре офицера остаётся для лута с трупа при силовом решении.
			}
			if (CheckAttribute(rOfficer, "items")) 
			{
				SetCharacterGoods(rOfficer, GOOD_BALLS, GetCargoGoods(rOfficer, GOOD_BALLS)); 
			}
			LAi_SetFightMode(pchar, false);
			DialogExit();
			AddDialogExitQuest("TohmasPetitSoldier_go_prison");
		break;

		case "TohmasPetitOfficer_1":
			dialog.text = "Капитан, ты сейчас в таком щекотливом положении, что я на твоем месте начал бы исповедоваться еще до прихода священника.";
			link.l1 = "Что, все так плохо?";
			link.l1.go = "TohmasPetitOfficer_2";
		break;

		case "TohmasPetitOfficer_2":
			// Борода: если у ГГ изъяли каперский патент - офицер отдельно давит на потерю званий
			if (CheckAttribute(pchar, "QuestTemp.ArrestPatent"))
			{
				dialog.text = "На тебе, "+GetSexPhrase("парень","девка")+", висит убийство четырех человек и попытка ограбления двух горожан. Да ещё мы изъяли твой каперский патент. Знай: не станешь с нами сотрудничать - лишишься всех своих званий и привилегий. Посмертно, разумеется. Но я могу 'замять' это дело, и патент вернётся к тебе целым, будто ничего и не было. Только это будет стоить немало.";
				link.l1 = "И во сколько мне обойдётся моё доброе имя?";
			}
			else
			{
				dialog.text = "На тебе, "+GetSexPhrase("парень","девка")+", висит убийство четырех человек и попытка ограбления двух горожан, но я могу 'замять' это дело. Но это будет стоить немало.";
				link.l1 = "О какой сумме идёт речь?";
			}
			link.l1.go = "TohmasPetitOfficer_3";
		break;
		
		case "TohmasPetitOfficer_3":
			dialog.text = "Ну, скажем, сто пятьдесят тысяч пиастров.";
			link.l1 = "Ты в своём уме? Это целая куча денег.";
			link.l1.go = "TohmasPetitOfficer_4";
		break;
		
		case "TohmasPetitOfficer_4":
			dialog.text = "Да, много, но ты и меня пойми, мне придётся многим заплатить, а также учесть свой интерес и долю моего друга.";
			if(sti(pchar.money)>=150000)
			{
				link.l1 = "Похоже, выбора у меня нет. Я готов"+GetSexPhrase("","а")+" заплатить.";
				link.l1.go = "TohmasPetitOfficer_money";
			}
			link.l2 = "У меня нет таких денег.";
			link.l2.go = "TohmasPetitOfficer_executon";
			
		break;
		
		case "TohmasPetitOfficer_money":
			dialog.text = "Это хорошо. До завтра придется тебе еще посидеть, но к утру будь уверен, тебя выпустят, и вернут все твои пожитки.";
			link.l1 = "Делать нечего, придется ждать.";
			link.l1.go = "TohmasPetitOfficer_money2";
		break;
		
		case "TohmasPetitOfficer_money2":
			DialogExit();
			AddTimeToCurrent(24, 00);
			SetCurrentTime(8, 20);
			AddQuestRecord("TohmasPetit", "TohmasPetit_PrisonBribePaid");
			AddDialogExitQuest("TohmasPetitSoldier_free");
		break;
		
		case "TohmasPetitOfficer_executon":
			dialog.text = "Ну, как знаешь. Завтра утром придёт священник, а к полудню тебя казнят. А мы в любом случае получим свои деньги, продав конфискованное у тебя имущество.";
			link.l1 = "Надеюсь вы подавитесь деньгами.";
			link.l1.go = "TohmasPetit_PrisonDecision_No";
		break;
		
        case "TohmasPetit_PrisonDecision_No":
			DialogExit();
			SetCurrentTime(22, 20);
            SetLaunchFrameFormParam("Офицер отошёл от вашей камеры. Вы остались в раздумьях, ожидая рассвета и петли, и не заметили, как уснули. Прошло несколько часов, и вы сквозь тяжёлый сон слышите, как лязгает тюремный засов...", "Reload_To_Location", 0.1, 2.0);
			SetLaunchFrameReloadLocationParam("BasTer_prison", "goto","goto24", "TohmasPetit_CellmateWakeUp");
			playSound("Ambient\JAIL\jail_door2.wav");
			LaunchFrameForm();
        break;
		
		case "Prison_Escaper_Start":
			dialog.text = GetSexPhrase("Эй, приятель... Вид у тебя неважный. За что загремел?","Ого, вот это мне повезло с камерой. Эй, красотка, за что тебя взяли?");
			link.l1 = "За убийство.";
			link.l1.go = "Prison_Escaper_1";
		break;

		case "Prison_Escaper_1":
			dialog.text = "За убийство говоришь... А кого хоть убил"+GetSexPhrase("?","а?");
			link.l1 = "Четверых бандитов, которые оказались не бандитами, а местными жителями. И во всём виноват этот пройдоха Томас";
			link.l1.go = "Prison_Escaper_2";
		break;
		
		case "Prison_Escaper_2":
			dialog.text = "Томас? Ты случаем не про Томаса Петита говоришь?";
			link.l1 = "Да, он самый. А ты откуда его знаешь? Уж не он ли подослал тебя, чтобы пораньше меня прикончить?";
			link.l1.go = "Prison_Escaper_3";
		break;
		
		case "Prison_Escaper_3":
			dialog.text = "Нет, конечно, из меня никудышный убийца, я больше по взломам и кражам. А что до Томаса, так многие его знают в определённых кругах, его и дружка офицера городской стражи, они часто проворачивают сомнительные авантюры вместе.";
			link.l1 = "Вот почему патруль подоспел так быстро, это многое объясняет.";
			link.l1.go = "Prison_Escaper_4";
		break;
		
		case "Prison_Escaper_4":
			dialog.text = "Да ты не перв"+GetSexPhrase("ый","ая")+", кто повёл"+GetSexPhrase("ся","ась")+" на этот развод. Схема простая: они нанимают всякий сброд - бывших моряков или бандитов, тех, кто хоть как-то умеют держать оружие в руках, но топят свою жизнь на дне кружки. Дело-то рисковое, но  таким людям особо и терять нечего.";
			link.l1 = "Понятно, приятно было поболтать, жаль что эти сведения мне не пригодятся, завтра у меня назначена встреча с петлей.";
			link.l1.go = "Prison_Escaper_5";
		break;

		case "Prison_Escaper_5":
			dialog.text = "Нам обоим крупно повезло. Я придумал план побега, замок - моя забота. Но там, в коридоре, сидит офицер. В одиночку я его не пройду, а вот с тобой шанс есть. Возьми заточку, её мой приятель припрятал под нарами. Твоя задача снять стражника без шума. За ним будет проход к старой стене. Мешкать нельзя, патрули ходят каждые полчаса. Открываю?";
			link.l1 = "Хм, так у меня есть хоть какой-то шанс выжить, а возможно ещё и вернуть свои пожитки. Давай свою заточку и открывай камеру.";
			link.l1.go = "Prison_Escaper_6";
		break;
		
		case "Prison_Escaper_6":
			dialog.text = "Хорошо, погоди чуток - сейчас найду где он её запрятал. А, вот она, держи, сейчас я открою камеру.";
			link.l1 = "Пожелай мне удачи.";
			link.l1.go = "Prison_Escaper_7";
		break;

		case "Prison_Escaper_7":
			DialogExit();
			AddDialogExitQuest("TohmasPetit_Start_Fight");
			AddQuestRecord("TohmasPetit", "TohmasPetit_PrisonEscapeStart");
		break;

		case "Prison_Escaper_8":
			dialog.text = "Фух, вырвались! Стража форта теперь на ушах, назад нам ходу нет. Сейчас нам нужно убраться отсюда, пока нас не схватили.";
			link.l1 = "В путь!";
			link.l1.go = "exit";
			AddDialogExitQuest("TohmasPetit_follow");
		break;

		case "Prison_Escaper_10":
			dialog.text = "Кажется, погони нет. Слушай, кэп, без звонкой монеты мне с острова не уйти, нужно перекупить лодку у местных рыбаков. Есть у меня одна вещица... Клочок карты сокровищ. Напрямик скажу, карта порвана, у меня только часть. Я потратил кучу времени и, как мне кажется, разгадал, где это место! Возможно и ты сможешь разобраться, или удача улыбнётся тебе, и ты найдёшь недостающую часть. Я готов продать её тебе за сорок тысяч пиастров. Выручишь меня, а сам сможешь разбогатеть.";
			bool bHasTreasure = false;
			if (GetCharacterItem(pchar, "map_full") > 0)
			{ 
				bHasTreasure = true;
			}
			if (GetCharacterItem(pchar, "map_part1") > 0 && GetCharacterItem(pchar, "map_part2") > 0) 
			{
				bHasTreasure = true;
			}
			if (sti(pchar.money) >= 40000 && bHasTreasure == false) 
			{
				link.l1 = "Идёт. 40 000 пиастров - честная цена за такую наводку. Забирай золото и удачи с рыбаками.";
				link.l1.go = "Prison_Escape_BuyMap";
			}
			if (sti(pchar.Ship.Type) != SHIP_NOTUSED)
			{
				link.l2 = "Какие сокровища, Жак? Форт на ушах, мне бы свой корабль из этой заварухи вытащить. Бывай.";
				link.l3 = "Постой. Раз ты уже разгадал место, зачем мне покупать кота в мешке? У меня есть отличный корабль, у тебя - координаты. Давай объединим усилия. Я везу нас к тайнику, берём клад и делим пополам. И ты, и я в обиде не останемся, надеюсь.";
				link.l3.go = "Prison_Escape_Klad";
				pchar.questTemp.Tohmas_ShipName = pchar.Ship.name;
			}
			else
			{
				link.l2 = "Какие сокровища, Жак? Форт на ушах, мне бы выбраться из этой заварухи. Бывай.";
			}
			link.l2.go = "Prison_Escape_Farewell";
		break;

		case "Prison_Escape_BuyMap":
			AddMoneyToCharacter(pchar, -40000);
			if (GetCharacterItem(pchar, "map_full") == 0)
			{
			if (!GetCharacterItem(pchar, "map_part1")) 	GiveItem2Character(pchar, "map_part1");
			else 										GiveItem2Character(pchar, "map_part2");
			}
			dialog.text = "Удачи, капитан! Надеюсь, этот клочок принесет тебе больше пользы, чем мне.";
			link.l1 = "Прощай.";
			link.l1.go = "Prison_Escape_End";
		break;
		
		case "Prison_Escape_End":
			DialogExit();
            DisableAllExits (false);
			bDisableOfficers = false; // купили карту — побег окончен, офицеры снова при ГГ
			LAi_SetActorType(npchar);
			LAi_ActorRunToLocation(npchar, "reload", "reload1", "none", "", "", "", -1);
			AddQuestRecord("TohmasPetit", "TohmasPetit_JacquesBoughtMap");
			AddQuestUserData("TohmasPetit", "sSex", GetSexPhrase("л","ла")); // LEO: пол ГГ - "отдал/отдала"
			CloseQuestHeader("TohmasPetit");
			AddGoodDeed("TohmasPetit", 3); // Gregg: доброе дело - развязка без резни
			pchar.questdata.TohmasPetit = 1; 
			TohmasPetit_ClearQuestTemp(); // Борода: квест закрыт - снимаем временные атрибуты
			DeleteAttribute(pchar, "DisableChangeFlagMode"); // снимаем запрет смены флага (проверка идёт по существованию атрибута, не по значению)
		break;

		case "Prison_Escape_Farewell":
			dialog.text = "Что ж, воля твоя. Бывай, не попадайся больше страже!";
			link.l1 = "Прощай.";
			link.l1.go = "Prison_Escape_End3";
		break;
		
		case "Prison_Escape_End3":
			DialogExit();
            DisableAllExits (false);
			bDisableOfficers = false; // отказались от карты — побег окончен, офицеры снова при ГГ
			LAi_SetActorType(npchar);
			LAi_ActorRunToLocation(npchar, "reload", "reload1", "none", "", "", "", -1);
			AddQuestRecord("TohmasPetit", "TohmasPetit_JacquesRefused");
			CloseQuestHeader("TohmasPetit");
			pchar.questdata.TohmasPetit = 1;
			TohmasPetit_ClearQuestTemp(); // Борода: квест закрыт - снимаем временные атрибуты
			DeleteAttribute(pchar, "DisableChangeFlagMode"); // снимаем запрет смены флага (проверка идёт по существованию атрибута, не по значению)
		break;
		
		case "Prison_Escape_Klad":
			bool ok = false;
			if (CheckAttribute(Pchar, "location.from_sea") && Pchar.location.from_sea == "BasTer_town")
			{
				ok = true;
			}
			dialog.text = "Хм... Твоя правда. С тобой у нас есть реальный шанс. Где пришвартован твой корабль?";
			if (ok == true) 
			{
				link.l1 = "На пирсе в городе, корабль " + pchar.questTemp.Tohmas_ShipName + ", укомплектован и готов к отплытию.";
				link.l1.go = "Prison_Escape_ShipInPort";
			}
			else
			{
				link.l1 = "Мой корабль " + pchar.questTemp.Tohmas_ShipName + " стоит на якоре вне городского порта, в бухте " + XI_ConvertString(Pchar.location.from_sea) + ".";
				link.l1.go = "Prison_Escape_ShipSafe";

			}
			DeleteAttribute(pchar, "DisableChangeFlagMode"); // снимаем запрет смены флага (проверка идёт по существованию атрибута, не по значению)
		break;
		
		case "Prison_Escape_ShipInPort":
			dialog.text = "Так не пойдёт! Даже если мы минуем стражу в городе и прорвёмся на пирс, форт сделает из твоей посудины решето. У меня есть идея. Жди здесь, я передам маляву своим ребятам в городе. Они найдут способ, как связаться с твоей командой, чтобы те по-тихому отогнали корабль в бухту Морн л'О, место там тихое. А мы доберёмся туда пешком через джунгли.";
			link.l1 = "Умный ход. Давай я напишу им записку, чтобы команда поверила твоим людям. Старпом должен узнать мой почерк.";
			link.l1.go = "Prison_Escape_ShipInPort1";
		break;

		case "Prison_Escape_ShipInPort1":
			DialogExit();
			AddTimeToCurrent(0, 15);
			Log_Info("Вы написали и передали записку");
			PlaySound("interface\important_item.wav");
			SetLaunchFrameFormParam("Жак убежал в сторону города. Прошло 15 минут, ты слышишь чьи-то шаги...", "Reload_To_Location", 0.1, 2.0);
			SetLaunchFrameReloadLocationParam(pchar.location, "reload", "reload3_back", "TohmasPetit_AtGatesScene2");
			LaunchFrameForm();
		break;

		case "Prison_Escape_ShipInPort2":
			dialog.text = "Дело сделано, записку передали. Твоя команда должна выдвигаться. Нам нельзя здесь торчать, пошли в бухту Морн л'О пешком через джунгли.";
			link.l1 = "Вперед, не теряем времени.";
			link.l1.go = "exit";
			AddQuestRecord("TohmasPetit", "TohmasPetit_JacquesShipInPort");
			AddDialogExitQuest("TohmasPetit_MoveShipToMonrlo_Start");
			DisableAllExits (false);
		break;

		case "Prison_Escape_SailorReport":
			ref rSailor = CharacterFromID("TohmasQuest_Sailor");
			dialog.text = "Капитан! "+GetSexPhrase("Живой","Живая")+"! Ну и задали вы нам задачку. Приходят на борт какие-то мутные типы, суют записку... Хорошо, я ваш почерк сразу узнал. Мы без шума снялись с якоря и ушли сюда, форт даже не почесался.";
			link.l1 = "Отличная работа. Жак, корабль на месте, шлюпка ждет. Куда теперь держим курс?";
			link.l1.go = "exit";
			AddDialogExitQuest("Prison_Escape_SailorReport_2");
		break;

		case "Prison_Escape_ShipInPort_Final":
			dialog.text = "Отличные у тебя ребята, кэп! Раз шлюпка у берега, прыгаем на борт. Приказывай команде ставить паруса на " + XI_ConvertString(pchar.questTemp.TohmasPetit_landId + "Acc") + " - именно там зарыто наше золото!";
			link.l1 = "Отлично, Жак. Занимай каюту, мы отплываем.";
			link.l1.go = "exit";
			AddDialogExitQuest("TohmasPetit_ToGrot");
			string sTargetIsland1 = XI_ConvertString(pchar.questTemp.TohmasPetit_landId);
			AddQuestRecord("TohmasPetit", "TohmasPetit_SailFromPortToBay");
			AddQuestUserData("TohmasPetit", "sTargetIsland", sTargetIsland1);
		break;

		case "Prison_Escape_ShipSafe":
			dialog.text = "Это сильно облегчает задачу! Раз форт нас не зажмет, пробираемся через джунгли сразу к твоему кораблю в бухту.";
			link.l1 = "Тогда вперед, не теряем времени.";
			link.l1.go = "exit";
			string sBayName; 
			
			if (CheckAttribute(Pchar, "location.from_sea"))
			{
				string sBayID = Pchar.location.from_sea; 
				sBayName = XI_ConvertString(sBayID);   
			}
			AddDialogExitQuest("TohmasPetit_WalkToSafeBay_Start");
			AddQuestRecord("TohmasPetit", "TohmasPetit_JacquesShipSafe");
			AddQuestUserData("TohmasPetit", "sBayName", sBayName);
			DisableAllExits (false);
		break;

		case "Prison_Escape_ShipSafe_Final":
			dialog.text = "Ну вот мы и добрались до твоей лоханки невредимыми! Отличная посудина. Давай грузиться на борт и скорее поднимать паруса. Наш курс на " + XI_ConvertString(pchar.questTemp.TohmasPetit_landId + "Acc") + ", сокровища заждались! ";
			link.l1 = "Отлично, Жак. Занимай каюту, мы отплываем.";
			link.l1.go = "exit";
			AddDialogExitQuest("TohmasPetit_ToGrot");
			sTargetIsland1 = XI_ConvertString(pchar.questTemp.TohmasPetit_landId);
			AddQuestRecord("TohmasPetit", "TohmasPetit_SailFromSafeBay");
			AddQuestUserData("TohmasPetit", "sTargetIsland", sTargetIsland1);
		break;
		
		case "Prison_Escaper_InGrot":
			float fFortuneSkill = makefloat(GetCharacterSKILL(pchar, SKILL_FORTUNE)); 
			float fMultiplier = 1.0;

			if (fFortuneSkill <= 40.0) {
				fMultiplier = 0.8 + (fFortuneSkill / 40.0) * 0.2;
			} else {
				fMultiplier = 1.0 + ((fFortuneSkill - 40.0) / 60.0) * 1.0;
			}

			int iLuckChar = sti(pchar.SPECIAL.Luck);
			int iCount;
			// --- ТИР 1 ---
			iCount = makeint(8.0 * fMultiplier); 
			TakeNItems(pchar, "icollection", iCount/2);
			TakeNItems(pchar, "chest", iCount);  

			// --- ТИР 2 ---
			if (iLuckChar >= 4)
			{
				iCount = makeint(50.0 * fMultiplier); 
				TakeNItems(pchar, "jewelry2", iCount); 
				TakeNItems(pchar, "jewelry4", iCount); 
			}

			// --- ТИР 3 ---
			if (iLuckChar >= 8)
			{
				TakeNItems(pchar, "Chest_quest", 1);
				TakeNItems(pchar, "Chest_treasure", 3);
			}		
			Log_Info("Вы получили свою долю клада");
			PlaySound("interface\important_item.wav");
            DeleteAttribute(pchar, "questTemp.TohmasPetit");
			dialog.text = "Вот он, клад! Да тут целая гора сокровищ. Ну что, капитан, уговор дороже денег. Вот твоя половина, забирай.";
			link.l1 = "Отличный куш, Жак. Что планируешь делать дальше?";
			link.l1.go = "Prison_Escaper_InGrot_2";
		break;

		case "Prison_Escaper_InGrot_2":
			dialog.text = "С такой добычей можно завязать с пиратством и отплыть в Старый Свет. Подбрось меня до любого цивилизованного порта, кроме Бас-Тера. Там меня ищейки губернатора сразу в кандалы закроют.";
			link.l1 = "Хорошо.";
			link.l1.go = "exit";
			AddDialogExitQuest("TohmasPetit_EscaperPassenger_Start");
		break;
		
		case "Prison_Escaper_Goodbye":
			dialog.text = "Да, этот порт подойдёт. Спасибо за всё, капитан! Если бы не ты, гнить бы мне на нарах в тюрьме или того хуже. Счастливо оставаться, и пусть море всегда будет к тебе благосклонно!";
			link.l1 = "Прощай, Жак. Не спусти всё золото в первой же таверне. Попутного ветра.";
			link.l1.go = "Prison_Escaper_ubigaet";
			// сначала значение, потом податрибут — иначе перезапись родителя стирает .Path
			pchar.QuestTemp.TohmasPetit_Search = "active";
			pchar.QuestTemp.TohmasPetit_Search.Path = "TooLate";
		break;
		
		case "Prison_Escaper_ubigaet":
			DialogExit();
			AddQuestRecord("TohmasPetit", "TohmasPetit_JacquesLeftSearchActive");
			AddCharacterExpToSkill(PChar, "Fortune", 50);
			AddCharacterExpToSkill(PChar, "Sneak", 50);
			AddDialogExitQuest("TohmasPetit_Escaper_FullEnd");
		break;
		
		// Диалог в трюме корабля ГГ
		case "TohmasPetit_Hold_Dialog":
			if (!CheckAttribute(NPChar, "IsTomas"))
			{
				if (!CheckAttribute(NPChar, "TohmasPetit_HoldTalked"))
				{
					dialog.text = "Пожалуйста, не подходите ко мне... Мне страшно. Я ничего вам не скажу! Разговаривайте с Томасом, это всё его затея...";
					link.l1 = "Ладно, не дрожи. Я поговорю с твоим ненаглядным.";
				}
				else
				{
					dialog.text = "Умоляю, не трогайте меня... Все вопросы к Томасу. Оставьте меня в покое!";
					link.l1 = "Хорошо, сиди смирно.";
				}
				link.l1.go = "TohmasPetit_Girl_Hold_Exit";
				break;
			}
			dialog.text = "Капитан, выслушайте меня! Я знаю, что поступил паршиво, сбежав с Гваделупы, но у нас просто не было другого выхода! Мы не хотели причинить кому-либо вред.";
			link.l1 = "Вы совершили глупость, украв чужое золото и впутав в это кучу людей. Теперь пришло время отвечать.";
			link.l1.go = "TohmasPetit_Hold_To_Girl_1";
		break;

		case "TohmasPetit_Hold_To_Girl_1":
			DialogExit();
			AddDialogExitQuest("TohmasPetit_Hold_Girl_Speak_1");
		break;

		case "TohmasPetit_Girl_Hold_Dialog_1":
			dialog.text = "Мы никого не хотели впутывать! Томас просто искал способ увезти меня отсюда. Мы мечтали купить билеты на корабль до Европы и начать новую жизнь, подальше от этого душного острова!";
			link.l1 = "Красивая романтическая сказка. Жаль, что построена она на воровстве.";
			link.l1.go = "TohmasPetit_Girl_To_Tomas_1";
		break;

		case "TohmasPetit_Girl_To_Tomas_1":
			DialogExit();
			AddDialogExitQuest("TohmasPetit_Hold_Tomas_Speak_1");
		break;

		case "TohmasPetit_Hold_Dialog_3":
			dialog.text = "Капитан, проявите благоразумие! Какая вам выгода везти нас обратно или гноить в этих сырых цепях? У меня всё ещё при себе осталась часть золота, которую я не успел потратить на билеты. Я отдам вам ВСЁ до последнего пиастра, если вы отпустите нас!";
			link.l1 = "Ого, вор пытается выкупить свою свободу краденым золотом? Заманчиво, но я ещё не решил"+GetSexPhrase("","а")+".";
			link.l1.go = "TohmasPetit_Hold_To_Girl_2";
		break;

		case "TohmasPetit_Hold_To_Girl_2":
			DialogExit();
			AddDialogExitQuest("TohmasPetit_Hold_Girl_Speak_2");
		break;

		case "TohmasPetit_Girl_Hold_Dialog_2":
			dialog.text = "Умоляю вас, забирайте это проклятое золото! Оно не принесло нам ничего, кроме бед. Пожалуйста, проявите милосердие, не губите нас, дайте нам шанс уйти...";
			link.l1 = "Довольно слёз и причитаний.";
			link.l1.go = "TohmasPetit_Girl_To_Tomas_2";
		break;

		case "TohmasPetit_Girl_To_Tomas_2":
			DialogExit();
			AddDialogExitQuest("TohmasPetit_Hold_Tomas_Speak_2");
		break;

		case "TohmasPetit_Hold_Dialog_5":
			dialog.text = "Какова наша судьба, капитан? Что вы решите?";
			if (CheckAttribute(pchar, "equip.gun") && pchar.equip.gun != "")
			{
				link.l1 = "Вы оба слишком долго испытывали моё терпение. Вы заслужили смерть. Прямо здесь и сейчас я вас и пристрелю.";
			}
			else
			{
				link.l1 = "Вы заслужили смерть. Я пущу тебе пулю в лоб из твоего же пистолета, Томас, который мои матросы отобрали при обыске.";
			}
				link.l1.go = "TohmasPetit_Hold_Choice_Kill";
				link.l2 = "Твоё золото действительно может компенсировать мои издержки. Забирай свою девку, я высажу вас в ближайшем цивилизованном порту, и чтобы я вас больше не видел.";
				link.l2.go = "TohmasPetit_Hold_Choice_Release";
			if (!CheckAttribute(pchar, "QuestTemp.TohmasPetit_bloodytake"))
			{
				link.l3 = "Никаких сделок с ворами. Я доставлю вас на Бас-Тер и сдам местным властям. Пусть они решают, что с вами делать.";
				link.l3.go = "TohmasPetit_Hold_Choice_Gov";
			}
		break;

		case "TohmasPetit_Hold_Choice_Kill":
			DialogExit();
			AddDialogExitQuest("TohmasPetit_Kill_Start");
		break;

		case "TohmasPetit_Girl_Shock_Dialog":
			dialog.text = "Томас?! О Боже, нет, нет, нет! Томас! Вы убили его!.. Чудовище! Кровожадное чудовище! Умоляю, не надо, пощадите-е-е!..";
			link.l1 = "Меньше надо было воровать. Слишком поздно для милосердия, красотка.";
			link.l1.go = "TohmasPetit_Girl_Shock_End";
		break;

		case "TohmasPetit_Girl_Shock_End":
			DialogExit();
			AddDialogExitQuest("TohmasPetit_Kill_Girl_Final");
		break;

		case "TohmasPetit_Hold_Choice_Release":
			DialogExit();
			
			sld = CharacterFromID("TohmasPetit");
			sld.TohmasPetitQuestHold = true;
			RemovePassenger(pchar, sld);
			AddPassenger(pchar, sld, false);
			ChangeCharacterAddressGroup(sld, "none", "", "");
			
			sld = CharacterFromID("TohmasPetit_girl");
			sld.TohmasPetitQuestHold = true;
			RemovePassenger(pchar, sld);
			AddPassenger(pchar, sld, false);
			ChangeCharacterAddressGroup(sld, "none", "", "");

			AddDialogExitQuest("TohmasPetit_SetupRelease");
			AddQuestRecord("TohmasPetit", "TohmasPetit_Released");
			AddQuestUserData("TohmasPetit", "sSex", GetSexPhrase("ся","ась")); // LEO: пол ГГ - "согласился/согласилась"
		break;

		case "TohmasPetit_Hold_Choice_Gov":
			DialogExit();
			AddQuestRecord("TohmasPetit", "TohmasPetit_HoldToGov");
			AddQuestUserData("TohmasPetit", "sSex", GetSexPhrase("ся","ась")); // LEO: пол ГГ - "отказался/отказалась"
			pchar.QuestTemp.TohmasPetit_Quest = "return_to_baster"; // Стадия: Вернуться на Бас-Тер
		break;

		case "TohmasPetit_Girl_Hold_Exit":
			DialogExit();
			NPChar.TohmasPetit_HoldTalked = true;
		break;
		
		case "TohmasPetit_Cap_Abordage":
			// Проверяем, говорили ли мы с ним до этого на палубе
			if (CheckAttribute(pchar, "QuestTemp.TohmasPetit_CapTalked") && pchar.QuestTemp.TohmasPetit_CapTalked == "failed")
			{
				dialog.text = "Вы?! Значит, вы всё-таки решились на это безумие и атаковали мой шлюп?! Проливать кровь моих матросов ради одного несчастного беглеца... У вас нет ни чести, ни совести!";
				link.l1 = "Я предупреждал"+GetSexPhrase("","а")+", капитан, что заберу Томаса в любом случае. Вы сами выбрали этот путь, отказавшись договориться по-хорошему. А теперь мне придётся избавиться от свидетелей.";
			}
			else
			{
				dialog.text = "Какого дьявола?! Кто вы такой и по какому праву вы атаковали моё судно?!";
				link.l1 = "Мне нужен попутчик, которого вы укрываете на борту, Томас Петит. А с вами, капитан, разговор будет недолгим. Лишние свидетели мне ни к чему.";
			}
			link.l1.go = "TohmasPetit_Cap_Abordage_Kill";
		break;

		case "TohmasPetit_Cap_AbordageFOrWhat":
			dialog.text = "Позвольте, но это же форменное безумие! Я ведь выполнил ваше требование - отдал вам и Томаса, и его невесту, как вы и просили! Мы разошлись с миром, к чему теперь эта бойня?! Что вам ещё нужно от меня?";
			link.l1 = "Всё верно, капитан, претензий к вам больше нет. Но вы и ваша команда слишком много видели и слишком много знаете. А я не привык оставлять за спиной лишних свидетелей.";
			link.l1.go = "TohmasPetit_Cap_Abordage_Kill2";
		break;

		case "TohmasPetit_Cap_Abordage_Kill":
			DialogExit();
			// Борода: QuestAboardCabinDialogExitWithBattle сам ставит LAi_SetFightMode -
			// отдельный MainHeroFightModeOn дублировал его с задержкой (гонка).
			QuestAboardCabinDialogExitWithBattle("TohmasPetit_Vkaute");
		break;
		case "TohmasPetit_Cap_Abordage_Kill2":
			DialogExit();
			// Борода: Томас уже получен мирно - без колбэка, просто дерёмся с командой шлюпа
			QuestAboardCabinDialogExitWithBattle("");
		break;

		case "TohmasPetit_Sailor_Report":
			dialog.text = "Капитан, бой на палубах утих, судно под нашим контролем! Мы досконально обыскали трюмы шлюпа и нашли этого проныру Томаса вместе с его девицей Мадлен, как вы и приказывали. Притащить их к вам?";
			link.l1 = "Отличная работа. Тащите их сюда, в каюту, а после возвращайся к остальной абордажной команде и готовься к отплытию.";
			link.l1.go = "TohmasPetit_Sailor_Leave";
		break;

		case "TohmasPetit_Sailor_Leave":
			DialogExit();
			ChangeCharacterAddressGroup(CharacterFromID("TohmasPetit_sailor"), "none", "", ""); 
			AddDialogExitQuest("TohmasPetit_Vkaute2"); 
		break;
		
		case "TohmasPetit_Cabin_Thomas_Kill":
			dialog.text = "Капитан, умоляю, пощадите нас! Мы отдадим вам всё золото, до последнего! Забирайте всё, только сохраните нам жизнь!";
			link.l1 = "Вы стоили мне целого корабля и кучи убитых матросов. Слишком поздно молить о пощаде, Томас. А оставлять в живых свидетелей этой резни я не намерен"+GetSexPhrase("","а")+".";
			link.l1.go = "TohmasPetit_Hold_Choice_Kill"; 
		break;
		
		case "TohmasPetit_Gratitude":
			dialog.text = "Спасибо, что доставили нас. Вот ваше вознаграждение, как и договаривались.";
			link.l1 = "Удачи вам.";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, 200000); 
			AddDialogExitQuest("TohmasPetit_RunAway");
		break;

		case "exit":
			if (CheckAttribute(NextDiag,"TempNode")) NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
