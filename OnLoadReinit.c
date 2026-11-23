//Boyer mod #20170301-6

// Gregg: подсистемы миграций (UpdateGameDataOnLoad с версия-шагами и extern-блок под
// неё) здесь больше нет. Она дописывала новый контент в хвост Items/Locations на уже
// начатых партиях - каталоги приходят из сейва позиционно, мимо InitItems/InitLocations.
// Мод рассчитан на новую игру: весь контент заводится штатными инитами, а старые сейвы
// отсекаются несовпадением VERSION_NUM (_mod_on_off.h).
// Заодно убрана onLoadReInit() - мёртвый код, её не звали ни скрипты, ни движок.
// Здесь остались только живые хелперы: ClearAllFantomShips (зовётся из reinit.c) и её
// спутник RemoveShipFromShipyard. Остальное - storeCabinMoney, setCurrentCabinLocIndex,
// resetCabinMoney, storeOldShipType, resetRealShipType, fixChurchNightLocks,
// GetShipIdxByName - висело только на ней и никем не вызывалось.

//Borrowed from GOF reinit.c
void ClearAllFantomShips()
{
	int  i;
	ref  chr;
	for (i = 0; i < REAL_SHIPS_QUANTITY; i++)
	{
        	DeleteAttribute(&RealShips[i], "lock");
	}
	RealShips[sti(Characters[BOAT_CHARACTER].ship.type)].lock = true;
	// LEO: фикс утечки кораблей ПУ/аукциона. Было `i < MAX_CHARACTERS`; на загрузке он = globalCharacters, а сторожа
	//   ПУ/аукциона висят выше, в пробеле [MAX_CHARACTERS, 1700) - их корабль не лочился, GC стирал слот, энкаунтер
	//   переиспользовал (Синяя Птица → бой как «error»). Сканим [0, FANTOM_CHARACTERS); id!="0" - пустые слоты не лочим.
	for (i = 0; i < FANTOM_CHARACTERS; i++)
	{
        	chr = &Characters[i];
        	if (chr.id != "0" && CheckAttribute(chr, "ship.type") && chr.ship.type != SHIP_NOTUSED)
        	{
            		RealShips[sti(chr.ship.type)].lock = true;
        	}
		if (CheckAttribute(chr, "shipyard"))
		{
		   RemoveShipFromShipyard(chr);
		}
	}
	for (i = FANTOM_CHARACTERS; i < TOTAL_CHARACTERS; i++)
	{
        	chr = &Characters[i];
        	if (CheckAttribute(chr, "ship.type") && chr.ship.type != SHIP_NOTUSED)
        	{
            	RealShips[sti(chr.ship.type)].lock = true;
        	}
	}
	for (i = 0; i < REAL_SHIPS_QUANTITY; i++)
	{
	// LEO: StoreShip не проверяем - все верфи выше уже очищены RemoveShipFromShipyard, а флаг, оставшийся без
	//   ссылки из верфи, раньше держал слот вечно (переживал каждую загрузку)
	if (!CheckAttribute(&RealShips[i], "lock") && !CheckAttribute(&RealShips[i], "HiredPrize"))  // LEO: HiredPrize - слот приза наёмного пирата (переживает GC на загрузке, пока капер-однодневка мёртв)
        	{
			DeleteAttribute(&RealShips[i], "");
		}
	}
}

void RemoveShipFromShipyard(ref NPChar)
{
        aref   arDest, arImt;
	string sAttr;
	int    iNum, i, iShip;
	makearef(arDest, NPChar.shipyard);
	iNum = GetAttributesNum(arDest);
	for (i = 0; i < iNum; i++)
	{
	    arImt = GetAttributeN(arDest, i);
	    iShip = sti(arImt.Type);
	    if (iShip >= 0 && iShip < REAL_SHIPS_QUANTITY) DeleteAttribute(&RealShips[iShip], "StoreShip");
	}
        DeleteAttribute(NPChar, "shipyard");
}
