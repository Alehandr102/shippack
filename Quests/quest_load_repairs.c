// LEO: только существующий одноразовый таймер; чужие условия и завершённые узлы не перевзводим.
void BO_RepairSavedWait(string sName, int iOldDays, int iNewDays)
{
    if (!CheckAttribute(PChar, "quest." + sName)) return;
    aref arQuest;
    makearef(arQuest, PChar.quest.(sName));
    if (CheckAttribute(arQuest, "over") || CheckAttribute(arQuest, "again")) return;
    if (!CheckAttribute(arQuest, "function")) return;
    if (arQuest.function != sName) return;
    if (!CheckAttribute(arQuest, "win_condition.l1")) return;
    if (arQuest.win_condition.l1 != "Timer") return;
    if (CheckAttribute(arQuest, "win_condition.l2")) return;
    if (CheckAttribute(arQuest, "win_condition.l1.date.hour")) return;
    if (!CheckAttribute(arQuest, "win_condition.l1.date.year")) return;
    if (!CheckAttribute(arQuest, "win_condition.l1.date.month")) return;
    if (!CheckAttribute(arQuest, "win_condition.l1.date.day")) return;

    int iYear = sti(arQuest.win_condition.l1.date.year);
    int iMonth = sti(arQuest.win_condition.l1.date.month);
    int iDay = sti(arQuest.win_condition.l1.date.day);
    if (iYear < 1 || iMonth < 1 || iMonth > 12) return;
    if (iDay < 1 || iDay > GetMonthDays(iMonth, iYear)) return;

    // LEO: короткое ожидание может быть уже новым; без даты старта его не отличить от старого.
    int iLimit = GetAddingDataYear(0, 0, iNewDays) * 10000 +
                 GetAddingDataMonth(0, 0, iNewDays) * 100 + GetAddingDataDay(0, 0, iNewDays);
    if (iYear * 10000 + iMonth * 100 + iDay <= iLimit) return;

    // LEO: сдвиг исходного дедлайна сохраняет прошедшие дни; часы мира не меняются.
    for (int i = 0; i < iOldDays - iNewDays; i++)
    {
        iDay--;
        if (iDay < 1)
        {
            iMonth--;
            if (iMonth < 1)
            {
                iMonth = 12;
                iYear--;
            }
            iDay = GetMonthDays(iMonth, iYear);
        }
    }
    int iToday = GetDataYear() * 10000 + GetDataMonth() * 100 + GetDataDay();
    if (iYear * 10000 + iMonth * 100 + iDay < iToday)
    {
        iYear = GetDataYear();
        iMonth = GetDataMonth();
        iDay = GetDataDay();
    }
    // LEO: просроченный квест выполнит штатная проверка после загрузки, не этот хук.
    arQuest.win_condition.l1.date.year = iYear;
    arQuest.win_condition.l1.date.month = iMonth;
    arQuest.win_condition.l1.date.day = iDay;
}

void BO_RepairQuestRewardsAndWaits()
{
    int iItem;
    // LEO: END бывает и при провале банка; победу доказывает только отметка энциклопедии.
    bool bPawEarned = false;
    if (CheckAttribute(PChar, "questdata.CaptainMonkey"))
    {
        bPawEarned = sti(PChar.questdata.CaptainMonkey) == 1;
    }
    if (CheckAttribute(PChar, "CaptainMonkey"))
    {
        if (PChar.CaptainMonkey == "END" && bPawEarned)
        {
            iItem = FindItem("MaugliPaw");
            if (iItem >= 0)
            {
                if (CheckAttribute(&Items[iItem], "price"))
                {
                    if (sti(Items[iItem].price) == 0) Items[iItem].price = 15000;
                }
            }
        }
    }
    if (CheckAttribute(PChar, "SpanishGenerosity"))
    {
        // LEO: эти состояния достижимы только после возврата оригинала; FAIL бывает после засады.
        string sStage = PChar.SpanishGenerosity;
        bool bReturned = sStage == "6" || sStage == "8" || sStage == "9" || sStage == "10" ||
                         sStage == "11" || sStage == "12" || sStage == "13" || sStage == "14" ||
                         sStage == "15" || sStage == "16" || sStage == "END" || sStage == "FAIL";
        if (bReturned)
        {
            iItem = FindItem("BlackOrchid");
            if (iItem >= 0)
            {
                if (CheckAttribute(&Items[iItem], "price"))
                {
                    if (sti(Items[iItem].price) == 0) Items[iItem].price = 40000;
                }
            }
        }
    }

    // LEO: один проход на сейв, включая ещё не начатые главы; цены проверяются независимо от этой отметки.
    if (CheckAttribute(PChar, "questTemp.BO_Waits1093Migrated")) return;
    if (!CheckAttribute(PChar, "SmallMongrel")) BO_RepairSavedWait("_SmallMongrelStart", 60, BO_SM_START_DAYS);
    if (!CheckAttribute(PChar, "RaulEsperanto")) BO_RepairSavedWait("RaulEsperantoTime", 30, BO_SM_NEXT_DAYS);
    if (!CheckAttribute(PChar, "FindAndDestroy")) BO_RepairSavedWait("FindAndDestroyTime", 45, BO_RE_NEXT_DAYS);
    if (!CheckAttribute(PChar, "TwoSisters")) BO_RepairSavedWait("TwoSistersTimeEnd", 45, BO_FD_NEXT_DAYS);
    PChar.questTemp.BO_Waits1093Migrated = true;
}
