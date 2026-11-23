// LEO: Общие настройки жара стволов и схем огня; редактировать значения ниже.
// LEO: Применяются при InitParticles, входе в море и Sea_Reload, не каждый кадр.
// LEO: Без глобального object: старые значения из сейва не подменяют литералы.
// LEO: Enabled не трогаем: включение остаётся за меню и engine.ini.
void CannonHeatHaze_ApplyConfig()
{
    // LEO: Видимых пятен на весь кадр, не на корабль; 0 отключает добавление.
    // LEO: Пул и видимые линзы ограничены 512; текущий бюджет видимости — 128.
    SetCannonHeatHazeParam("maxSources", 512.0);
    SetCannonHeatHazeParam("maxVisible", 128.0);
    SetCannonHeatHazeParam("maxTraceTests", 32.0); // LEO: Пар лучей видимости за кадр, 0..512.
    SetCannonHeatHazeParam("lifetime", 1.8);       // LEO: Игровые секунды жизни, 0.05..3.
    SetCannonHeatHazeParam("radius", 1.2);         // LEO: Начальный радиус в метрах, 0.05..3.
    SetCannonHeatHazeParam("strength", 6.0);       // LEO: Амплитуда смещения в пикселях, 0..8.
    SetCannonHeatHazeParam("maxDistance", 500.0);  // LEO: Дальность от камеры в метрах, 1..500.
    SetCannonHeatHazeParam("distanceFade", 0.65);  // LEO: Начало затухания, доля maxDistance, 0..0.99.
    SetCannonHeatHazeParam("fadeIn", 0.08);        // LEO: Длительность появления, доля lifetime, 0.01..0.5.
    SetCannonHeatHazeParam("fadeOutStart", 0.55);  // LEO: Начало исчезновения, доля lifetime, 0.05..0.95.
    SetCannonHeatHazeParam("heightScale", 1.8);    // LEO: Отношение высоты к ширине, 1..3.
    SetCannonHeatHazeParam("expansion", 0.6);      // LEO: Прирост радиуса за lifetime, доля начального, 0..2.
    SetCannonHeatHazeParam("rise", 0.7);           // LEO: Подъём центра в м/с игрового времени, 0..3.
    SetCannonHeatHazeParam("forward", 0.75);       // LEO: Вынос от дульного среза вперёд в метрах, 0..3.
    SetCannonHeatHazeParam("minPixels", 2.0);      // LEO: Отсечь меньший проекционный радиус, пиксели, 0.5..16.
    SetCannonHeatHazeParam("maxPixels", 72.0);     // LEO: Предел горизонтального/вертикального полурадиуса, пиксели, 16..128.
    SetCannonHeatHazeParam("minRadiusPixels", 0.0); // LEO: Без раздувания вдали; явный минимум только для стволов, 0..32.
    SetCannonHeatHazeParam("frequency", 4.0);      // LEO: 1..32: рад/local малого огня, рад/метр для 7/8; больше = мельче рябь, не Гц.
    SetCannonHeatHazeParam("speed", 8.0);          // LEO: Скорость фазы, рад/с игрового времени, 0..60.

    // LEO: 1 = средний огонь, 2 = прежняя вспышка, 3 = свеча, 4 = факел, 5 = большой пожар; 0 убирает имя.
    // LEO: 6 = большой взрыв: шлейф на 3 секунды, полуширина 3 м, высота 9.6 м, дальность 300 м.
    // LEO: 7 — облако ShipExplode3 на 8 с: полуширина 18..25.2 м, высота = полуширина * 1.4.
    // LEO: 8 — огненный столб ShipFire2, 16x24 м; исчезает с эмиссией огня, а не с дымом.
    // LEO: Облако проявляется за 1 с, затухает с 5.6 до 8 с; сила min(12 px, общая * 1.5).
    // LEO: Постоянные профили имеют собственный радиус в метрах, без пиксельного минимума стволов.
    // LEO: Только точные имена, без масок и поиска fire в строке; дым и пыль не включать.
    ClearFireHeatSources();
    SetFireHeatSource("fire", 1);
    SetFireHeatSource("fire2", 1);
    SetFireHeatSource("fire_mix", 1);
    SetFireHeatSource("fire_from_hole", 1);
    SetFireHeatSource("torch", 4);
    SetFireHeatSource("candle", 3);
    SetFireHeatSource("ship_fire", 5);
    SetFireHeatSource("ship_fire_ext", 5);
    SetFireHeatSource("shipfire", 5);
    SetFireHeatSource("shipfire1", 5);
    SetFireHeatSource("shipfire2", 8);
    SetFireHeatSource("fortfire", 8);
    SetFireHeatSource("fortfire_net0", 8);
    SetFireHeatSource("fortfire_net1", 8);
    SetFireHeatSource("fortfire_net2", 8);
    SetFireHeatSource("gunfire", 4);
    SetFireHeatSource("grenade_expl", 7);
    SetFireHeatSource("pellet_expl", 7); // LEO: схема петарды содержит настоящее ExplosionFire, не только искры.
    SetFireHeatSource("ship_cannon_fire", 2);
    SetFireHeatSource("cancloud_fire", 2);
    SetFireHeatSource("cancloud_fire_big", 2);
    SetFireHeatSource("cancloud_fire_small", 2);
    SetFireHeatSource("cancloud_fire_old", 2);
    SetFireHeatSource("cancloud_fire_ext", 2);
    SetFireHeatSource("cancloud_fire_big_ext", 2);
    SetFireHeatSource("bombard", 2);
    SetFireHeatSource("shipexplode", 0);
    SetFireHeatSource("shipexplode2", 6);
    SetFireHeatSource("shipexplode3", 7);
    SetFireHeatSource("shipexplode_ext", 0);
    SetFireHeatSource("blast_big", 6);
    SetFireHeatSource("blast_bomb", 2);
    SetFireHeatSource("blast_bomb_big", 6);
    SetFireHeatSource("blast_bomb_ptr", 2);
    SetFireHeatSource("blast_inv", 2);
    SetFireHeatSource("blast_inv_ext", 2);
}
