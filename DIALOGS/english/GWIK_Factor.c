// The Company's factor - meeting him in person before the voyage to Fort Orange, the Jamaica office quest (Gregg)

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			if (CheckAttribute(pchar, "GWIK.Jamaica.FactorAboard")) // already agreed - he is "taking his things to the pier"
			{
				dialog.text = "My chests are already being carried to the pier, Captain. See you aboard.";
				link.l1 = "See you aboard.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Captain "+ GetFullName(pchar) +"? At last! "+ GetFullName(NPChar) +", factor of the Company, at your service - I am the one you are to carry to Fort Orange. Ten years I kept the Willemstad warehouses, and now the Heeren XVII have entrusted me with a whole office... I confess, I still can't believe it.";
			link.l1 = "Tell me about yourself. Who am I giving a cabin to?";
			link.l1.go = "Factor_About";
			link.l2 = "Pack your things - we sail at once.";
			link.l2.go = "Factor_Go";
		break;

		case "Factor_About":
			dialog.text = "All my battles are fought over ledgers: income, expenses, bills of lading. In those, mind you, I know no defeat. But I won't hide it, the voyage worries me: the competition has long arms, and the port whispers of an English corvette off Jamaica. I am no fighter, Captain. All my hope is in you and your gunners.";
			link.l1 = "My cannon aren't for show. Pack your things.";
			link.l1.go = "Factor_Go";
			link.l2 = "I'll make ready for the voyage and come back for you.";
			link.l2.go = "exit";
		break;

		case "Factor_Go":
			GWIK_JamaicaFactorBoard();
			dialog.text = "Agreed, Captain. My things will be packed within the quarter hour - I'll have them carried straight to the pier. To Fort Orange, then. God willing, we'll slip through quietly.";
			link.l1 = "We will. See you aboard.";
			link.l1.go = "exit";
		break;

		case "exit":
			if (!CheckAttribute(pchar, "GWIK.Jamaica.FactorAboard")) LAi_SetOwnerType(NPChar); // Gregg: sailing postponed - he stays standing and waiting (not a frozen actor)
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
