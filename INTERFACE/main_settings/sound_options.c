string SoundOptionsList[50];
int SoundOptionsQty = 0;

void GetSoundOptionsList()
{
	string s;
	SoundOptionsQty = 0;
	for(int i = 0; i < 50; i++)
	{
		s = "";
		switch(i)
		{
			case 0: s = "EnabledAltSoundsGun"; break;
			// StreamerMode вынесен из таблицы в отдельный радио-переключатель (STREAMERMODE_CHECKBOX) в окне звука
		}		
		SoundOptionsList[SoundOptionsQty] = s;
		if(s != "") SoundOptionsQty++;
	}
}

void ClickSoundOptions() {
	ClickOption("TABLE_SOUND_OPTIONS");
}

void RightClickSoundOptions() {
	RightClickOption("TABLE_SOUND_OPTIONS");
}