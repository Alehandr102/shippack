// LEO: Запрет операций по сохранённым сцепкам, в том числе на суше.
bool SeaTowOperations_IsLinked(ref chr)
{
	int index = CompanionTravel_DepartureIndex(chr);
	if (index < 0) return false;
	ref ship = GetCharacter(index);
	if (LAi_IsDead(ship)) return false;
	if (!CheckAttribute(pchar, "SeaTow.Links")) return false;
	aref links, link, tug, prize;
	makearef(links, pchar.SeaTow.Links);
	for (int i = 0; i < GetAttributesNum(links); i++)
	{
		link = GetAttributeN(links, i);
		if (!CheckAttribute(link, "Tug") || !CheckAttribute(link, "Prize")) continue;
		makearef(tug, link.Tug);
		makearef(prize, link.Prize);
		if (!SeaTow_ValidEndpoint(tug) || !SeaTow_ValidEndpoint(prize)) continue;
		if (LAi_IsDead(GetCharacter(sti(tug.Index))) || LAi_IsDead(GetCharacter(sti(prize.Index)))) continue;
		if (SeaTow_Matches(tug, ship) || SeaTow_Matches(prize, ship)) return true;
	}
	return false;
}

void SeaTowOperations_Warn()
{
	Log_InfoColoredAnim("red", XI_ConvertString("SeaTowOperationsWarning"));
	Log_Info(XI_ConvertString("SeaTowOperationsDetach"));
}

bool SeaTowOperations_Block(ref chr)
{
	if (!SeaTowOperations_IsLinked(chr)) return false;
	SeaTowOperations_Warn();
	return true;
}

// LEO: Обе стороны проверяем до обмена; даже при двух буксирах предупреждение выводится один раз.
bool SeaTowOperations_BlockSwap(ref first, ref second)
{
	if (!SeaTowOperations_IsLinked(first) && !SeaTowOperations_IsLinked(second)) return false;
	SeaTowOperations_Warn();
	return true;
}

// LEO: Сообщение после успешного зацепа.
void SeaTowOperations_Attached(ref tug, ref prize)
{
	int hull = SeaTow_Hull(prize);
	if (hull < 0) return;
	Log_InfoColored("green", XI_ConvertString(RealShips[hull].BaseName) + " " + XI_ConvertString("SeaTowOperationsAttached"));
}
