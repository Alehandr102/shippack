// Gregg: The Tortuga commandant, the patrol, the false mayor and his daughter - «Чёрная Орхидея», глава 9 «Найти и уничтожить».
// Логика - Program\Quests\reaction_functions\blackorchid.c.

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch (Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "Move along.";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "...(looks the pair of you over)... Two beggars in a locked house. Curious.";
			link.l1 = "We sleep here. The house is empty.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Empty it may be, but it is not nobody. This house is mine, and it was let a fortnight ago. ...(steps closer)... Show me your hands.";
			link.l1 = "...(holds out his palms)... Look, if you must.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "...(looks a long while, then steps back)... Wrong calluses. Very well. Be gone by morning.";
			link.l1 = "We will go.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyDialogInShelterEnd");
		break;

		case "4":
			dialog.text = "Where to.";
			link.l1 = "The town hall. To beg.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "...(smirks)... Beg away, nobody is stopping you. Only inside one at a time and in silence. The other waits out here.";
			link.l1 = "He will wait.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyReloadToTownhall");
		break;

		case "7":
			dialog.text = "...(turns round)... Ah, it is you. Your hands were wrong after all.";
			link.l1 = "You knew back at the house.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyFirstBattleInTownhall");
		break;

		case "8":
			dialog.text = "...(speaks evenly, without blinking)... You have come to the wrong door, Captain. This is the governor residence.";
			link.l1 = "I know. And I know you are not the governor.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "I am the governor of Tortuga. Ask anyone in this town.";
			link.l1 = "I will ask somebody beyond the gate. They still answer in their own words there.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "...(his tone changes for the first time)... You will not.";
			link.l1 = "I want the key your daughter wears.";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "...(his tone changes at last)... So that is what you are here for. She does not know what she wears, and she will not learn. She will not be carried out of here.";
			link.l1 = "...(draws)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyBattleWithGovernor");
		break;

		case "13":
			dialog.text = "...(on the floor, holding his side)... Upstairs. At her neck, on a cord. It is neither mine nor yours.";
			link.l1 = "Whose, then?";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "The one who comes for both keys. ...(closes his eyes)... The girl has done nothing. Take her out of here, do you hear.";
			link.l1 = "Upstairs.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyToTownhallRoom");
		break;

		case "15":
			dialog.text = "...(presses into the corner)... Do not come near. Please.";
			link.l1 = "I am not here for you. You are being taken out.";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "Father... He is not my father, is he? I understood on the third day and there was nobody to tell.";
			link.l1 = "You understood rightly. And one thing more: take the key off your neck, I need it. Come.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyInTownhallRoomDialog");
		break;

		case "17":
			dialog.text = "...(in the doorway, at a loss)... The mayor ordered... and there is no mayor.";
			link.l1 = "There is no mayor any more. Go, and hinder nobody leaving.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyRoomOfficersOut");
		break;

		case "21":
			dialog.text = "Captain. We are not detaining you. ...(steps out of the way)... In truth, we thank you.";
			link.l1 = "Look after the girl.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("FindAndDestroyExitTortuga");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
