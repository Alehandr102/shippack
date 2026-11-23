bool isDrawShootDistance = false;

string sEType[9] = {"blade","cirass","suit","talisman","jewelry_indian_left","jewelry_indian_right","indian_center","idols_left","idols_right"};
int CheckItemInSets(ref _character, string sItemName)
{
	if (!checkattribute(_character, "selectedSet")) return 0; //у этого персонажа не было сохранено комплектов
	int i, q;
	string sSET, sTemp;
	string sGunNow, sShelved, sP, sM;//Gregg: для проверки огнестрела в комплектах
	for(i=1;i<7;i++)//6 комплектов
	{
		if (_character.selectedSet == i) continue;//текущий комплект уже скрыт в списке перемещения предметов
		sSET = "Set" + i;
		if (!checkattribute(_character, sSET)) continue; //нет комплекта
		for (q=0;q<9;q++)
		{
			sTemp = sEType[q];
			if(_character.(sSET).(sTemp) == sItemName && GetCharacterEquipByGroup(_character, sTemp) != sItemName) return i;
				//предмет есть в неактивном комплекте И не экипирован прямо сейчас						//возвращаем номер комплекта
		}
		//Gregg: огнестрел хранится отдельной веткой Set<N>.gun.pistol/.mushket — проверяем и её.
		//"Сейчас экипирован" для ствола = он в слоте GUN (у мушкетёра это мушкет) ИЛИ это «нижний» пистоль под мушкетом.
		if (CheckAttribute(_character, sSET + ".gun.mode"))
		{
			sGunNow = GetCharacterEquipByGroup(_character, GUN_ITEM_TYPE);
			sShelved = GetParkedPistol(_character);
			sP = "";	if (CheckAttribute(_character, sSET + ".gun.pistol"))  sP = _character.(sSET).gun.pistol;
			sM = "";	if (CheckAttribute(_character, sSET + ".gun.mushket")) sM = _character.(sSET).gun.mushket;
			if (sP == sItemName && sP != sGunNow && sP != sShelved) return i;
			if (sM == sItemName && sM != sGunNow && sM != sShelved) return i;
		}
	}
	return 0;
}
//---------------------------------------------------------------------------------------------------
// scrollimage
//---------------------------------------------------------------------------------------------------
void FillScrollImageWithCompanionShips(string sNodeName, int iNotUsed)
{

	DeleteAttribute(&GameInterface, sNodeName);
	GameInterface.(sNodeName).current = -1;

	GameInterface.(sNodeName).ImagesGroup.t0 = "BLANK_SHIP2";
	GameInterface.(sNodeName).BadTex1 = 0;
	GameInterface.(sNodeName).BadPic1 = "Not Used2";

	FillShipList(sNodeName + ".ImagesGroup", pchar);

	string attributeName, shipName;
	string potc = "";
	int iShipType, cn;
	int iListSize = 0;
	if (InterfaceStates.InterfaceShipString == "interfaces\ships_PotC\") potc = "_POTC";

	int isPossibleToFill = 1;
	for(int i= 0; i< COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);

		if(cn!= -1)
		{
				isPossibleToFill = 1;
			if(isPossibleToFill == 1)
			{
				iShipType = sti(characters[cn].ship.type);
				if(iShipType != SHIP_NOTUSED)
				{
					if (iShipType!= SHIP_NOTUSED)
					{
						attributeName = "pic" + (iListSize+1);

						//Boyer change #20170430-03
                        ref rBaseShip = GetRealShip(iShipType);
                        iShipType = rBaseShip.basetype;
                        //#20170430-03 Companion ships are appended with a "1"
                        if(strlen(rBaseShip.Name)>2)
                            shipName = strcut(rBaseShip.Name, 0, strlen(rBaseShip.Name)-2);
                        else
                            shipName = "";


						shipName = ShipsTypes[iShipType].Name;
						attributeName = "pic" + (iListSize+1);

						GameInterface.(sNodeName).(attributeName).companionIndex = cn;
						GameInterface.(sNodeName).(attributeName).img1 = "ship";
						GameInterface.(sNodeName).(attributeName).tex1 = FindFaceGroupNum(sNodeName + ".ImagesGroup","SHIPS_"+shipName+potc);
						if (sti(rBaseShip.Type.Merchant)) GameInterface.(sNodeName).(attributeName).str1 = "#"+"Í";
						if (sti(rBaseShip.Type.War)) GameInterface.(sNodeName).(attributeName).str2 = "#"+"É";

						iListSize++;
					}
				}
			}
		}
	}

	GameInterface.(sNodeName).ListSize = iListSize;
	GameInterface.SHIPS_SCROLL.NotUsed = iNotUsed - iListSize + 1;

	SendMessage(&GameInterface, "lsl", MSG_INTERFACE_SCROLL_CHANGE, sNodeName, -1);
}

void FillScrollImageWithCompanions(string sNodeName, int iNotUsed)
{
	DeleteAttribute(&GameInterface, sNodeName);
	GameInterface.(sNodeName).current = -1;

	GameInterface.(sNodeName).ImagesGroup.t0 = "BLANK_SHIP2";
	GameInterface.(sNodeName).BadTex1 = 0;
	GameInterface.(sNodeName).BadPic1 = "Not Used2";

	FillShipList(sNodeName + ".ImagesGroup", pchar);

	string attributeName, shipName;
	string potc = "";
	if (InterfaceStates.InterfaceShipString == "interfaces\ships_PotC\") potc = "_POTC";
	int iShipType, cn;
	int iListSize = 0;

	int isPossibleToFill = 1;
	for(int i= 1; i< COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);

		if(cn!= -1)
		{
				isPossibleToFill = 1;
			if(isPossibleToFill == 1)
			{
				iShipType = sti(characters[cn].ship.type);
				if(iShipType != SHIP_NOTUSED)
				{
					if (iShipType!= SHIP_NOTUSED)
					{
						attributeName = "pic" + (iListSize+1);

						//Boyer change #20170430-03
                        ref rBaseShip = GetRealShip(iShipType);
                        iShipType = rBaseShip.basetype;
                        //#20170430-03 Companion ships are appended with a "1"
                        if(strlen(rBaseShip.Name)>2)
                            shipName = strcut(rBaseShip.Name, 0, strlen(rBaseShip.Name)-2);
                        else
                            shipName = "";


						shipName = ShipsTypes[iShipType].Name;
						attributeName = "pic" + (iListSize+1);

						GameInterface.(sNodeName).(attributeName).companionIndex = cn;
						GameInterface.(sNodeName).(attributeName).img1 = "ship";
						GameInterface.(sNodeName).(attributeName).tex1 = FindFaceGroupNum(sNodeName + ".ImagesGroup","SHIPS_"+shipName+potc);
						if (sti(rBaseShip.Type.Merchant)) GameInterface.(sNodeName).(attributeName).str1 = "#"+"Í";
						if (sti(rBaseShip.Type.War)) GameInterface.(sNodeName).(attributeName).str2 = "#"+"É";

						iListSize++;
					}
				}
			}
		}
	}

	GameInterface.(sNodeName).ListSize = iListSize;
	GameInterface.SHIPS_SCROLL.NotUsed = iNotUsed - iListSize + 1;

	SendMessage(&GameInterface, "lsl", MSG_INTERFACE_SCROLL_CHANGE, sNodeName, -1);
}

void FillScrollImageWithCompanionsUp(string sNodeName, int iNotUsed)
{
	FillShipList(sNodeName + ".ImagesGroup", pchar);

	string attributeName, shipName;
	int iShipType, cn;
	int iListSize = 0;

	int isPossibleToFill = 1;
	for(int i= 1; i< COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);

		if(cn!= -1)
		{
				isPossibleToFill = 1;
			if(isPossibleToFill == 1)
			{
				iShipType = sti(characters[cn].ship.type);
				if(iShipType != SHIP_NOTUSED)
				{
					if (iShipType!= SHIP_NOTUSED)
					{
						attributeName = "pic" + (iListSize+1);

						//Boyer change #20170430-03
                        ref rBaseShip = GetRealShip(iShipType);
                        iShipType = rBaseShip.basetype;
                        //#20170430-03 Companion ships are appended with a "1"
                        if(strlen(rBaseShip.Name)>2)
                            shipName = strcut(rBaseShip.Name, 0, strlen(rBaseShip.Name)-2);
                        else
                            shipName = "";


						shipName = ShipsTypes[iShipType].Name;
						attributeName = "pic" + (iListSize+1);

						GameInterface.(sNodeName).(attributeName).tex1 = FindFaceGroupNum(sNodeName + ".ImagesGroup","SHIPS_"+shipName);

						iListSize++;
					}
				}
			}
		}
	}
	SendMessage(&GameInterface, "lsl", MSG_INTERFACE_SCROLL_CHANGE, sNodeName, -1);
}
//---------------------------------------------------------------------------------------------------
void FillScrollImageWithFaces(string sNodeName, int iNotUsed, bool bCompanions, bool bPassengers)
{

	DeleteAttribute(&GameInterface, sNodeName);
	GameInterface.(sNodeName).current = -1;

	GameInterface.(sNodeName).ImagesGroup.t0 = "EMPTYFACE";
	GameInterface.(sNodeName).BadTex1 = 0;
	GameInterface.(sNodeName).BadPic1 = "emptyface";

	ref refCharacter = pchar;

	int iListSize = 0;
	int cn;
	string attributeName;
	aref pRef,pRef2;
	string PsgAttrName;


	if (bCompanions){

		FillFaceList(sNodeName + ".ImagesGroup", refCharacter, 1); // companions

		for(int n= 0; n< COMPANION_MAX; n++)
		{
			attributeName = "pic" + (iListSize+1);
			cn = GetCompanionIndex(refCharacter, n);

			if(cn!= -1)
			{
				GameInterface.(sNodeName).(attributeName).character = cn;
				GameInterface.(sNodeName).(attributeName).img1 = GetFacePicName(GetCharacter(cn));
				GameInterface.(sNodeName).(attributeName).tex1 = FindFaceGroupNum(sNodeName + ".ImagesGroup","FACE128_"+ characters[cn].FaceID);

				iListSize++;
			}
		}
	}

	if (bPassengers){


		makearef(pRef,refCharacter.Fellows.Passengers);
		FillFaceList(sNodeName + ".ImagesGroup", refCharacter, 2); // passengers

		for(int i= 0; i< GetPassengersQuantity(refCharacter); i++)
		{
			attributeName = "pic" + (iListSize+1);
			PsgAttrName = "id"+(i+1);
			cn = sti(pRef.(PsgAttrName));

			if(cn!=-1 && !CheckAttribute(&characters[cn], "isfree"))
			{
				GameInterface.(sNodeName).(attributeName).character = cn;
				GameInterface.(sNodeName).(attributeName).img1 = GetFacePicName(GetCharacter(cn));
				GameInterface.(sNodeName).(attributeName).tex1 = FindFaceGroupNum(sNodeName + ".ImagesGroup","FACE128_"+ characters[cn].FaceID);

				iListSize++;
			}
		}
	}
	GameInterface.(sNodeName).ListSize = iListSize;
	GameInterface.(sNodeName).NotUsed = iNotUsed;

	SendMessage(&GameInterface, "lsl", MSG_INTERFACE_SCROLL_CHANGE, sNodeName, -1);
}
/// boal -->
void StartAboveForm(bool _pauseTime)
{
    ChangeShowIntarface();

	EngineLayersOffOn(true);
    if (_pauseTime)
    {
		SetTimeScale(0.0);
	}
	locCameraSleep(true);
	LAi_CharacterDisableDialog(pchar);
}

void EndAboveForm(bool _pauseTime)
{
    if (_pauseTime)
    {
		SetTimeScale(1.0);
		TimeScaleCounter = 0;
	 	DelPerkFromActiveList("TimeSpeed");
	 	DeleteAttribute(pchar, "pause");
	}
	locCameraSleep(false);
	LAi_CharacterEnableDialog(pchar);
	ChangeShowIntarface();
}

void ChangeShowIntarface()
{
    if (bSeaActive && !bAbordageStarted)
    {
        if (!IsEntity(BattleInterface))
        {
            InitBattleInterface();
            StartBattleInterface();
            RefreshBattleInterface();
        }
        else DeleteBattleInterface();
    }
    else
    {
		if (!IsEntity(worldMap))
		{
			if (!bLandInterfaceStart)
	        {
	            StartBattleLandInterface();
	        }
	        else EndBattleLandInterface();
        }
    }
}
// вернуть тип по номеру скрола в ф2 персонаж
string GetOfficerTypeByNum(int nCurScrollNum)
{
    string ret = "";

	switch (nCurScrollNum)
	{
		case 1:
   			ret = "navigator";
		break;

		case 2:
   			ret = "boatswain";
		break;

		case 3:
   			ret = "cannoner";
		break;

		case 4:
   			ret = "doctor";
		break;

		case 5:
   			ret = "treasurer";
		break;

		case 6:
   			ret = "carpenter";
		break;

		//Boyer mod
		//default
			ret = "fighter";
		//End Boyer add
	}
	return ret;
}

////////////////////////////// общие методы для форм
void SetOTHERMiniTable(string _tabName, ref _chr)
{
    int     i;
	string  row, skillName, col;
    int     skillVal;

    GameInterface.(_tabName).select = 0;
	GameInterface.(_tabName).hr.td1.str = "";
	for (i=1; i<=5; i++)
	{
	    row = "tr" + i;

	    GameInterface.(_tabName).(row).td1.icon.width = 23;
    	GameInterface.(_tabName).(row).td1.icon.height = 23;
    	GameInterface.(_tabName).(row).td1.icon.offset = "-2, 2";
		GameInterface.(_tabName).(row).td2.align = "left";
		GameInterface.(_tabName).(row).td2.scale = 0.8;
		GameInterface.(_tabName).(row).td2.textoffset = "-2,-1";
		GameInterface.(_tabName).(row).td3.align = "right";
	}
	GameInterface.(_tabName).tr1.UserData.ID = "Rank";
	GameInterface.(_tabName).tr1.td1.icon.group = "ICONS_CHAR";
    GameInterface.(_tabName).tr1.td1.icon.image = "Rank";
	GameInterface.(_tabName).tr1.td2.str = XI_ConvertString("Rank");
	GameInterface.(_tabName).tr1.td3.str = sti(_chr.rank);

	GameInterface.(_tabName).tr2.UserData.ID = "Life";
	GameInterface.(_tabName).tr2.td1.icon.group = "ICONS_CHAR";
    GameInterface.(_tabName).tr2.td1.icon.image = "Life";
	GameInterface.(_tabName).tr2.td2.str = XI_ConvertString("Life");
	GameInterface.(_tabName).tr2.td3.str = MakeInt(LAi_GetCharacterHP(_chr)) + " / " + MakeInt(LAi_GetCharacterMaxHP(_chr));

    GameInterface.(_tabName).tr3.UserData.ID = "Reputation";
	GameInterface.(_tabName).tr3.td1.icon.group = "ICONS_CHAR";
    GameInterface.(_tabName).tr3.td1.icon.image = "Reputation";
	GameInterface.(_tabName).tr3.td2.str = XI_ConvertString("Reputation");
	GameInterface.(_tabName).tr3.td3.str = XI_ConvertString(GetReputationName(sti(_chr.reputation)));
	GameInterface.(_tabName).tr3.td3.scale = 0.8;
	GameInterface.(_tabName).tr3.td3.textoffset = "0,-1";

    GameInterface.(_tabName).tr4.UserData.ID = "Loyality";
	GameInterface.(_tabName).tr4.td1.icon.group = "ICONS_CHAR";
    GameInterface.(_tabName).tr4.td1.icon.image = "Loyality";
	GameInterface.(_tabName).tr4.td2.str = XI_ConvertString("Loyality");
	if (_chr.id == pchar.id)
	{
		GameInterface.(_tabName).tr4.td3.str = "-";
	}
	else
	{
		GameInterface.(_tabName).tr4.td3.str = XI_ConvertString(GetLoyalityName(makeint(GetCharacterLoyality(_chr) * 100 / MAX_LOYALITY)));
	}
	GameInterface.(_tabName).tr4.td3.scale = 0.78;

	GameInterface.(_tabName).tr5.UserData.ID = "weight";
	GameInterface.(_tabName).tr5.td1.icon.group = "ICONS_CHAR";
    GameInterface.(_tabName).tr5.td1.icon.image = "weight";
	GameInterface.(_tabName).tr5.td2.str = XI_ConvertString("weight");
	GameInterface.(_tabName).tr5.td3.str = FloatToString(GetItemsWeight(_chr), 1) + " / "+GetMaxItemsWeight(_chr);
	GameInterface.(_tabName).tr5.td3.scale = 0.8;

	// прорисовка
	Table_UpdateWindow(_tabName);
}
void SetSPECIALMiniTable(string _tabName, ref _chr)
{
    int     i;
	string  row, skillName, col;
    int     diff, skillVal;

    GameInterface.(_tabName).select = 0;
	GameInterface.(_tabName).hr.td1.str = "";
	row = "tr1";
	for (i=1; i<=7; i++)
	{
	    col = "td" + i;
	    skillName = GetSkillNameByTRIdx("SPECIALType", i);

		GameInterface.(_tabName).(row).(col).fontidx = 0;
		//#20170627-01
		GameInterface.(_tabName).(row).(col).scale = 0.65;
		GameInterface.(_tabName).(row).(col).textoffset = "-5,-6";
		GameInterface.(_tabName).(row).(col).str = XI_ConvertString(skillName + "T");
	}
	row = "tr2";
	for (i=1; i<=7; i++)
	{
	    col = "td" + i;
	    skillName = GetSkillNameByTRIdx("SPECIALType", i);
        skillVal = GetCharacterSPECIAL(_chr, skillName);
		GameInterface.(_tabName).(row).(col).fontidx = 1;
		//#20170627-01
		GameInterface.(_tabName).(row).(col).scale = 1.0;
		GameInterface.(_tabName).(row).(col).textoffset = "-5,-4";
		GameInterface.(_tabName).(row).(col).align = "center";

		diff = skillVal - GetSkillValue(_chr, SPECIAL_TYPE, skillName);
		GameInterface.(_tabName).(row).(col).str = skillVal;
		if (diff == 0)
		{
     		GameInterface.(_tabName).(row).(col).color = argb(255,255,255,255);
		}
		else
		{
		   if (diff > 0)
		   {
	          GameInterface.(_tabName).(row).(col).color = argb(255,196,255,196);
	       }
	       else
	       {
	          GameInterface.(_tabName).(row).(col).color = argb(255,255,196,196);
	       }
		}
	}
	row = "tr3";
	for (i=1; i<=7; i++)
	{
	    col = "td" + i;
	    skillName = GetSkillNameByTRIdx("SelfType", i);

		GameInterface.(_tabName).(row).(col).icon.group = "ICONS_SPEC";
	    GameInterface.(_tabName).(row).(col).icon.image = skillName + " skill icon";
	    GameInterface.(_tabName).(row).(col).icon.width = 30;
    	GameInterface.(_tabName).(row).(col).icon.height = 30;
		GameInterface.(_tabName).(row).(col).icon.offset = "0,0";
	}
	row = "tr4";
	for (i=1; i<=7; i++)
	{
	    col = "td" + i;
	    skillName = GetSkillNameByTRIdx("SelfType", i);
        skillVal = GetSummonSkillFromName(_chr, skillName);
		GameInterface.(_tabName).(row).(col).fontidx = 1;
		//#20170627-01
		GameInterface.(_tabName).(row).(col).scale = 0.8;
		GameInterface.(_tabName).(row).(col).textoffset = "-5,-4";
		GameInterface.(_tabName).(row).(col).align = "center";

		diff = skillVal - GetSkillValue(_chr, SKILL_TYPE, skillName);
		GameInterface.(_tabName).(row).(col).str = skillVal;
		if (diff == 0)
		{
     		GameInterface.(_tabName).(row).(col).color = argb(255,255,255,255);
		}
		else
		{
		   if (diff > 0)
		   {
	          GameInterface.(_tabName).(row).(col).color = argb(255,196,255,196);
	       }
	       else
	       {
	          GameInterface.(_tabName).(row).(col).color = argb(255,255,196,196);
	       }
		}
	}

	row = "tr5";
	for (i=1; i<=7; i++)
	{
	    col = "td" + i;
	    skillName = GetSkillNameByTRIdx("ShipType", i);

		GameInterface.(_tabName).(row).(col).icon.group = "ICONS_SPEC";
	    GameInterface.(_tabName).(row).(col).icon.image = skillName + " skill icon";
	    GameInterface.(_tabName).(row).(col).icon.width = 30;
    	GameInterface.(_tabName).(row).(col).icon.height = 30;
		GameInterface.(_tabName).(row).(col).icon.offset = "0,0";
	}
	row = "tr6";
	for (i=1; i<=7; i++)
	{
	    col = "td" + i;
	    skillName = GetSkillNameByTRIdx("ShipType", i);
        skillVal = GetSummonSkillFromName(_chr, skillName);
		GameInterface.(_tabName).(row).(col).fontidx = 1;
		//#20170627-01
		GameInterface.(_tabName).(row).(col).scale = 0.8;
		GameInterface.(_tabName).(row).(col).textoffset = "-5,-4";
		GameInterface.(_tabName).(row).(col).align = "center";

		diff = skillVal - GetSkillValue(_chr, SKILL_TYPE, skillName);
		GameInterface.(_tabName).(row).(col).str = skillVal;
		if (diff == 0)
		{
     		GameInterface.(_tabName).(row).(col).color = argb(255,255,255,255);
		}
		else
		{
		   if (diff > 0)
		   {
	          GameInterface.(_tabName).(row).(col).color = argb(255,196,255,196);
	       }
	       else
	       {
	          GameInterface.(_tabName).(row).(col).color = argb(255,255,196,196);
	       }
		}
	}
	// прорисовка
	Table_UpdateWindow(_tabName);
}

// опыт команды нпс _chr
void SetCrewExpTable(ref _chr, string _tabName, string _bar1, string _bar2, string _bar3)
{
	SetCrewExpTableEx(_chr, _tabName, _bar1, _bar2, _bar3, false);
}

// LEO: bWnd = показывать БУФЕР РАНЕНЫХ (свой опыт/мораль лазарета) вместо активной команды.
// Обёртка выше сохраняет старую сигнатуру, чтобы не трогать все остальные точки вызова.
void SetCrewExpTableEx(ref _chr, string _tabName, string _bar1, string _bar2, string _bar3, bool bWnd)
{
    int     i;
	string  row, skillName, col;
    int     skillVal;
	int     iBodies;
	int     iSail, iCann, iSold;

	// LEO: чьи цифры показываем и есть ли вообще кого показывать
	if (bWnd)
	{
		iBodies = GetWoundedQuantity(_chr);
		iSail = makeint(GetWoundedExpSailors(_chr));
		iCann = makeint(GetWoundedExpCannoners(_chr));
		iSold = makeint(GetWoundedExpSoldiers(_chr));
	}
	else
	{
		// раненые - тоже люди на борту, пул опыта корабля жив даже когда актив выбит в ноль
		iBodies = GetCrewQuantity(_chr) + GetWoundedQuantity(_chr);
		iSail = sti(_chr.Ship.Crew.Exp.Sailors);
		iCann = sti(_chr.Ship.Crew.Exp.Cannoners);
		iSold = sti(_chr.Ship.Crew.Exp.Soldiers);
	}

    GameInterface.(_tabName).select = 0;
	GameInterface.(_tabName).hr.td1.str = "";
	for (i=1; i<=3; i++)
	{
	    row = "tr" + i;

	    GameInterface.(_tabName).(row).td1.icon.width = 23;
    	GameInterface.(_tabName).(row).td1.icon.height = 23;
    	GameInterface.(_tabName).(row).td1.icon.offset = "0, 2";
		GameInterface.(_tabName).(row).td2.align = "left";
		GameInterface.(_tabName).(row).td2.scale = 0.8;
		GameInterface.(_tabName).(row).td2.textoffset = "3,0";
		GameInterface.(_tabName).(row).td3.align = "right";
		GameInterface.(_tabName).(row).td3.scale = 0.8;
	}
	GameInterface.(_tabName).tr1.UserData.ID = "Sailors";
	GameInterface.(_tabName).tr1.td1.icon.group = "ICONS_CHAR";
    GameInterface.(_tabName).tr1.td1.icon.image = "Class";
	GameInterface.(_tabName).tr1.td2.str = XI_ConvertString("Sailors");
	if (iBodies > 0)
    {
		GameInterface.(_tabName).tr1.td3.str = XI_ConvertString(GetExpName(iSail));
	}
	else
	{
		GameInterface.(_tabName).tr1.td3.str = "";
	}

	GameInterface.(_tabName).tr2.UserData.ID = "Cannoners";
	GameInterface.(_tabName).tr2.td1.icon.group = "ICONS_SPEC";
    GameInterface.(_tabName).tr2.td1.icon.image = "Cannons skill icon";
	GameInterface.(_tabName).tr2.td2.str = XI_ConvertString("Cannoners");
	if (iBodies > 0)
    {
		GameInterface.(_tabName).tr2.td3.str = XI_ConvertString(GetExpName(iCann));
	}
		else
	{
		GameInterface.(_tabName).tr2.td3.str = "";
	}

    GameInterface.(_tabName).tr3.UserData.ID = "Soldiers";
	GameInterface.(_tabName).tr3.td1.icon.group = "ICONS_SPEC";
    GameInterface.(_tabName).tr3.td1.icon.image = "grappling skill icon";
	GameInterface.(_tabName).tr3.td2.str = XI_ConvertString("Soldiers");
	if (iBodies > 0)
    {
		GameInterface.(_tabName).tr3.td3.str = XI_ConvertString(GetExpName(iSold));
	}
		else
	{
		GameInterface.(_tabName).tr3.td3.str = "";
	}
	// прорисовка
	Table_UpdateWindow(_tabName);

	///  прогресбары
	GameInterface.StatusLine.(_bar1).Max   = 100;
    GameInterface.StatusLine.(_bar1).Min   = 1;
    if (iBodies > 0)
    {
    	GameInterface.StatusLine.(_bar1).Value = iSail;
    }
    else
    {
    	GameInterface.StatusLine.(_bar1).Value = 1;
    }
    SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, _bar1,0);

    GameInterface.StatusLine.(_bar2).Max   = 100;
    GameInterface.StatusLine.(_bar2).Min   = 1;
    if (iBodies > 0)
    {
    	GameInterface.StatusLine.(_bar2).Value = iCann;
    }
    else
    {
    	GameInterface.StatusLine.(_bar2).Value = 1;
    }
    SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, _bar2,0);

    GameInterface.StatusLine.(_bar3).Max   = 100;
    GameInterface.StatusLine.(_bar3).Min   = 1;
    if (iBodies > 0)
    {
    	GameInterface.StatusLine.(_bar3).Value = iSold;
    }
    else
    {
    	GameInterface.StatusLine.(_bar3).Value = 1;
    }
    SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, _bar3,0);
}

void SetShipOTHERTable(string _tabName, ref _chr)
{
	int i;
	string row;

	int iShip = sti(_chr.ship.type);
	ref refBaseShip = GetRealShip(iShip);
	if (iShip >= REAL_SHIPS_QUANTITY) {
		return;
	}

	aref chrship;
	makearef(chrship, _chr.Ship);

	GameInterface.(_tabName).select = 0;
	GameInterface.(_tabName).hr.td1.str = "";
	for (i = 1; i <= 10; i++)
	{
		row = "tr" + i;

		GameInterface.(_tabName).(row).td1.icon.width = 24;
		GameInterface.(_tabName).(row).td1.icon.height = 24;
		GameInterface.(_tabName).(row).td1.icon.offset = "2, 1";
		GameInterface.(_tabName).(row).td2.align = "left";
		GameInterface.(_tabName).(row).td2.scale = 0.8;
		GameInterface.(_tabName).(row).td2.textoffset = "0,0";
		GameInterface.(_tabName).(row).td3.align = "right";
		GameInterface.(_tabName).(row).td3.scale = 0.8;
	}

	string sText2, sAdditionalInfo;//red and green text

	//red text
	if (GetHullPercent(_chr) < 100.0 && GetHullRepairDay(_chr, false)) {//блок появления если корабль без кэпа
		sText2 = XI_ConvertString("Repair") + ": "
		+ FloatToString(GetHullRepairDay(_chr, false), 2)
		+ XI_ConvertString("repair2") + " "
		+ makeint(GetHullRepairDay(_chr, true))
		+ XI_ConvertString("Pcs") + " "
		+ XI_ConvertString("PlanksGen");
	}

	//green text
	// ship.boxsize заводит ДВИЖОК при постановке корабля в морскую сцену, у не плававшего его нет.
	//   Раньше мы просто НЕ ПЕЧАТАЛИ строку про длину (чтобы убрать спам в error.log), теперь габарит
	//   считает GetShipBoxSize - с фолбэком на .gm через движковую GetShipModelBox, то есть числа
	//   показываются и на стапеле.
	sAdditionalInfo = XI_ConvertString("ship_length") + ": "
	+ makeint(GetShipBoxSize(_chr, 2)) + " "
	+ XI_ConvertString("m") + ", "
	+ XI_ConvertString("bort_height") + ": "
	+ sti(refBaseShip.fireheight) + " "
	+ XI_ConvertString("m") + "\n"
	+ XI_ConvertString("Hull") + ": "
	+ FloatToString(GetHullPercent(_chr), 1) + "%";
	if (!ShipSpeedBonusFromHP(_chr, refBaseShip)) {
		sAdditionalInfo += ", " + XI_ConvertString("Penalty") + ": -"
		+ FloatToString(100.0 - 100 * ShipSpeedBonusFromHP(_chr, refBaseShip), 1) + "% "
		+ XI_ConvertString("of_speed_and") + " -"
		+ FloatToString(100.0 - 100 * ShipTurnRateBonusFromHP(_chr, refBaseShip), 1) + "% "
		+ XI_ConvertString("of_maneuver");
	}
	else {
		sAdditionalInfo += ", " + XI_ConvertString("hp_reserve") + ": "
		+ FloatToString((GetHullPercent(_chr) - SHIP_HP_PENALTY_THRESH), 1) + "%";
	}
	float fMinusArmor = stf(RealShips[sti(_chr.Ship.Type)].HullArmor) / 2.0;
	float fCaliber = GetCannonCaliber(sti(_chr.Ship.Cannons.Type));
	float fResistValue;
	bool bIsCannonsInstalled = true;
	ref rCannon = GetCannonByType(sti(_chr.Ship.Cannons.Type));
	//если пушек не стоит то лучше показывать защиту от макс доступного калибра, т.е. паспортную
	if (MakeInt(fCaliber) <= 8)
	{
		Makeref(rCannon, Cannon[GetCannonByTypeAndCaliber("cannon", sti(refBaseShip.maxcaliber))]);
		fCaliber = refBaseShip.maxcaliber;
		bIsCannonsInstalled = false;
	}
	if (fMinusArmor > 0.75 * fCaliber) { fResistValue = 0.75; }//больше 75% урона броня не поглощает
	else { fResistValue = 1 - ((fCaliber - fMinusArmor) / fCaliber); }
	if (fResistValue > 0.75) { fResistValue = 0.75; }

	sAdditionalInfo += "\n\n" + XI_ConvertString("DR_from") + " "
	+ fCaliber + " " + XI_ConvertString("lbs");
	if (bIsCannonsInstalled) {
		sAdditionalInfo += " (" + XI_ConvertString("installed_on_ship") + ")";
	}
	sAdditionalInfo += ":\n" + MakeInt(100 * fResistValue) + "% "
	+ XI_ConvertString("OnShip") + " "
	+ MakeInt(0.2 * GetShootDistance(_chr, "ball")) + " "
	+ XI_ConvertString("m") + "\n";

	fResistValue = 1 - ((0.8 * fCaliber - fMinusArmor) / (0.8 * fCaliber));//TODO переписать hullhitEvent под расчеты

	sAdditionalInfo += MakeInt(100 * fResistValue) + "% "
	+ XI_ConvertString("OnShip") + " "
	+ GetShootDistance(_chr, "ball") + " "
	+ XI_ConvertString("m") + "\n\n "
	+ XI_ConvertString("Flames_time_damage") + ": "
	+ MakeInt(Ship_GetTotalFireTime(_chr)) + " "
	+ XI_ConvertString("sec.");
	//типа форматирование 
	if (SHIP_DEFAULT_FIRE_TIME != MakeInt(Ship_GetTotalFireTime(_chr))) {
		sAdditionalInfo += " (";
		if (SHIP_DEFAULT_FIRE_TIME < MakeInt(Ship_GetTotalFireTime(_chr))) {
			sAdditionalInfo += "+";
		}
		sAdditionalInfo += MakeInt(100 * (Ship_GetTotalFireTime(_chr) - SHIP_DEFAULT_FIRE_TIME) / SHIP_DEFAULT_FIRE_TIME)
		+ "% " + XI_ConvertString("from_skill") + " "
		+ XI_ConvertString("Repair") + ")";
	}

	GameInterface.(_tabName).tr1.UserData.ID = "Hull";
	GameInterface.(_tabName).tr1.UserData.ID1 = sText2;
	GameInterface.(_tabName).tr1.UserData.ID2 = sAdditionalInfo;
	GameInterface.(_tabName).tr1.td1.icon.group = "ICONS_CHAR";
	GameInterface.(_tabName).tr1.td1.icon.image = "Hull";
	GameInterface.(_tabName).tr1.td2.str = XI_ConvertString("Hull");
	GameInterface.(_tabName).tr1.td3.str = sti(_chr.ship.hp) + " / " + sti(refBaseShip.hp);
	GameInterface.(_tabName).tr1.tocompare = sti(refBaseShip.hp);

	if (!CheckAttribute(&RealShips[iShip], "Tuning.HP"))
	{
		GameInterface.(_tabName).tr1.td3.color = argb(255, 255, 255, 255);
	}
	else
	{
		GameInterface.(_tabName).tr1.td3.color = argb(255, 128, 200, 225);
	}

	//red text
	if (GetSailPercent(_chr) < 100.0 && GetSailRepairDay(_chr, false)) {//блок появления если корабль без кэпа
		sText2 = XI_ConvertString("Repair") + ": "
		+ FloatToString(GetSailRepairDay(_chr, false), 2)
		+ XI_ConvertString("Repair2") + " "
		+ makeint(GetSailRepairDay(_chr, true))
		+ XI_ConvertString("Pcs") + " "
		+ XI_ConvertString("SailclothGen");
	}
	else { sText2 = ""; }//чтоб не показывало ремонт корпуса в подсказке  парусов

	//green text
	// габариты такелажа - через GetShipBoxSize (см. комментарий выше про ship.boxsize)
	sAdditionalInfo = XI_ConvertString("sails_height") + ": "
	+ makeint(GetShipBoxSize(_chr, 1)) + " "
	+ XI_ConvertString("m") + ", "
	+ XI_ConvertString("sails_width") + ": "
	+ makeint(GetShipBoxSize(_chr, 0)) + " "
	+ XI_ConvertString("m") + "\n\n"
	+ xiStr("Sails") + ": "
	+ FloatToString(GetSailPercent(_chr), 1) + "%";
	if (!ShipSpeedBonusFromSails(_chr)) {
		sAdditionalInfo += ", " + XI_ConvertString("Penalty") + ": -"
		+ FloatToString(100.0 - ShipSpeedBonusFromSails(_chr) * 100, 1) + "% "
		+ XI_ConvertString("speed_div_maneuver") + "";
	}
	else {
		sAdditionalInfo += ", " + XI_ConvertString("sail_hp_reserve") + ": "
		+ FloatToString((GetSailPercent(_chr) - SHIP_SP_PENALTY_THRESH), 1) + "%";
	}

	GameInterface.(_tabName).tr2.UserData.ID = "Sails";
	GameInterface.(_tabName).tr2.UserData.ID1 = sText2;
	GameInterface.(_tabName).tr2.UserData.ID2 = sAdditionalInfo;
	GameInterface.(_tabName).tr2.td1.icon.group = "ICONS_CHAR";
	GameInterface.(_tabName).tr2.td1.icon.image = "Sails";
	GameInterface.(_tabName).tr2.td2.str = XI_ConvertString("Sails");
	GameInterface.(_tabName).tr2.td3.str = sti(_chr.ship.sp) + " / " + sti(refBaseShip.sp);

	//green text
	string sMastDescr = "";//перечисление поврежденных мачт для подсказки по правой кнопке
	float fBaddestMast = 0.0;//ищем самую поврежденную мачту

	if (checkattribute(refBaseShip, "mastHP"))
	{
		string sMastNum;
		float fMast;
		int iCount = 0;//число мачт с повреждениями
		aref arMasts;
		makearef(arMasts, refBaseShip.mastHP);
		for (int iM = 0; iM < GetAttributesNum(arMasts); iM++)
		{
			sMastNum = GetAttributeName(GetAttributeN(arMasts, iM));
			fMast = GetAttributeValue(GetAttributeN(arMasts, iM));
			if (checkattribute(refBaseShip, "mastHPRep." + sMastNum)) { fMast -= makefloat(refBaseShip.mastHPRep.(sMastNum)); }//учитываем отложенный ремонт
			if (fMast > 0.005)
			{
				iCount++;
				sMastDescr += XI_ConvertString("Mast_cond") + sMastNum + ": " + makeint((1 - fMast) * 100) + "%\n";
			}
			if (fMast > fBaddestMast && fMast < 1.0) fBaddestMast = fMast;
		}
		if (iCount == 0) { sMastDescr = XI_ConvertString("Masts_intact"); }
	}
	else { sMastDescr = XI_ConvertString("Masts_intact"); }

	GameInterface.(_tabName).tr3.UserData.ID = "MastHP";
	GameInterface.(_tabName).tr3.UserData.ID1 = "";
	GameInterface.(_tabName).tr3.UserData.ID2 = sMastDescr;
	GameInterface.(_tabName).tr3.td1.icon.group = "ICONS_CHAR";
	GameInterface.(_tabName).tr3.td1.icon.image = "MastHP";
	GameInterface.(_tabName).tr3.td2.str = XI_ConvertString("MastHP");
	GameInterface.(_tabName).tr3.td3.str = FloatToString(stf(refBaseShip.MastMultiplier), 2);
	GameInterface.(_tabName).tr3.tocompare = FloatToString(stf(refBaseShip.MastMultiplier), 2);
	if (fBaddestMast > 0.005) GameInterface.(_tabName).tr3.td3.str = "" + makeint((1 - fBaddestMast) * 100) + "% | " + FloatToString(stf(refBaseShip.MastMultiplier), 2);

	if (!CheckAttribute(&RealShips[iShip], "Tuning.MastMultiplier"))
	{
		GameInterface.(_tabName).tr3.td3.color = argb(255, 255, 255, 255);
	}
	else
	{
		GameInterface.(_tabName).tr3.td3.color = argb(255, 128, 200, 225);
	}

	//green text
	BI_UpdateRSRollSpeed(_chr);
	float fRollSpeed, fBonusTotal, fBonusBySkill, fBonusByPeople;
	string sAddtitinalInfoPC = "(" + XI_ConvertString("Skills_Perks") + ")";
	if (sti(_chr.skill.Sailing) > 1) {
		fRollSpeed = 3.5 / stf(_chr.Ship.RollSpeed);
		fBonusTotal = FindShipSpeedBonus(_chr, refBaseShip);
		fBonusBySkill = SpeedBySkill(_chr, iShip);
		fBonusByPeople = ShipSpeedBonusFromPeople(_chr, refBaseShip);
	}
	else {
		fRollSpeed = 3.5;
		fBonusTotal = FindShipSpeedBonus(_chr, refBaseShip) / SpeedBySkill(_chr, iShip) * SpeedBySkill(PChar, iShip) / ShipSpeedBonusFromPeople(_chr, refBaseShip);
		fBonusBySkill = SpeedBySkill(PChar, iShip);
		fBonusByPeople = 1;
		sAddtitinalInfoPC = "(" + XI_ConvertString("Skills_Perks_PC") + ")";
	}

	sAdditionalInfo = XI_ConvertString("speed_up_time") + ": ~"
	+ FloatToString(20 / stf(refBaseship.InertiaAccelerationZ), 1) + " "
	+ XI_ConvertString("sec.") + " (+"
	+ FloatToString(fRollSpeed, 1) + " "
	+ XI_ConvertString("sec.") + " - "
	+ XI_ConvertString("roll_speed") + ")\n"
	+ XI_ConvertString("speed") + ": "
	+ FloatToString(100 * fBonusTotal, 2) + "% = "
	+ FloatToString(100 * fBonusBySkill, 1) + "%" + sAddtitinalInfoPC;
	if (!ShipSpeedBonusFromUpgrades(_chr, refBaseShip)) {
		sAdditionalInfo += ShipSpeedBonusFromUpgrades(_chr, refBaseShip) + "% ("
		+ XI_ConvertString("upgrades") + ")";
	}
	if (!fBonusByPeople) {
		sAdditionalInfo += " * " + FloatToString(100 * fBonusByPeople, 1) + "% ("
		+ XI_ConvertString("crew") + ")";
	}
	if (!ShipSpeedBonusFromSails(_chr)) {
		sAdditionalInfo += " * " + FloatToString(100 * ShipSpeedBonusFromSails(_chr), 1) + "% ("
		+ XI_ConvertString("sails") + ")";
	}
	if (!ShipSpeedBonusFromHP(_chr, refBaseShip)) {
		sAdditionalInfo += " * " + FloatToString(100 * ShipSpeedBonusFromHP(_chr, refBaseShip), 1) + "% ("
		+ XI_ConvertString("hull") + ")";
	}
	if (!ShipSpeedBonusFromWeight(_chr, refBaseShip)) {
		sAdditionalInfo += " * " + FloatToString(100 * ShipSpeedBonusFromWeight(_chr, refBaseShip), 1) + "% ("
		+ XI_ConvertString("cargo") + ")";
	}
	if (!ShipSpeedBonusFromSoiling(_chr)) {
		sAdditionalInfo += " * " + FloatToString(100 * ShipSpeedBonusFromSoiling(_chr), 1) + "% ("
		+ XI_ConvertString("soiling") + ")";
	}

	GameInterface.(_tabName).tr4.UserData.ID = "Speed";
	GameInterface.(_tabName).tr4.UserData.ID1 = "";
	GameInterface.(_tabName).tr4.UserData.ID2 = sAdditionalInfo;
	GameInterface.(_tabName).tr4.td1.icon.group = "ICONS_CHAR";
	GameInterface.(_tabName).tr4.td1.icon.image = "Speed";
	GameInterface.(_tabName).tr4.td2.str = XI_ConvertString("Speed");
	if (IsCompanion(_chr))
	{
		GameInterface.(_tabName).tr4.td3.str = FloatToString(FindShipSpeed(_chr, refBaseShip), 2) + " / " + FloatToString(stf(refBaseShip.SpeedRate), 2);
	}
	else
	{
		GameInterface.(_tabName).tr4.td3.str = FloatToString(stf(refBaseShip.SpeedRate), 2);
	}
	if (!CheckAttribute(&RealShips[iShip], "Tuning.SpeedRate"))
	{
		GameInterface.(_tabName).tr4.td3.color = argb(255, 255, 255, 255);
	}
	else
	{
		GameInterface.(_tabName).tr4.td3.color = argb(255, 128, 200, 225);
	}

	//green text
	sAddtitinalInfoPC = "(" + XI_ConvertString("Skills_Perks") + ")";
	if (sti(_chr.skill.Sailing) > 1) {
		fRollSpeed = 180.0 / (stf(refBaseShip.turnrate) * FindShipTurnRate(_chr, refBaseShip));
		fBonusTotal = FindShipTurnRateBonus(_chr, refBaseShip);
		fBonusBySkill = TurnBySkill(_chr, iShip);
		fBonusByPeople = ShipTurnRateBonusFromPeople(_chr, refBaseShip);
	}
	else {
		fRollSpeed = 180.0 / stf(refBaseShip.turnrate);
		fBonusTotal = FindShipTurnRateBonus(_chr, refBaseShip) / TurnBySkill(_chr, iShip) * TurnBySkill(PChar, iShip) / ShipTurnRateBonusFromPeople(_chr, refBaseShip);
		fBonusBySkill = TurnBySkill(PChar, iShip);
		fBonusByPeople = 1;
		sAddtitinalInfoPC = "(" + XI_ConvertString("Skills_Perks_PC") + ")";
	}

	sAdditionalInfo = XI_ConvertString("turn_time") + ": ~"
	+ FloatToString(fRollSpeed, 1) + " "
	+ XI_ConvertString("sec.") + " + "
	+ FloatToString(10 / stf(refBaseship.InertiaAccelerationY), 1) + " "
	+ XI_ConvertString("sec.") + " "
	+ XI_ConvertString("turn_rate") + "\n"
	+ XI_ConvertString("maneuver") + ": "
	+ FloatToString(100 * fBonusTotal, 2) + "% = "
	+ FloatToString(100 * fBonusBySkill, 1) + "% " + sAddtitinalInfoPC;
	if (!ShipTurnRateBonusFromUpgrades(_chr, refBaseShip)) {
		sAdditionalInfo += ShipTurnRateBonusFromUpgrades(_chr, refBaseShip) + "% ("
		+ XI_ConvertString("upgrades") + ")";
	}
	if (!ShipTurnRateBonusFromPeople(_chr, refBaseShip)) {
		sAdditionalInfo += " * " + FloatToString(100 * fBonusByPeople, 1) + "% ("
		+ XI_ConvertString("crew") + ")";
	}
	if (!ShipTurnRateBonusFromSails(_chr)) {
		sAdditionalInfo += " * " + FloatToString(100 * ShipTurnRateBonusFromSails(_chr), 1) + "% ("
		+ XI_ConvertString("sails") + ")";
	}
	if (!ShipTurnRateBonusFromHP(_chr, refBaseShip)) {
		sAdditionalInfo += " * " + FloatToString(100 * ShipTurnRateBonusFromHP(_chr, refBaseShip), 1) + "% ("
		+ XI_ConvertString("hull") + ")";
	}
	if (!ShipTurnRateBonusFromWeight(_chr, refBaseShip)) {
		sAdditionalInfo += " * " + FloatToString(100 * ShipTurnRateBonusFromWeight(_chr, refBaseShip), 1) + "% ("
	   + XI_ConvertString("cargo") + ")";
	}
	if (!ShipTurnRateBonusFromSoiling(_chr)) {
		sAdditionalInfo += " * " + FloatToString(100 * ShipTurnRateBonusFromSoiling(_chr), 1) + "% ("
		+ XI_ConvertString("soiling") + ")";
	}


	GameInterface.(_tabName).tr5.UserData.ID = "Maneuver";
	GameInterface.(_tabName).tr5.UserData.ID1 = "";
	GameInterface.(_tabName).tr5.UserData.ID2 = sAdditionalInfo;
	GameInterface.(_tabName).tr5.td1.icon.group = "ICONS_CHAR";
	GameInterface.(_tabName).tr5.td1.icon.image = "Maneuver";
	GameInterface.(_tabName).tr5.td2.str = XI_ConvertString("Maneuver");
	if (IsCompanion(_chr))
	{
		GameInterface.(_tabName).tr5.td3.str = FloatToString((stf(refBaseShip.turnrate) * FindShipTurnRate(_chr, refBaseShip)), 2) + " / " + FloatToString(stf(refBaseShip.TurnRate), 2);
		GameInterface.(_tabName).tr5.tocompare = FloatToString(stf(refBaseShip.TurnRate), 2);
	}
	else
	{
		GameInterface.(_tabName).tr5.td3.str = FloatToString(stf(refBaseShip.TurnRate), 2);
	}
	if (!CheckAttribute(&RealShips[iShip], "Tuning.TurnRate"))
	{
		GameInterface.(_tabName).tr5.td3.color = argb(255, 255, 255, 255);
	}
	else
	{
		GameInterface.(_tabName).tr5.td3.color = argb(255, 128, 200, 225);
	}

	//green text
	float fMaxSpeedZ;
	if (CheckAttribute(_chr, "Ship.MaxSpeedZ")) { fMaxSpeedZ = stf(_chr.Ship.MaxSpeedZ); }
	else { fMaxSpeedZ = 1.0; }
	sAdditionalInfo = XI_ConvertString("AgainstWind_desc") + ": ";
	if (Sea_ApplyMaxSpeedZ(_chr, 1.0) != 0) {
		sAdditionalInfo += "\n 180 (" + XI_ConvertString("straight_wind")
		+ ")      - "
		+ FloatToString(Sea_ApplyMaxSpeedZ(_chr, 1.0) * 100 / fMaxSpeedZ, 1)
		+ "%\n 124 (" + XI_ConvertString("backstay_wind")
		+ ")          - "
		+ FloatToString(Sea_ApplyMaxSpeedZ(_chr, 0.6) * 100 / fMaxSpeedZ, 1)
		+ "%\n 90 (" + XI_ConvertString("halfwind")
		+ ")               - "
		+ FloatToString(Sea_ApplyMaxSpeedZ(_chr, 0.0) * 100 / fMaxSpeedZ, 1)
		+ "%\n 67 (" + XI_ConvertString("AgainstWind_easy")
		+ ")    - "
		+ FloatToString(Sea_ApplyMaxSpeedZ(_chr, -0.4999999) * 100 / fMaxSpeedZ, 1)
		+ "%\n 45 (" + XI_ConvertString("AgainstWind")
		+ ")       - "
		+ FloatToString(Sea_ApplyMaxSpeedZ(_chr, -0.6999999) * 100 / fMaxSpeedZ, 1)
		+ "%\n 22 (" + XI_ConvertString("AgainstWind_hard")
		+ ")     - "
		+ FloatToString(Sea_ApplyMaxSpeedZ(_chr, -0.8599999) * 100 / fMaxSpeedZ, 1) + "%";
	}
	else {//по идее ошибка
		sAdditionalInfo = "Выйдите в море для получения точных данных о бейдевинде корабля.";
	}

	GameInterface.(_tabName).tr6.UserData.ID = "AgainstWind";
	GameInterface.(_tabName).tr6.UserData.ID1 = "";
	GameInterface.(_tabName).tr6.UserData.ID2 = sAdditionalInfo;
	GameInterface.(_tabName).tr6.td1.icon.group = "ICONS_CHAR";
	GameInterface.(_tabName).tr6.td1.icon.image = "AgainstWind";
	GameInterface.(_tabName).tr6.td2.str = XI_ConvertString("AgainstWind");
	GameInterface.(_tabName).tr6.td3.str = FloatToString(stf(refBaseShip.WindAgainstSpeed), 2);
	if (!CheckAttribute(&RealShips[iShip], "Tuning.WindAgainstSpeed"))
	{
		GameInterface.(_tabName).tr6.td3.color = argb(255, 255, 255, 255);
	}
	else
	{
		GameInterface.(_tabName).tr6.td3.color = argb(255, 128, 200, 225);
	}

	//green text
	RecalculateCargoLoad(_chr);
	float fCargoPct = (stf(GetCargoLoad(_chr)) / stf(GetCargoMaxSpace(_chr))) * 100.0;
	sAdditionalInfo = XI_ConvertString("Occupied") + ": "
	+ FloatToString(fCargoPct, 1) + "%";

	if (!ShipTurnRateBonusFromWeight(_chr, refBaseShip))
	{
		sAdditionalInfo += "\n" + XI_ConvertString("Penalty") + ": -"
		+ FloatToString(100.0 - ShipSpeedBonusFromWeight(_chr, refBaseShip) * 100, 1) + "% "
		+ XI_ConvertString("crew_speed");

		if (!ShipTurnRateBonusFromWeight(_chr, refBaseShip))
		{
			sAdditionalInfo += XI_ConvertString("and") + " -"
			+ FloatToString(100.0 - ShipTurnRateBonusFromWeight(_chr, refBaseShip) * 100, 1) + "% "
			+ XI_ConvertString("crew_maneuver");
		}
	}
	else {
		sAdditionalInfo += "\n\n" + XI_ConvertString("lightweight_descr") + "\n" + XI_ConvertString("lightweight") + " "
		+ FloatToString(100 * 0.7 - fCargoPct, 1) + "% "
		+ XI_ConvertString("to_speed") + " "
		+ FloatToString(100 * 0.5 - fCargoPct, 1) + "% "
		+ XI_ConvertString("to_maneuver");
	}

	GameInterface.(_tabName).tr7.UserData.ID = "Capacity";
	GameInterface.(_tabName).tr7.UserData.ID1 = "";
	GameInterface.(_tabName).tr7.UserData.ID2 = sAdditionalInfo;
	GameInterface.(_tabName).tr7.td1.icon.group = "ICONS_CHAR";
	GameInterface.(_tabName).tr7.td1.icon.image = "Capacity";
	GameInterface.(_tabName).tr7.td2.str = XI_ConvertString("Capacity");
	GameInterface.(_tabName).tr7.td3.str = GetCargoLoad(_chr) + " / " + GetCargoMaxSpace(_chr);
	GameInterface.(_tabName).tr7.tocompare = GetCargoMaxSpace(_chr);
	if (!CheckAttribute(&RealShips[iShip], "Tuning.Capacity"))
	{
		GameInterface.(_tabName).tr7.td3.color = argb(255, 255, 255, 255);
	}
	else
	{
		GameInterface.(_tabName).tr7.td3.color = argb(255, 128, 200, 225);
	}

	//red text
	sText2 = XI_ConvertString("overload_crew") + "\n"
	+ XI_ConvertString("max_crew") + ": "
	+ GetMaxCrewQuantity(_chr) + " "
	+ XI_ConvertString("people");

	//green text

	if (sti(_chr.Ship.Crew.quantity)) {
		float fPowerRatio = GetCrewBonusRank(_chr) / stf(_chr.chr_ai.hp_max) + 1;
		sAdditionalInfo = XI_ConvertString("crew_opt_descr") + "\n "
		+ XI_ConvertString("crew_opt") + ": "
		+ MakeInt(MakeFloat(stf(refBaseShip.OptCrew)) / ShipTurnRateBonusFromPeopleUncapped(_chr, stf(refBaseShip.MaxCrew), refBaseShip)) + " "
		+ XI_ConvertString("people")
		+ "\n\n"
		+ XI_ConvertString("crew_sailors") + ": "
		+ FloatToString(ShipSpeedBonusFromPeople(_chr, refBaseShip) * 100, 1) + "% "
		+ XI_ConvertString("crew_speed") + ", "
		+ FloatToString(ShipTurnRateBonusFromPeople(_chr, refBaseShip) * 100.0, 1) + "% "
		+ XI_ConvertString("crew_maneuver") + ", "
		+ FloatToString(stf(_chr.Ship.RollSpeed) * 100, 1) + "% "
		+ XI_ConvertString("crew_roll_speed") + "\n"
		+ XI_ConvertString("crew_cannons_speed") + " "
		+ FloatToString(100.0 / Cannon_GetRechagreTimeBonusFromPeople(_chr, 1.0), 1) + "% "
		+ XI_ConvertString("on_reload_speed") + " "
		+ FloatToString(100.0 / ShipCannonFireTimeBonusFromPeople(_chr, refBaseShip), 1) + "% "
		+ XI_ConvertString("salvo_speed") + "\n"
		+ XI_ConvertString("crew_HP") + " "
		+ FloatToString(100 * fPowerRatio, 1) + "% ("
		+ XI_ConvertString("crew_HP_of_captain") + ")";
	}
	else {
		sAdditionalInfo = XI_ConvertString("no_crew");
	}
	// LEO: раненые — полный прогноз в подсказку ПКМ строки Команда (активные бусты + лекарства + дни до полного выздоровления)
	sAdditionalInfo = sAdditionalInfo + Wounded_ForecastText(_chr);


	GameInterface.(_tabName).tr8.UserData.ID = "Crew";
	GameInterface.(_tabName).tr8.UserData.ID1 = sText2;
	GameInterface.(_tabName).tr8.UserData.ID2 = sAdditionalInfo;
	GameInterface.(_tabName).tr8.td1.icon.group = "ICONS_CHAR";
	GameInterface.(_tabName).tr8.td1.icon.image = "Crew";
	GameInterface.(_tabName).tr8.td2.str = XI_ConvertString("Crew");
	GameInterface.(_tabName).tr8.td3.str = GetCrewQuantity(_chr) + " : " + sti(refBaseShip.MinCrew) + " / " + sti(refBaseShip.OptCrew);
	GameInterface.(_tabName).tr8.tocompare = sti(refBaseShip.OptCrew);
	if (!CheckAttribute(&RealShips[iShip], "Tuning.MaxCrew"))
	{
		GameInterface.(_tabName).tr8.td3.color = argb(255, 255, 255, 255);
	}
	else
	{
		GameInterface.(_tabName).tr8.td3.color = argb(255, 128, 200, 225);
	}


	//green text
	if (sti(_chr.Ship.Cannons.type) != CANNON_TYPE_NONECANNON || pchar.ship.type != refBaseShip.index) {//работает если есть хотя бы одна
		_chr.TmpSkillRecall = 0;//для Ship_UpdateTmpSkills
		Ship_UpdateTmpSkills(_chr);//иначе tmpskill не работает
		int iCannonsL = GetBortIntactCannonsNum(_chr, "cannonl", sti(refBaseShip.cannonl));
		int iCannonsR = GetBortIntactCannonsNum(_chr, "cannonr", sti(refBaseShip.cannonr));
		float fCannonDamageMultiply = stf(rCannon.DamageMultiply);
		//прицениваемся к новому кораблю, представляя что у него стоят лучшие пушки
		if (iCannonsL <= 0 && iCannonsR <= 0)
		{
			iCannonsL = sti(refBaseShip.cannonl);
			iCannonsR = iCannonsL;
			//берем урон типа орудия по макс калибру просматриваемого корабля
		}
		if (fCannonDamageMultiply <= 0)
		{
			fCannonDamageMultiply = FloatToString(Cannon[GetCannonByTypeAndCaliber("cannon", sti(refBaseShip.maxcaliber))].DamageMultiply, 1);
		}
		float fCannonFireTime = 1.2 * CalcShipCannonFireTimeMult(_chr, refBaseShip) * ShipCannonFireTimeBonusFromBaseShip(_chr, refBaseShip);

		sAdditionalInfo = XI_ConvertString("bort_fire_time") + " ";

		if (iCannonsR != iCannonsL)
		{
			sAdditionalInfo += XI_ConvertString("cannonl") + " - ";
		}
		sAdditionalInfo += FloatToString(fCannonFireTime * iCannonsL, 1) + " с.";
		if (iCannonsR != iCannonsL) {
			sAdditionalInfo += ", " + XI_ConvertString("cannonr") + " - "
			+ FloatToString(fCannonFireTime * iCannonsR, 1) + " с.";
		}

		sAdditionalInfo += "\n " + XI_ConvertString("bort_damage") + " ";
		if (iCannonsR != iCannonsL)
		{
			sAdditionalInfo += "\n" + XI_ConvertString("cannonl") + " - ";
		}
		sAdditionalInfo += FloatToString(iCannonsL * stf(rCannon.DamageMultiply), 1);
		if (pchar.ship.type == refBaseShip.index) {
			sAdditionalInfo += " (" + FloatToString((iCannonsL * stf(rCannon.DamageMultiply)) / (Cannon_GetRechargeTimeValue(_chr) + fCannonFireTime * iCannonsL), 1) + " "
			+ XI_ConvertString("dps") + ")";
		}
		if (iCannonsR != iCannonsL)
		{
			sAdditionalInfo += "\n" + XI_ConvertString("cannonr") + " - "
			+ FloatToString(iCannonsR * stf(rCannon.DamageMultiply), 1) + " ("
			+ FloatToString((iCannonsR * stf(rCannon.DamageMultiply)) / (Cannon_GetRechargeTimeValue(_chr) + fCannonFireTime * iCannonsR), 1) + " "
			+ XI_ConvertString("dps") + " )";
		}

		float fSalvoWidth = 0;
		if (CheckAttribute(_chr, "Ship.MaxCannonDist") && stf(_chr.Ship.MaxCannonDist) > 0) {
			fSalvoWidth = _chr.Ship.MaxCannonDist;
		}
		else {
			// ширина залпа от длины корпуса; у не плававшего корабля габарит даст фолбэк по .gm
			fSalvoWidth = 0.22 * GetShipBoxSize(_chr, 2);
		}
		string sFiredBort = "";
		if (CheckAttribute(_chr, "Ship.FiredBort")) sFiredBort = _chr.Ship.FiredBort;

		sAdditionalInfo += "\n" + XI_ConvertString("Flames_damage") + " "
		+ MakeInt(SHIP_DEFAULT_FIRE_TIME * Ship_CalculateFireDamage(rCannon, false)) + "-"
		+ MakeInt(SHIP_DEFAULT_FIRE_TIME * (1.0 + Ship_CalculateFireDamage(rCannon, false))) + " ("
		+ FloatToString(Ship_CalculateFireDamage(rCannon, false), 1) + "-"
		+ FloatToString(Ship_CalculateFireDamage(rCannon, false) + 1.0, 1) + " "
		+ XI_ConvertString("dps") + " * "
		+ SHIP_DEFAULT_FIRE_TIME + " "
		+ XI_ConvertString("sec.") + ")\n\n "
		+ XI_ConvertString("Aim_error") + " ("
		+ XI_ConvertString("last_salvo") + " - "
		+ XI_ConvertString(sFiredBort) + "):\n "
		+ XI_ConvertString("OnShip") + " 200 "
		+ XI_ConvertString("m") + " - "
		+ XI_ConvertString("to") + " "
		+ FloatToString(Ship_GetBortFireDeltaFromDistance(_chr, 200.0, 0.0), 1) + " "
		+ XI_ConvertString("m") + "; "
		+ XI_ConvertString("OnShip") + " "
		+ GetShootDistance(_chr, "ball") + " "
		+ XI_ConvertString("m") + " - "
		+ XI_ConvertString("to") + " "
		+ FloatToString(Ship_GetBortFireDeltaFromDistance(_chr, MakeFloat(GetShootDistance(_chr, "ball")), 0.0), 1) + " "
		+ XI_ConvertString("m");
		if (sFiredBort == "cannonr" || sFiredBort == "cannonl") {
			sAdditionalInfo += "\n" 
			+ XI_ConvertString("shooting_spread") + ": "
			+ XI_ConvertString("salvo_width") + " - "
			+ FloatToString(fSalvoWidth, 1) + " "
			+ XI_ConvertString("m");
		}
		if (CheckAttribute(_chr, "ship.Cannons.speedv0")) {
			sAdditionalInfo += "\n"
			+ XI_ConvertString("Ball_speed") + " (" 
			+ XI_ConvertString(Goods[sti(_chr.Ship.Cannons.Charge.Type)].Name) + "): "
			+ 3 * sti(_chr.ship.Cannons.speedv0) + " "
			+ XI_ConvertString("mps");
		}
	}
	//green text	
	string sBort, sBortDesc;

	sAdditionalInfo += "\n\n" + XI_ConvertString("cannons_cond");
	for (int j = 0; j < 4; j++) {
		switch (j) {
			case 0:
				sBort = "cannonl";
				sBortDesc = "\n" + XI_ConvertString("cannonl") + ": ";
				break;
			case 1:
				sBort = "cannonr";
				sBortDesc = "\n" + XI_ConvertString("cannonr") + ": ";
				break;
			case 2:
				sBort = "cannonf";
				sBortDesc = "\n" + XI_ConvertString("cannonf") + ": ";
				break;
			case 3:
				sBort = "cannonb";
				sBortDesc = "\n" + XI_ConvertString("cannonb") + ": ";
				break;
		}
		int iNumCannonsOnBort = sti(refBaseShip.(sBort));
		if (GetBortIntactCannonsNum(_chr, sBort, iNumCannonsOnBort) > 0) {
			sAdditionalInfo += sBortDesc + " "
			+ XI_ConvertString("average") + ": "
			+ MakeInt(100 * (1 - GetBortConditionAvg(_chr, sBort, iNumCannonsOnBort))) + "%, "
			+ XI_ConvertString("min") + ": "
			+ MakeInt(100 * (1 - GetBortConditionMin(_chr, sBort, iNumCannonsOnBort))) + "%";
		}
	}

	//нет орудий и НЕ примеряемся к другому кораблю
	if (!GetCannonsNum(_chr) && pchar.ship.type == refBaseShip.index) {
		sText2 = XI_ConvertString("nocannons");
		sAdditionalInfo = "";
	}

	GameInterface.(_tabName).tr9.UserData.ID = "sCannons";
	GameInterface.(_tabName).tr9.UserData.ID1 = "";
	GameInterface.(_tabName).tr9.UserData.ID2 = sAdditionalInfo;
	GameInterface.(_tabName).tr9.td1.icon.group = "ICONS_CHAR";
	GameInterface.(_tabName).tr9.td1.icon.image = "Caliber";
	GameInterface.(_tabName).tr9.td2.str = XI_ConvertString("sCannons");
	GameInterface.(_tabName).tr9.td3.str = XI_ConvertString("caliber" + refBaseShip.MaxCaliber) + " / " + sti(refBaseShip.CannonsQuantity);
	GameInterface.(_tabName).tr9.tocompare = refBaseShip.MaxCaliber;
	if (!CheckAttribute(&RealShips[iShip], "Tuning.Cannon"))
	{
		GameInterface.(_tabName).tr9.td3.color = argb(255, 255, 255, 255);
	}
	else
	{
		GameInterface.(_tabName).tr9.td3.color = argb(255, 128, 200, 225);
	}

	//red text
	if (sti(_chr.Ship.Cannons.Type) != CANNON_TYPE_NONECANNON && GetCannonsNum(_chr)) {
		sText2 = XI_ConvertString(GetCannonType(sti(_chr.Ship.Cannons.Type))) + " "
		+ XI_ConvertString("caliber" + GetCannonCaliber(sti(_chr.Ship.Cannons.Type))) + "\n"
		+ XI_ConvertString("Fire range2") + ": "
		+ sti(rCannon.FireRange) + " "
		+ XI_ConvertString("m") + ", "
		+ XI_ConvertString("dmg_x")
		+ FloatToString(stf(rCannon.DamageMultiply), 1) + "\n"
		+ XI_ConvertString("Reload time") + ": "
		+ sti(GetCannonReloadTime(rCannon)) + " "
		+ XI_ConvertString("sec.") + ", "
		+ XI_ConvertString("weight") + ": "
		+ sti(rCannon.Weight) + " "
		+ XI_ConvertString("cwt");
	}
	//green text
	//копия предыдущих строк + подсказка
	sAdditionalInfo += "\n\n" + XI_ConvertString("check_cannons");

	GameInterface.(_tabName).tr10.UserData.ID = "CannonType";
	GameInterface.(_tabName).tr10.UserData.ID1 = sText2;
	GameInterface.(_tabName).tr10.UserData.ID2 = sAdditionalInfo;
	GameInterface.(_tabName).tr10.td1.icon.group = "ICONS_CHAR";
	GameInterface.(_tabName).tr10.td1.icon.image = "Cannons";
	GameInterface.(_tabName).tr10.td2.str = XI_ConvertString(GetCannonType(sti(_chr.Ship.Cannons.Type)) + "s2");
	GameInterface.(_tabName).tr10.tocompare = GetCannonsNum(_chr);
	if (sti(_chr.Ship.Cannons.Type) != CANNON_TYPE_NONECANNON)
	{
		if (GetCannonsNum(_chr) > 0)
		{
			GameInterface.(_tabName).tr10.td3.str = XI_ConvertString("caliber" + GetCannonCaliber(sti(_chr.Ship.Cannons.Type))) + " / " + GetCannonsNum(_chr) + " (" + MakeInt(100 * (1 - GetAllCannonsConditionAvg(_chr))) + "%)";
		}
		else
		{
			GameInterface.(_tabName).tr10.td3.str = GetCannonsNum(_chr);
		}
	}
	else
	{
		GameInterface.(_tabName).tr10.td3.str = "";
	}
	// прорисовка
	Table_UpdateWindow(_tabName);
}

void SetShipQualityTable(ref _chr, string _bar1, string _bar7, string _bar2, string _bar3, string _bar4, string _bar5, string _bar6)
{
	int iShip = sti(_chr.ship.type);
	float fHP = 100;
	float fMast = 100;
	float fSpeedRate = 100;
	float fTurnRate = 100;
	float fWAS = 100;
	float fCapacity = 100;
	float fCrew = 100;
	ref refShip = GetRealShip(iShip);

	ref rBaseShip = GetShipByType(sti(refShip.BaseType));
	float fTemp;

	fTemp = stf(refShip.Capacity);
	if (CheckAttribute(refShip, "Untuned.Capacity")) fTemp = refShip.Untuned.Capacity;
	fCapacity = 50 * (fTemp - stf(rBaseShip.Capacity)) / (stf(rBaseShip.Capacity) * SHIP_STAT_RANGE_SPECIAL) + 50;

	fTemp = stf(refShip.HP);
	if (CheckAttribute(refShip, "Untuned.HP")) fTemp = refShip.Untuned.HP;
	fHP = 50 * (fTemp - stf(rBaseShip.HP)) / (stf(rBaseShip.HP) * SHIP_STAT_RANGE_SPECIAL) + 50;

	fTemp = stf(refShip.SpeedRate);
	if (CheckAttribute(refShip, "Untuned.SpeedRate")) fTemp = refShip.Untuned.SpeedRate;
	fSpeedRate = 50 * (fTemp - stf(rBaseShip.SpeedRate)) / (stf(rBaseShip.SpeedRate) * SHIP_STAT_RANGE_SPECIAL) + 50;

	fTemp = stf(refShip.TurnRate);
	if (CheckAttribute(refShip, "Untuned.TurnRate")) fTemp = refShip.Untuned.TurnRate;
	fTurnRate = 50 * (fTemp - stf(rBaseShip.TurnRate)) / (stf(rBaseShip.TurnRate) * SHIP_STAT_RANGE_SPECIAL) + 50;

	fTemp = stf(refShip.MastMultiplier);
	if (CheckAttribute(refShip, "Untuned.MastMultiplier")) fTemp = refShip.Untuned.MastMultiplier;
	fMast = 50 * (stf(rBaseShip.MastMultiplier) - fTemp) / 0.3 + 50;

	fTemp = stf(refShip.WindAgainstSpeed);
	if (CheckAttribute(refShip, "Untuned.WindAgainstSpeed")) fTemp = refShip.Untuned.WindAgainstSpeed;
	fWAS = 50 * (fTemp - stf(rBaseShip.WindAgainstSpeed)) / (stf(rBaseShip.WindAgainstSpeed) * SHIP_STAT_RANGE_SPECIAL) + 50;

	fTemp = stf(refShip.OptCrew); 	//тюнинг не влияет на оптимальную
	fCrew = 50 * (fTemp - stf(rBaseShip.OptCrew)) / (stf(rBaseShip.OptCrew) * SHIP_STAT_RANGE_SPECIAL) + 50;

	GameInterface.StatusLine.(_bar1).Max   = 100;
	GameInterface.StatusLine.(_bar1).Min   = 0;
	GameInterface.StatusLine.(_bar1).Value = fHP;
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, _bar1,0);

	GameInterface.StatusLine.(_bar2).Max   = 100;
	GameInterface.StatusLine.(_bar2).Min   = 0;
	GameInterface.StatusLine.(_bar2).Value = fSpeedRate;
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, _bar2,0);

	GameInterface.StatusLine.(_bar3).Max   = 100;
	GameInterface.StatusLine.(_bar3).Min   = 0;
	GameInterface.StatusLine.(_bar3).Value = fTurnRate;
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, _bar3,0);

	GameInterface.StatusLine.(_bar4).Max   = 100;
	GameInterface.StatusLine.(_bar4).Min   = 0;
	GameInterface.StatusLine.(_bar4).Value = fWAS;
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, _bar4,0);

	GameInterface.StatusLine.(_bar5).Max   = 100;
	GameInterface.StatusLine.(_bar5).Min   = 0;
	GameInterface.StatusLine.(_bar5).Value = fCapacity;
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, _bar5,0);

	GameInterface.StatusLine.(_bar6).Max   = 100;
	GameInterface.StatusLine.(_bar6).Min   = 0;
	GameInterface.StatusLine.(_bar6).Value = fCrew;
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, _bar6,0);

	GameInterface.StatusLine.(_bar7).Max   = 100;
	GameInterface.StatusLine.(_bar7).Min   = 0;
	GameInterface.StatusLine.(_bar7).Value = fMast;
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, _bar7,0);
}

void SetFoodShipInfo(ref chr, string _textName)
{
	int iColor, iFood;
	string sText;

	SetFormatedText(_textName, "");
	if (sti(chr.ship.type) != SHIP_NOTUSED)
	{
		sText = GetConvertStrWithReplace("Variable_interface_utils_4", "Interface.txt", "#space#", " ");
		iFood = CalculateShipFood(chr);
		sText = sText + FindRussianDaysString(iFood);
		SetFormatedText(_textName, sText);
		if(iFood >= 5)
		{
			iColor = argb(255,255,255,192);
		}
		if(iFood > 10)
		{
			iColor = argb(255,192,255,192);
		}
		if(iFood < 5)
		{
			iColor = argb(255,255,192,192);
		}
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE, _textName, 8,-1,iColor);
	}
}

void SetSquadronFoodShipInfo(ref chr, string _textName)
{
	int iColor, iFood;
	string sText;

	SetFormatedText(_textName, "");
	if (sti(chr.ship.type) != SHIP_NOTUSED)
	{
		sText = GetConvertStrWithReplace("Variable_interface_utils_6", "Interface.txt", "#space#", " ") + "\n" + GetConvertStrWithReplace("Variable_interface_utils_6.1", "Interface.txt", "#space#", " ")+ "\n" + GetConvertStrWithReplace("Variable_interface_utils_6.2", "Interface.txt", "#space#", " ");
		iFood = CalculateShipFood(chr);
		sText = sText + " " + iFood+" / "+FindRussianDaysString(CalculateFood());
		SetFormatedText(_textName, sText);
		if(iFood >= 5)
		{
			iColor = argb(255,255,255,192);
		}
		if(iFood > 10)
		{
			iColor = argb(255,192,255,192);
		}
		if(iFood < 5)
		{
			iColor = argb(255,255,192,192);
		}
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE, _textName, 8,-1,iColor);
	}
}

// Warship 11.07.09 Вывести в текстовое поле инфу о количестве дней, на сколько хватит рому на судне
void SetRumShipInfo(ref _character, String _node)
{
	int color, rum;
	String text;

	SetFormatedText(_node, "");

	if(sti(_character.ship.type) != SHIP_NOTUSED)
	{
		text = GetConvertStrWithReplace("Variable_interface_utils_5", "Interface.txt", "#space#", " ");
		rum = CalculateShipRum(_character);
		text = text + " " + FindRussianDaysString(rum);
		SetFormatedText(_node, text);

		if(rum < 3)
		{
			color = argb(255, 255, 192, 192);
		}

		if(rum >= 3)
		{
			color = argb(255, 255, 255, 192);
		}

		if(rum >= 10)
		{
			color = argb(255, 192, 255, 192);
		}

		SendMessage(&GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, _node, 8, -1, color);
	}
}

int GetTradeItemPrice(string sItemID, int tradeType)
{
	int itmIdx = Items_FindItemIdx(sItemID);
	if (itmIdx < 0 || itmIdx > ITEMS_QUANTITY) return 0;

	int itmprice = GetItemPrice(sItemID);
	if (itmprice < 0) itmprice = 0;

	float skillDelta = GetSummonSkillFromNameToOld(pchar, SKILL_COMMERCE);
	float skillModify;
	if(tradeType == PRICE_TYPE_BUY)
	{
		skillModify = 1.45 - skillDelta*0.019;
		if(CheckOfficersPerk(pchar,"Trader")) { skillModify -= 0.05; }
		if(CheckOfficersPerk(pchar,"AdvancedCommerce"))	{ skillModify -= 0.2; }
		else
		{
			if(CheckOfficersPerk(pchar,"BasicCommerce"))	{ skillModify -= 0.1; }
		}
		if(CheckAttribute(&Items[itmIdx],"groupID"))
		{
			if(Items[itmIdx].groupID == BLADE_ITEM_TYPE || Items[itmIdx].groupID == GUN_ITEM_TYPE) skillModify *= 10.0;//меняем после всех перков, иначе скидка от перков непропорциональная
		}
	}
	else
	{
		skillModify = 0.70 + skillDelta*0.019;
		if (CheckOfficersPerk(pchar,"AdvancedCommerce"))	skillModify += 0.05;
		if (CheckOfficersPerk(pchar,"Trader")) { skillModify += 0.05; }
	}

	return makeint(makefloat(itmprice)*skillModify);
}

//--> mod tablesort		Qwerry, St.
//идея необходимости сортировки в таблицах:	DiamiR
//версия для OS		с визуальной индикацией и автозапоминанием и автоопределением параметров		на MS использовалось другое сравнение строк 	strcmp()
void SortTable(string sControl, int iColumn)
{
	int m, n;
	bool bIsString = false;
	string sColumn = "td" + iColumn;
	if (checkattribute(&GameInterface, sControl + ".hr." + sColumn + ".sorttype"))
		{if(GameInterface.(sControl).hr.(sColumn).sorttype == "string") bIsString = true;}
	else return; //если атрибут .sorttype не поставили, значит сортировать не нужно
//TO DO		убрать это деление на строки, автоматически определять - текст или число сортируем

	bool bAsc = true;
	if (checkattribute(&GameInterface, sControl + ".hr." + sColumn + ".sortdir"))
		{if(GameInterface.(sControl).hr.(sColumn).sortdir == "dec") bAsc = false;}
	//атрибут .sortdir = "dec" указываем, если нужно, чтобы при первом клике сортировалось в обратном направлении

	aref aRows;
	makearef(aRows, GameInterface.(sControl));
	//в коде заполнения таблиц не должно быть пустых строк - проверить везде, чтобы все фильтрации выполнялись до первой записи в строчку
	int iLinesCount = 0;
	for (n = 0; n < GetAttributesNum(aRows); n++)
	{
		if (HasSubStr(GetAttributeName(GetAttributeN(aRows, n)), "tr")) iLinesCount++;
	}

	if (iLinesCount < 3) return;//две строки нет смысла сортировать

	if (checkattribute(&GameInterface, sControl + ".hr.sortedColumn"))//таблица уже сортировалось ранее
	{
		if (iColumn == sti(GameInterface.(sControl).hr.sortedColumn)) //повторный клик по той же колонке - просто реверс
		{
			ReverseTable(sControl, iLinesCount);
			if (GameInterface.(sControl).hr.(sColumn).icon.image  == "downbutton") bAsc = true; else bAsc = false;
			DrawSortDir(sControl, iColumn, bAsc);
			return;
		}
		string sTemp = sControl + ".hr.td" + GameInterface.(sControl).hr.sortedColumn + ".icon";
		DeleteAttribute(&GameInterface, sTemp); // стираем стрелку сортировки с предыдушей колонки
	}


	string sRow, sNewRow, sSortedString, sLast;
	aref aCurRow, aNextRow, aRow;

	if (sti(GameInterface.(sControl).select))
	{
		sRow = "tr" + GameInterface.(sControl).select;
		sLast = GameInterface.(sControl).(sRow).index; // запоминаем номер выбранной строки, чтобы выделить её же после сортировки
		// TO DO везде нужно заполнять индекс
	}

	if (bIsString) // строки
	{
		object oSorted, oTableCopy;
		string sAdd;
		int iAdd;
		for (n = 0; n < iLinesCount; n++)
		{
			sRow = "tr" + (n + 1);
			sSortedString = GameInterface.(sControl).(sRow).(sColumn).str;
			sSortedString = strreplace(sSortedString, ".", ","); // фикс краша из-за точек	подменяем их на запятые
			sAdd = ""; iAdd = 0;
			while (checkattribute(oSorted, sSortedString + sAdd)) {iAdd++; sAdd = DigitsToString(iAdd, 3);}//фикс Краша из-за дубликатов
			// до 1000 одинаковых значений в одной таблице - дальше краш, увеличьте
			sSortedString += sAdd;
			oSorted.(sSortedString) = sRow;
		}
		aref rootItems;
		makearef(rootItems, oSorted);
		sort(rootItems);
		aref aTable, aTableCopy;
		makearef(aTable, GameInterface.(sControl));
		makearef(aTableCopy, oTableCopy.(sControl));
		CopyAttributes(aTableCopy, aTable);
		for (n = 0; n < iLinesCount; n++)
		{
			sRow = "tr" + (n + 1);
			makearef(aCurRow, aTable.(sRow));
			sNewRow = GetAttributeValue(GetAttributeN(rootItems, n));
			makearef(aNextRow, aTableCopy.(sNewRow));
			CopyAttributes(aCurRow, aNextRow);
		}
		if (!bAsc) ReverseTable(sControl, iLinesCount); //нужна было сортировка по убыванию
	}
	else // числа
	{
		string sCurRow, sNextRow;
		sRow = "trx"; // пузырёк
		// TO DO		полный двойной цикл - поискать более оптимальную сортировку
		for (m = 0; m < iLinesCount - 1; m++)
		{
			for (n = 0; n < iLinesCount - 1; n++)
			{
				sCurRow = "tr" + (n + 1);
				sNextRow = "tr" + (n + 2);
				if (bAsc) // по возрастанию
				{
					if (stf(GameInterface.(sControl).(sCurRow).(sColumn).str) > stf(GameInterface.(sControl).(sNextRow).(sColumn).str))
					{
						makearef(aRow, GameInterface.(sControl).(sRow));
						makearef(aCurRow, GameInterface.(sControl).(sCurRow));
						makearef(aNextRow, GameInterface.(sControl).(sNextRow));
						CopyAttributes(aRow, aCurRow);
						CopyAttributes(aCurRow, aNextRow);
						CopyAttributes(aNextRow, aRow);
					}
				}
				else
				{
					if (stf(GameInterface.(sControl).(sCurRow).(sColumn).str) < stf(GameInterface.(sControl).(sNextRow).(sColumn).str))
					{
						makearef(aRow, GameInterface.(sControl).(sRow));
						makearef(aCurRow, GameInterface.(sControl).(sCurRow));
						makearef(aNextRow, GameInterface.(sControl).(sNextRow));
						CopyAttributes(aRow, aCurRow);
						CopyAttributes(aCurRow, aNextRow);
						CopyAttributes(aNextRow, aRow);
					}
				}
			}
		}
		DeleteAttribute(&GameInterface, sControl + "." + sRow); // удаляем пузырёк, будет портить число строк в таблице
	}
	DrawSortDir(sControl, iColumn, bAsc);
	ResetSelectedRow(sControl, sLast, iLinesCount);
}

void DrawSortDir(string sControl, int iColumn, bool bAsc)
{
	string sColumn = "td" + iColumn;
	GameInterface.(sControl).hr.(sColumn).icon.group = "MAIN_ICONS"; // рисуем стрелку направления сортировки
	GameInterface.(sControl).hr.(sColumn).icon.width = 8;
	GameInterface.(sControl).hr.(sColumn).icon.height = 16;
	GameInterface.(sControl).hr.(sColumn).icon.valign = "bottom";
	GameInterface.(sControl).hr.(sColumn).icon.offset = "0,8"; // смещаем на половину высоты ниже границы между заголовком и первой строкой	//Тут проблема - есть же таблицы, где заголовок спрятан - проверить
	if 	(bAsc) 	GameInterface.(sControl).hr.(sColumn).icon.image  = "upbutton";
		else	GameInterface.(sControl).hr.(sColumn).icon.image  = "downbutton"; //при изменении картинки - менять и чуть выше, искать: 	.icon.image  == "
	GameInterface.(sControl).hr.sortedColumn = iColumn;
}

void ReverseTable(string sControl, int iLinesCount)
{
	string sFirstRow, sLastRow;
	string sRow = "trx";
	aref aFirstRow, aLastRow, aRow;
	for (int n = 0; n < iLinesCount / 2; n++)
	{
		sFirstRow = "tr" + (n + 1);
		sLastRow = "tr" + (iLinesCount - n);
		makearef(aRow, GameInterface.(sControl).(sRow));
		makearef(aFirstRow, GameInterface.(sControl).(sFirstRow));
		makearef(aLastRow, GameInterface.(sControl).(sLastRow));
		CopyAttributes(aRow, aFirstRow);
		CopyAttributes(aFirstRow, aLastRow);
		CopyAttributes(aLastRow, aRow);
	}
	if (sti(GameInterface.(sControl).select)) // не меняем, если было 0
	{
		n = iLinesCount + 1 - sti(GameInterface.(sControl).select);
		GameInterface.(sControl).select = n;
	}
	DeleteAttribute(&GameInterface, sControl + "." + sRow); // удаляем пузырёк, будет портить число строк в таблице
}

void ResetSelectedRow(string sControl, string sLastIdx, int iLinesCount)
{
	string sRow;
	if (sti(GameInterface.(sControl).select)) // не меняем, если было 0, т.е. ничего не выделено
	{
		for (int n = 1; n <= iLinesCount; n++)
		{
			sRow = "tr" + n;
			if (GameInterface.(sControl).(sRow).index == sLastIdx)
			{
				GameInterface.(sControl).select = n;
				break;
			}
		}
	}
}
//<-- mod tablesort

void RecursiveCopy(ref arFrom, string sFrom, ref arTo, string sTo) {
	aref to;
	aref from;
	makearef(to, arTo.(sTo));
	makearef(from, arFrom.(sFrom));
	string field;
	for (int i=0; i < GetAttributesNum(from); i++)
	{
		field = GetAttributeName(GetAttributeN(from, i));
		to.(field) = from.(field);
		RecursiveCopy(from, field, to, field);
	}
}

void MoveNodeSpecially(string nodeName, int left, int top, int right, int bottom)
{
	AddNodeToMoveList(nodeName,left,top,right,bottom);
	MoveAllNodesToDirSoftly("right", 1, 1, "open");
	ClearMoveList();
}

// evganat - двигалка интерфейсов

object MoveList;

void AddNodeToMoveList(string nodeName, int left, int top, int right, int bottom)
{
	float offsetX, offsetY;
	GetXYWindowOffset(&offsetX, &offsetY);
	MoveList.(nodeName).left = makeint(offsetX+left);
	MoveList.(nodeName).top = makeint(offsetY+top);
	MoveList.(nodeName).right = makeint(offsetX+right);
	MoveList.(nodeName).bottom = makeint(offsetY+bottom);
}

void DeleteNodeFromMoveList(string nodeName)
{
	if(CheckAttribute(MoveList, nodeName))
		DeleteAttribute(&MoveList, nodeName);
}

void ClearMoveList()
{
	int n = GetAttributesNum(&MoveList);
	aref arNode;
	string nodeName;
	for (int i=0; i<n; i++)
	{
		arNode = GetAttributeN(&MoveList, 0);
		nodeName = GetAttributeName(arNode);
		DeleteAttribute(&MoveList, nodeName);
	}
}

#event_handler("MoveNodeSteady", "MoveNodeToDirSteady");
void MoveNodeToDirSteady()
{
	string nodeName = GetEventData();
	string dir = GetEventData();
	int dist = GetEventData();
	int remain = GetEventData();
	string tag = GetEventData();
	if(remain <= 0)
	{
		PostEvent("MoveFinished", 100, "s", tag);
		return;
	}
	if(!CheckAttribute(MoveList, nodeName))
		return;
	int oldleft = sti(MoveList.(nodeName).left);
	int oldtop = sti(MoveList.(nodeName).top);
	int oldright = sti(MoveList.(nodeName).right);
	int oldbottom = sti(MoveList.(nodeName).bottom);
	int newleft, newright, newtop, newbottom;
	int kleft = 0;
	int kright = 0;
	int ktop = 0;
	int kbottom = 0;
	switch(dir)
	{
		case "up":			ktop = -1;		kbottom = -1;	break;
		case "down":		ktop = 1;		kbottom = 1;	break;
		case "left":		kleft = -1;		kright = -1;	break;
		case "right":		kleft = 1;		kright = 1;		break;
		case "up-left":		ktop = -1;		kbottom = -1;	kleft = -1;		kright = -1;		break;
		case "up-right":	ktop = -1;		kbottom = -1;	kleft = 1;		kright = 1;			break;
		case "down-left":	ktop = 1;		kbottom = 1;	kleft = -1;		kright = -1;		break;
		case "down-right":	ktop = 1;		kbottom = 1;	kleft = 1;		kright = 1;			break;
	}
	if(remain < dist)
		dist = remain;
	newleft = oldleft + kleft*dist;
	newright = oldright + kright*dist;
	newtop = oldtop + ktop*dist;
	newbottom = oldbottom + kbottom*dist;
	SendMessage(&GameInterface, "lsllllll", MSG_INTERFACE_MSG_TO_NODE, nodeName, -1, 4, newleft, newtop, newright, newbottom);
	MoveList.(nodeName).left = newleft;
	MoveList.(nodeName).top = newtop;
	MoveList.(nodeName).right = newright;
	MoveList.(nodeName).bottom = newbottom;

	
	PostEvent("MoveNodeSteady", 20, "sslls", nodeName, dir, dist, remain-dist, tag);

}	

#event_handler("MoveNodeSoftly", "MoveNodeToDirSoftly");
void MoveNodeToDirSoftly()
{
	string nodeName = GetEventData();
	string dir = GetEventData();
	int startdist = GetEventData();
	int remain = GetEventData();
	int total = GetEventData();
	string tag = GetEventData();
	if(remain <= 0)
	{
		ClearMoveList();//почистить лист того, что двигать по достижении нуля
		return;
	}
	if(!CheckAttribute(MoveList, nodeName))
		return;
	
	int oldleft = sti(MoveList.(nodeName).left);
	int oldtop = sti(MoveList.(nodeName).top);
	int oldright = sti(MoveList.(nodeName).right);
	int oldbottom = sti(MoveList.(nodeName).bottom);
	int newleft, newright, newtop, newbottom;
	int kleft = 0;
	int kright = 0;
	int ktop = 0;
	int kbottom = 0;
	switch(dir)
	{
		case "up":			ktop = -1;		kbottom = -1;	break;
		case "down":		ktop = 1;		kbottom = 1;	break;
		case "left":		kleft = -1;		kright = -1;	break;
		case "right":		kleft = 1;		kright = 1;		break;
		case "up-left":		ktop = -1;		kbottom = -1;	kleft = -1;		kright = -1;		break;
		case "up-right":	ktop = -1;		kbottom = -1;	kleft = 1;		kright = 1;			break;
		case "down-left":	ktop = 1;		kbottom = 1;	kleft = -1;		kright = -1;		break;
		case "down-right":	ktop = 1;		kbottom = 1;	kleft = 1;		kright = 1;			break;
	}
	
	int dist = startdist;
	if(remain >= total*0.8)
	{
		dist = round_near((4.2 - (makefloat(remain)/makefloat(total))*4)*startdist);
	}
	else
	{
		if(remain <= total*0.2)
		{
			dist = round_near(startdist*(0.2 + 4*(makefloat(remain)/makefloat(total))));
		}
	}
	
	if(remain < dist)
		dist = remain;	
	
	
	newleft = oldleft + kleft*dist;
	newright = oldright + kright*dist;
	newtop = oldtop + ktop*dist;
	newbottom = oldbottom + kbottom*dist;
	SendMessage(&GameInterface, "lsllllll", MSG_INTERFACE_MSG_TO_NODE, nodeName, -1, 4, newleft, newtop, newright, newbottom);
	MoveList.(nodeName).left = newleft;
	MoveList.(nodeName).top = newtop;
	MoveList.(nodeName).right = newright;
	MoveList.(nodeName).bottom = newbottom;
	
	PostEvent("MoveNodeSoftly", 20, "ssllls", nodeName, dir, startdist, remain-dist, total, tag);
}	

void MoveAllNodesToDirSteady(string dir, int speed, int dist, string tag)
{
	int n = GetAttributesNum(MoveList);
	string nodeName;
	aref arNode;
	for (int i=0; i<n; i++)
	{
		arNode = GetAttributeN(&MoveList, i);
		nodeName = GetAttributeName(arNode);
		Event("MoveNodeSteady", "sslls", nodeName, dir, speed, dist, tag);
	}
}

void MoveAllNodesToDirSoftly(string dir, int speed, int dist, string tag)
{
	int n = GetAttributesNum(MoveList);
	string nodeName;
	aref arNode;
	for (int i=0; i<n; i++)
	{
		arNode = GetAttributeN(&MoveList, i);
		nodeName = GetAttributeName(arNode);
		Event("MoveNodeSoftly", "ssllls", nodeName, dir, speed, dist, dist, tag);
	}
}

// Gregg: габарит корабля по осям. Атрибут ship.boxsize пишет ДВИЖОК и только при постановке корабля
// в морскую сцену (SHIP::Mount), поэтому у ещё не плававших — превью на стапеле верфи, свежекупленный,
// корабль в стоянке — его нет: подсказки читали пустоту, печатали «длина 0 м» и сыпали в error.log
// «missed attribute: boxsize». Фолбэк GetShipModelBox считает ТОТ ЖЕ бокс по .gm-иерархии модели
// (корпус + мачты + реи), читая только заголовки файлов, и кэширует результат в движке.
// iAxis: 0 = ширина по реям, 1 = высота по мачтам, 2 = длина корпуса.
float GetShipBoxSize(ref _chr, int iAxis)
{
	if (CheckAttribute(_chr, "Ship.boxsize"))
	{
		if (iAxis == 0) return stf(_chr.Ship.boxsize.x);
		if (iAxis == 1) return stf(_chr.Ship.boxsize.y);
		return stf(_chr.Ship.boxsize.z);
	}
	if (!CheckAttribute(_chr, "Ship.Type")) return 0.0;
	int iType = sti(_chr.Ship.Type);
	if (iType == SHIP_NOTUSED) return 0.0;
	ref rShip = GetRealShip(iType);
	if (!CheckAttribute(rShip, "name")) return 0.0;
	string sName = rShip.name;
	return GetShipModelBox("Ships/" + sName + "/" + sName, iAxis);
}

string AddTextAboutHull(ref refchar)
{
	string text = "";
	ref refBaseShip;
	refBaseShip = GetRealShip(sti(refchar.ship.type));//получаем ссылку на сам корабль
	if (CheckAttribute(refBaseShip, "QuestShip") && !checkAttribute(refBaseShip,"savesBonus"))
	{
		return XI_ConvertString("Unidentified");
	}
	if (checkAttribute(refBaseShip,"savesBonus")) //проверяем, есть ли атрибут.
	{
		switch ( sti(refBaseShip.savesBonus) )    //в атрибуте текст хранится, так что сначала преводим в число
		{
			case 0:
				text += XI_ConvertString("Refit")+XI_ConvertString("BasTerRefit");
			break;
			case 1:
				text += XI_ConvertString("Refit")+XI_ConvertString("KingstonRefit");
			break;
			case 2:
				text += XI_ConvertString("Refit")+XI_ConvertString("NassauRefit");
			break;
			case 3:
				text += XI_ConvertString("Refit")+XI_ConvertString("MarigoRefit");
			break;
			case 4:
				text += XI_ConvertString("Refit")+XI_ConvertString("HavanaRefit");
			break;
			case 5:
				text += XI_ConvertString("Refit")+XI_ConvertString("TrinidadRefit");
			break;
			case 6:
				text += XI_ConvertString("Refit")+XI_ConvertString("CartahenaRefit");
			break;	
			case 7:
				text += XI_ConvertString("Refit")+XI_ConvertString("AmsterdamRefit");
			break;	
			case 8:
				text += XI_ConvertString("Refit")+XI_ConvertString("ScheepswerfRefit");
			break;	
			case 9:
				text += XI_ConvertString("Refit")+XI_ConvertString("PortsmouthRefit");
			break;	
			case 10:
				text += XI_ConvertString("Refit")+XI_ConvertString("BrestRefit");
			break;	
			case 11:
				text += XI_ConvertString("Refit")+XI_ConvertString("RochesterRefit");
			break;	
			case 12:
				text += XI_ConvertString("Refit")+XI_ConvertString("BostonRefit");
			break;
		}
	}
	//а вот так дописываем нужное в text вывод информации о модификации верфью Qwerry Shanks
	//а вот так дописываем нужное в text вывод информации о материале из которого построен кораблик Qwerry Shanks
	if (checkAttribute(refBaseShip,"savemBonus")) //проверяем, есть ли атрибут.
	{
		switch ( sti(refBaseShip.savemBonus) )    //в атрибуте текст хранится, так что сначала преводим в число
		{
			case 0:
				text += "\n"+XI_ConvertString("HullMat")+XI_ConvertString("Teak");
			break;
			case 1:
				text += "\n"+XI_ConvertString("HullMat")+XI_ConvertString("WhiteOak");
			break;
			case 2:
				text += "\n"+XI_ConvertString("HullMat")+XI_ConvertString("Fir");
			break;
			case 3:
				text += "\n"+XI_ConvertString("HullMat")+XI_ConvertString("Mahogany");
			break;
			case 4:
				text += "\n"+XI_ConvertString("HullMat")+XI_ConvertString("Sabicu");
			break;
			case 5:
				text += "\n"+XI_ConvertString("HullMat")+XI_ConvertString("Cedr");
			break;
			case 6:
				text += "\n"+XI_ConvertString("HullMat")+XI_ConvertString("Larch");
			break;
			case 7:
				text += "\n"+XI_ConvertString("HullMat")+XI_ConvertString("SwampOak");
			break;
		}
	}
	//а вот так дописываем нужное в text вывод информации о модификации верфью Qwerry Shanks
	text += GetShipsBonuses(refBaseShip);
	return text;
}


int RoundSignedPercent(float fValue)
{
	if (fValue < 0.0) return 0 - makeint(0.5 - fValue);
	return makeint(fValue + 0.5);
}

string GetShipsBonuses(ref rRealShip)
{
	float bonusHP = 0.0;
    float bonusSpeed = 0.0;
    float bonusTurnRate = 0.0;
    float bonusWindAgainstSpeed = 0.0;
    float bonusCapacity = 0.0;
    float bonusOptCrew = 0.0;

	// Gregg: sti() от отсутствующего атрибута = RUNTIME ERROR + 0, то есть кораблю без
	// верфи/материала подставлялся case 0 (Бас-Тер + тик). Ловилось на превью-корабле
	// стапеля: FillOrderShip зовёт CreateBaseShip, а savesBonus пишется только в DoBuyShip.
	if (!CheckAttribute(rRealShip, "savesBonus") && !CheckAttribute(rRealShip, "savemBonus")) return "";

	if (CheckAttribute(rRealShip, "savesBonus"))
	{
	switch (sti(rRealShip.savesBonus))
	{
		case 0:
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS0;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS12;
			bonusCapacity            -= SHIP_STAT_RANGE_GENERATION_BONUS08;
			bonusOptCrew             -= SHIP_STAT_RANGE_GENERATION_BONUS04;
			break; // Basster refit
		case 1: 
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS06;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS06;				
			bonusWindAgainstSpeed    -= SHIP_STAT_RANGE_GENERATION_BONUS07;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusOptCrew             -= SHIP_STAT_RANGE_GENERATION_BONUS05;
			break; // Kingstown refit
		case 2:  
			bonusHP		     		 -= SHIP_STAT_RANGE_GENERATION_BONUS06;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS07;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS0;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            -= SHIP_STAT_RANGE_GENERATION_BONUS08;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS07;
			break; // Nassau refit
		case 3:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusTurnRate     		 -= SHIP_STAT_RANGE_GENERATION_BONUS10;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS08;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS08;
			break; // Marigo refit
		case 4:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusSpeed	      		 -= SHIP_STAT_RANGE_GENERATION_BONUS03;
			bonusTurnRate     		 -= SHIP_STAT_RANGE_GENERATION_BONUS04;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS10;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS06;
			break; // Gavana refit
		case 5:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS02;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS02;
			bonusTurnRate     		 -= SHIP_STAT_RANGE_GENERATION_BONUS09;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS02;
			break; // Trinidad refit
		case 6:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS10;
			bonusSpeed	      		 -= SHIP_STAT_RANGE_GENERATION_BONUS08;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS08;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            -= SHIP_STAT_RANGE_GENERATION_BONUS07;
			bonusOptCrew             -= SHIP_STAT_RANGE_GENERATION_BONUS05;
			break; // Cartagena refit
		case 7:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS03;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS03;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS10;
			bonusOptCrew             -= SHIP_STAT_RANGE_GENERATION_BONUS10;
			break; // Amsterdam refit
		case 8:  
			bonusHP		     		 -= SHIP_STAT_RANGE_GENERATION_BONUS06;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS04;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS08;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            -= SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS04;
			break; // Scheepswerf refit
		case 9:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS04;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusTurnRate     		 -= SHIP_STAT_RANGE_GENERATION_BONUS08;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS07;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS0;
			break; // Portsmouth refit
		case 10:  
			bonusHP		     		 -= SHIP_STAT_RANGE_GENERATION_BONUS04;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS07;
			bonusTurnRate     		 -= SHIP_STAT_RANGE_GENERATION_BONUS0;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS05;
			break; // Brest refit
		case 11:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS03;
			bonusSpeed	      		 -= SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS10;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS0;
			break; // Rochester refit
		case 12:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusTurnRate     		 -= SHIP_STAT_RANGE_GENERATION_BONUS0;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS06;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS03;
			break; // Boston refit
	}
	}

	if (CheckAttribute(rRealShip, "savemBonus"))
	{
	switch (sti(rRealShip.savemBonus))
	{
		case 0:
			bonusHP		     		 -= SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusSpeed	      		 -= SHIP_STAT_RANGE_GENERATION_BONUS04;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS06;				
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS06;
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS06;
			break; // teak
		case 1:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS02;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS02;
			bonusWindAgainstSpeed    -= SHIP_STAT_RANGE_GENERATION_BONUS10;	
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS06;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS0;
			break; // White oak
		case 2:  
			bonusHP		     		 -= SHIP_STAT_RANGE_GENERATION_BONUS10;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS07;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS04;
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS15;	
			bonusCapacity            -= SHIP_STAT_RANGE_GENERATION_BONUS08;
			bonusOptCrew             -= SHIP_STAT_RANGE_GENERATION_BONUS04;  
			break; // fir
		case 3:  
			bonusHP		     		 -= SHIP_STAT_RANGE_GENERATION_BONUS03;
			bonusSpeed	      		 -= SHIP_STAT_RANGE_GENERATION_BONUS03;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS08;
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS06;	
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusOptCrew             -= SHIP_STAT_RANGE_GENERATION_BONUS05;
			break; // Mahagony
		case 4:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS04;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS06;
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS05;	
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS0;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS0;
			break; // Sabicu	
		case 5:  
			bonusHP		     		 -= SHIP_STAT_RANGE_GENERATION_BONUS07;
			bonusSpeed	      		 += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS04;
			bonusWindAgainstSpeed    -= SHIP_STAT_RANGE_GENERATION_BONUS04;	
			bonusCapacity            -= SHIP_STAT_RANGE_GENERATION_BONUS04;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS0;
			break; // Cedr 
		case 6:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusSpeed	      		 -= SHIP_STAT_RANGE_GENERATION_BONUS03;
			bonusTurnRate     		 -= SHIP_STAT_RANGE_GENERATION_BONUS08;
			bonusWindAgainstSpeed    += SHIP_STAT_RANGE_GENERATION_BONUS05;	
			bonusCapacity            += SHIP_STAT_RANGE_GENERATION_BONUS05;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS06;
			break; // larch 
		case 7:  
			bonusHP		     		 += SHIP_STAT_RANGE_GENERATION_BONUS10;
			bonusSpeed	      		 -= SHIP_STAT_RANGE_GENERATION_BONUS07;
			bonusTurnRate     		 += SHIP_STAT_RANGE_GENERATION_BONUS06;
			bonusWindAgainstSpeed    -= SHIP_STAT_RANGE_GENERATION_BONUS07;	
			bonusCapacity            -= SHIP_STAT_RANGE_GENERATION_BONUS08;
			bonusOptCrew             += SHIP_STAT_RANGE_GENERATION_BONUS07;
			break; // Swamp oak
	}
	}
	bonusHP *= 100.0;
	bonusSpeed *= 100.0;
	bonusTurnRate *= 100.0;
	bonusWindAgainstSpeed *= 100.0;
	bonusCapacity *= 100.0;
	bonusOptCrew *= 100.0;
	// Gregg: 0.06*100 во float = 5.9999 -> makeint давал «+5%» вместо 6%. Округляем к ближайшему с учётом знака.
	string returnvalue = "\n"+XI_ConvertString("BonusHull")+RoundSignedPercent(bonusHP)
	+"%\n"+XI_ConvertString("BonusSpeed")+RoundSignedPercent(bonusSpeed)
	+"%\n"+XI_ConvertString("BonusTurn")+RoundSignedPercent(bonusTurnRate)
	+"%\n"+XI_ConvertString("BonusWind")+RoundSignedPercent(bonusWindAgainstSpeed)
	+"%\n"+XI_ConvertString("BonusCapacity")+RoundSignedPercent(bonusCapacity)
	+"%\n"+XI_ConvertString("BonusCrew")+RoundSignedPercent(bonusOptCrew)+"%";
	return returnvalue;
}

float GetCharCapFontScale()
{
		if (CheckAttribute(&InterfaceStates, "CharCapFontScale")) return 1.0+(stf(InterfaceStates.CharCapFontScale)*2.0);
		else return 1.3;
}

float GetCharCapNotifScale()
{
	return 1.0+(stf(InterfaceStates.CharCapNotifScale)*2.0);
}

// LEO: бары ХП/энергии над врагами на суше — слайдеры в окне CharCap (raw 0..1 -> значение).
float GetEnemyBarSize()  // размер баров, диапазон 1..8, дефолт 4.0 (raw 0.4286)
{
	if (CheckAttribute(&InterfaceStates, "EnemyBarSize")) return 1.0+(stf(InterfaceStates.EnemyBarSize)*7.0);
	else return 4.0;
}
float GetEnemyBarDist()  // дальность видимости, диапазон 5..40, дефолт 20 (raw 0.4286)
{
	if (CheckAttribute(&InterfaceStates, "EnemyBarDist")) return 5.0+(stf(InterfaceStates.EnemyBarDist)*35.0);
	else return 20.0;
}
int GetEnemyBarMax()     // кол-во баров (целое), диапазон 8..128, дефолт 32 (raw 0.2)
{
	if (CheckAttribute(&InterfaceStates, "EnemyBarMax")) return makeint(8.0+(stf(InterfaceStates.EnemyBarMax)*120.0));
	else return 32;
}

float GetShipCapNotifDistance()
{
	return 100.0+(stf(InterfaceStates.ShipCapNotifDistance)*1900.0); // LEO: макс дальность всплывашек в море 1000->2000
}

float GetShipCapNotifScale()
{
	return 1.0+(stf(InterfaceStates.ShipCapNotifScale)*2.0);
}

float GetShipCapNamesScale()
{
	return 1.0+(stf(InterfaceStates.ShipCapNamesScale)*1.0);
}

float GetShipCapIconsScale()
{
	return 1.0+(stf(InterfaceStates.ShipCapIconsScale)*1.0);
}

void SetCharCapLevelColor(ref chrTarget)
{
	if(IsOfficer(chrTarget))
	{
		SendMessage(chrTarget, "ll", MSG_CHARACTER_SET_CAPTURE_TEXT1_FONT_COLOR, argb(255,255,255,255));
	}
	else
	{
		if (sti(chrTarget.rank) > sti(pchar.rank)+15)
		{
			SendMessage(chrTarget, "ll", MSG_CHARACTER_SET_CAPTURE_TEXT1_FONT_COLOR, argb(255,255,0,0));
		}
		else
		{
			SendMessage(chrTarget, "ll", MSG_CHARACTER_SET_CAPTURE_TEXT1_FONT_COLOR, argb(255,255,CalculateRankDifference(chrTarget),CalculateRankDifference(chrTarget)));
		}
	}
}

/*!
 * \brief Возвращает цвет, зависящий от отношения ранга персонажа и игрока
 * \param chrTarget персонаж
 * \return int цвет в формате ARGB
 */
int GetCharCapLevelColor(ref chrTarget)
{
	/* Для офицеров цвет всегда равен "стандартному" цвету */
	if(IsOfficer(chrTarget))
	{
		return argb(255,255,255,255);
	}
	/* Для большой разницы в сторону персонажа устанавливаем красный */
	if (sti(chrTarget.rank) > sti(pchar.rank)+15)
	{
		return argb(255,255,0,0);
	}
	/* Для небольшой разницы в сторону персонажа устанавливаем градиент от розового к красному */
	else if (sti(chrTarget.rank) >= sti(pchar.rank))
	{
		return argb(255,255,CalculateRankDifference(chrTarget),CalculateRankDifference(chrTarget));
	}
	/* Для разницы в сторону игрока устанавливаем серый */
	else
	{
		return argb(255,127,127,127);
	}
}

int CalculateRankDifference(ref chrTarget)
{
	if (stf(pchar.rank)-stf(chrTarget.rank)<-13) return 105;
	return 255-((sti(chrTarget.rank)-sti(pchar.rank))*15);
}

int GetParticlesCount()
{
	int iParticles = 64000;
	// LEO: потолок поднят до 200000 (движок: MAX_BILLBOARDS + 32-битные индексы) под массовые бои
	if (CheckAttribute(&InterfaceStates, "ParticlesCount")) iParticles = 4000 + makeint(stf(InterfaceStates.ParticlesCount) * 196000);
	return iParticles;
}
