void Telescope_Request()
{
	int nTelescopeIn = GetEventData();
	// LEO: нет надетой трубы — ничего не делаем (как при попытке смотреть в трубу с палубы без трубы)
	if( GetCharacterEquipByGroup(pchar,SPYGLASS_ITEM_TYPE)=="" ) {
		nTelescopeIn = 0;
	}
	if( nTelescopeIn != 0 )
	{
		// нажатие CTRL
		if( SeaCameras.Camera=="SeaDeckCamera" ) {
			// уже от 1-го лица / на марсе — обычная труба
			SendMessage(&objISpyGlass, "ll", MSG_TELESCOPE_REQUEST,1);
		}
		else if( SeaCameras.Camera=="SeaShipCamera" && bCanSwitchCameras ) {
			// LEO: быстрый вход в трубу от 3-го лица — перелёт на палубу (или марс по опции) + труба.
			//      Направление взгляда сохраняется блендом+InheritView (см. AICameras.c).
			QuickSpyglass_Enter();
		}
		else {
			SendMessage(&objISpyGlass, "ll", MSG_TELESCOPE_REQUEST,0);
		}
	}
	else
	{
		// отпускание CTRL (или нет трубы)
		SendMessage(&objISpyGlass, "ll", MSG_TELESCOPE_REQUEST,0);
		QuickSpyglass_Exit(); // вернёт 3-е лицо, если входили быстрым режимом
	}
}

void Telescope_Off()
{
	SendMessage(&objISpyGlass, "ll", MSG_TELESCOPE_REQUEST, 0);
}
