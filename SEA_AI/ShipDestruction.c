object ShipDestructionConfig;
object ShipDebrisConfig;

// LEO: Общие настройки обломков; индивидуальные Ship.Debris имеют приоритет, секунды и м/с.
void ShipDebris_InitCharacter(ref chr)
{
    // LEO: Меняем лишь прежние штатные значения; настроенные пользователем импульсы не перезаписываем.
    if (!CheckAttribute(ShipDebrisConfig, "BlastPhysicsVersion"))
    {
        if (CheckAttribute(ShipDebrisConfig, "BlastSpeed") && stf(ShipDebrisConfig.BlastSpeed) == 2.5) ShipDebrisConfig.BlastSpeed = 6.0;
        if (CheckAttribute(ShipDebrisConfig, "BlastLift") && stf(ShipDebrisConfig.BlastLift) == 1.2) ShipDebrisConfig.BlastLift = 2.5;
        if (CheckAttribute(ShipDebrisConfig, "BlastSpin") && stf(ShipDebrisConfig.BlastSpin) == 0.35) ShipDebrisConfig.BlastSpin = 0.7;
        ShipDebrisConfig.BlastPhysicsVersion = 1;
    }
    if (!CheckAttribute(chr, "Ship.Debris.BlastPhysicsVersion"))
    {
        if (CheckAttribute(chr, "Ship.Debris.BlastSpeed") && stf(chr.Ship.Debris.BlastSpeed) == 2.5) chr.Ship.Debris.BlastSpeed = 6.0;
        if (CheckAttribute(chr, "Ship.Debris.BlastLift") && stf(chr.Ship.Debris.BlastLift) == 1.2) chr.Ship.Debris.BlastLift = 2.5;
        if (CheckAttribute(chr, "Ship.Debris.BlastSpin") && stf(chr.Ship.Debris.BlastSpin) == 0.35) chr.Ship.Debris.BlastSpin = 0.7;
        chr.Ship.Debris.BlastPhysicsVersion = 1;
    }
    if (!CheckAttribute(ShipDebrisConfig, "SailHoldSeconds")) ShipDebrisConfig.SailHoldSeconds = 60.0;
    if (!CheckAttribute(ShipDebrisConfig, "SailCollapseSeconds")) ShipDebrisConfig.SailCollapseSeconds = 8.0;
    if (!CheckAttribute(ShipDebrisConfig, "SinkSpeed")) ShipDebrisConfig.SinkSpeed = 0.6;
    if (!CheckAttribute(ShipDebrisConfig, "Gravity")) ShipDebrisConfig.Gravity = 9.81;
    if (!CheckAttribute(ShipDebrisConfig, "AirDrag")) ShipDebrisConfig.AirDrag = 0.15;
    if (!CheckAttribute(ShipDebrisConfig, "MaxFallSpeed")) ShipDebrisConfig.MaxFallSpeed = 12.0;
    if (!CheckAttribute(ShipDebrisConfig, "CollisionSeconds")) ShipDebrisConfig.CollisionSeconds = 5.0;
    if (!CheckAttribute(ShipDebrisConfig, "SinkDepth")) ShipDebrisConfig.SinkDepth = 20.0;
    if (!CheckAttribute(ShipDebrisConfig, "BlastSpeed")) ShipDebrisConfig.BlastSpeed = 6.0;
    if (!CheckAttribute(ShipDebrisConfig, "BlastLift")) ShipDebrisConfig.BlastLift = 2.5;
    if (!CheckAttribute(ShipDebrisConfig, "BlastSpin")) ShipDebrisConfig.BlastSpin = 0.7;
    if (!CheckAttribute(ShipDebrisConfig, "CollisionTraceBudget")) ShipDebrisConfig.CollisionTraceBudget = 64;
    if (!CheckAttribute(ShipDebrisConfig, "MidMastFracture")) ShipDebrisConfig.MidMastFracture = 1;
    if (!CheckAttribute(ShipDebrisConfig, "MidMastFractureChance")) ShipDebrisConfig.MidMastFractureChance = 0.6;
    if (!CheckAttribute(ShipDebrisConfig, "MidMastCutFraction")) ShipDebrisConfig.MidMastCutFraction = 0.5;
    if (!CheckAttribute(chr, "Ship.Debris")) chr.Ship.Debris = "";
    aref settings; makearef(settings, chr.Ship.Debris);
    for (int i = 0; i < GetAttributesNum(ShipDebrisConfig); i++)
    {
        aref value = GetAttributeN(&ShipDebrisConfig, i);
        string key = GetAttributeName(value);
        if (!CheckAttribute(settings, key)) settings.(key) = GetAttributeValue(value);
    }
}

// LEO: Новый живой корабль не наследует фазу и начало координат прежнего обломка.
void ShipDestruction_InitCharacter(ref chr, bool restoring)
{
    // LEO: При загрузке bDead ещё может быть не восстановлен; мёртвый корабль сохраняет фазу и при возврате в сцену.
    if (restoring || LAi_IsDead(chr)) return;
    DeleteAttribute(chr, "Ship.Destruction.Active");
    DeleteAttribute(chr, "Ship.Destruction.Fatal");
    DeleteAttribute(chr, "Ship.Destruction.DetonationHandled");
    DeleteAttribute(chr, "Ship.Destruction.Elapsed");
    DeleteAttribute(chr, "Ship.Destruction.Status");
    DeleteAttribute(chr, "Ship.Destruction.BudgetActive");
    DeleteAttribute(chr, "Ship.Destruction.BudgetVertices");
    DeleteAttribute(chr, "Ship.Destruction.BudgetRejected");
    for (int i = 0; i < 16; i++) DeleteAttribute(chr, "Ship.Destruction.Origin" + i);
}

// LEO: Переключатель берём из скрипта, а не из старого сейва; 0 отключает новые разломы.
void ShipDestruction_InitConfig()
{
    ShipDestructionConfig.Experimental = 1;
    if (!CheckAttribute(ShipDestructionConfig, "Lifetime")) ShipDestructionConfig.Lifetime = 30.0;
    if (!CheckAttribute(ShipDestructionConfig, "Lift")) ShipDestructionConfig.Lift = 3.0;
    if (!CheckAttribute(ShipDestructionConfig, "LiftTime")) ShipDestructionConfig.LiftTime = 2.0;
    if (!CheckAttribute(ShipDestructionConfig, "SpreadSpeed")) ShipDestructionConfig.SpreadSpeed = 0.8;
    if (!CheckAttribute(ShipDestructionConfig, "TurnRate")) ShipDestructionConfig.TurnRate = 0.08;
    if (!CheckAttribute(ShipDestructionConfig, "SinkSpeed")) ShipDestructionConfig.SinkSpeed = 1.8;
}

// LEO: Вызывать только для крупного взрыва: мачты всегда, разлом корпуса один раз с шансом 25%.
void ShipDestruction_FatalDetonation(ref chr)
{
    ShipDestruction_InitConfig();
    bool bDead = LAi_IsDead(chr);
    if (!bDead || !CheckAttribute(chr, "Ship.HP")) return;
    if (stf(chr.Ship.HP) > 0.0) return;
    if (CheckAttribute(chr, "Ship.Destruction.DetonationHandled"))
    {
        if (sti(chr.Ship.Destruction.DetonationHandled) == 1) return;
    }
    // LEO: Нативная загрузка обходит Ship_Add2Sea; обновляем параметры до взрыва.
    ShipDebris_InitCharacter(chr);
    chr.Ship.Destruction.DetonationHandled = 1;
    SendMessage(chr, "ll", MSG_SHIP_BROKE_ALL_MUST, 1);
    if (sti(ShipDestructionConfig.Experimental) != 1) return;
    if (CheckAttribute(chr, "Ship.Destruction.Active"))
    {
        if (sti(chr.Ship.Destruction.Active) == 1) return;
    }
    if (rand(3) != 0) return;
    chr.Ship.Destruction.Experimental = 1;
    chr.Ship.Destruction.Fatal = 1;
    chr.Ship.Destruction.Lifetime = ShipDestructionConfig.Lifetime;
    chr.Ship.Destruction.Lift = ShipDestructionConfig.Lift;
    chr.Ship.Destruction.LiftTime = ShipDestructionConfig.LiftTime;
    chr.Ship.Destruction.SpreadSpeed = ShipDestructionConfig.SpreadSpeed;
    chr.Ship.Destruction.TurnRate = ShipDestructionConfig.TurnRate;
    chr.Ship.Destruction.SinkSpeed = ShipDestructionConfig.SinkSpeed;
}
