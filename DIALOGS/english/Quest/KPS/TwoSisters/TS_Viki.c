// Gregg: Viki Henderson and her sister Diana - "Two Sisters", the Martinique branch.
// Logic - Program\Quests\reaction_functions\twosisters.c.

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
			dialog.text = "Do you have the ring?";
			link.l1 = "Not yet.";
			link.l1.go = "exit";
		break;

		// The brothel in Fort de France
		case "1":
			dialog.text = "Is the captain looking for something to do?";
			link.l1 = "The captain is looking for a woman willing to move.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "...(a short laugh)... To move. Usually they ask for something else. And where to?";
			link.l1 = "San Juan, Puerto Rico. A house is opening there, and they need women.";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "San Juan. ...(after a pause)... And why me? This place is full of us.";
			link.l1 = "I was told you are the best on Martinique. Were they lying?";
			link.l1.go = "4";
			link.l2 = "Because you are the first who spoke to me.";
			link.l2.go = "4";
		break;

		case "4":
			dialog.text = "They were not. ...(no smile)... Very well, I will go. But not for nothing, and not at once.";
			link.l1 = "How much?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Not money. There is something I cannot do myself.";
			link.l1 = "Say it.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Kill my sister. Diana Henderson, captain of the brigantine Amazon. She lies at Puerto Principe, on Cuba.";
			link.l1 = "...(a pause)... Your sister?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "My sister. Blood, elder, and the only one.";
			link.l1 = "Is there a reason?";
			link.l1.go = "8";
			link.l2 = "What you said is enough for me. But I want to know why.";
			link.l2.go = "8";
		break;

		case "8":
			dialog.text = "There is. Whether I tell it to you is another matter.";
			link.l1 = "You will tell it, or I do not go.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "...(after a silence)... We came here together, to relatives. The relatives were gone. She had the money, she bought the ship.";
			link.l1 = "And you?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "And I ended up here. For two years she gave me money and told me every time how ashamed of me she was. Then she stopped giving. The shame stayed.";
			link.l1 = "And for that you would have her killed?";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "While she lives I go nowhere. She would find me in San Juan too, and write to everyone she can reach about what I am.";
			link.l1 = "She is your sister.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "She was, for twenty-five years. ...(evenly)... Now she is a woman with a ship, money and a good name.";
			link.l1 = "...(say nothing)...";
			link.l1.go = "13";
		break;

		case "13":
			LAi_Fade("", "");
			dialog.text = "I want her dead. That is the only price, and there will not be another.";
			link.l1 = "Are you certain?";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "I am.";
			link.l1 = "How do I prove it to you?";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "She wears the family ring on her left hand. Take it off and bring it to me.";
			link.l1 = "Very well.";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "Then until we meet, captain. I am in no hurry.";
			link.l1 = "...(leave)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersVikiQuestStart");
		break;

		// Diana at Puerto Principe, after the fight with the hired man
		case "17":
			dialog.text = "...(sheathing her blade)... Hardly the place for a stroll. Who are you?";
			link.l1 = "I am looking for Diana Henderson.";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "You have found her. Go on.";
			link.l1 = "Your sister sent me.";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "...(without surprise)... Money again?";
			link.l1 = "No.";
			link.l1.go = "20";
		break;

		case "20":
			dialog.text = "What then? ...(looks closer)... Though I can guess. That is why you are standing there with a blade.";
			link.l1 = "She asked me to kill you.";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "She did. ...(flatly)... I expected some spite from her, but not this.";
			link.l1 = "She says you left her no choice.";
			link.l1.go = "22";
		break;

		case "22":
			dialog.text = "I paid for her for two years. I offered her a berth on my ship, more than once.";
			link.l1 = "And she?";
			link.l1.go = "23";
		break;

		case "23":
			dialog.text = "She got used to living on someone else. When I stopped giving, it turned out I was to blame for everything.";
			link.l1 = "You could have talked to her.";
			link.l1.go = "24";
		break;

		case "24":
			dialog.text = "I did. Every time. ...(a wry look)... You tried it too, I see.";
			link.l1 = "I do not want to kill you.";
			link.l1.go = "25";
			link.l2 = "I was paid for your death.";
			link.l2.go = "25";
		break;

		case "25":
			dialog.text = "Whether you want it no longer matters. You came here with this, and I will not simply let you walk away.";
			link.l1 = "Then let us finish it.";
			link.l1.go = "26";
		break;

		case "26":
			dialog.text = "Let us.";
			link.l1 = "...(draw the blade)...";
			link.l1.go = "27";
		break;

		case "27":
			dialog.text = "...";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersVikiFirstFightWithDiana");
		break;

		// Diana is beaten down
		case "28":
			dialog.text = "...(breathing hard)... Well?";
			link.l1 = "Get up.";
			link.l1.go = "29";
		break;

		case "29":
			dialog.text = "Why did you not finish it?";
			link.l1 = "Give me the ring and I will tell her it is done.";
			link.l1.go = "30";
		break;

		case "30":
			dialog.text = "...(slowly)... You mean you would lie to her. She would rest easy, and I would stay alive.";
			link.l1 = "That is how it would be.";
			link.l1.go = "31";
		break;

		case "31":
			dialog.text = "Hand over the family ring so that it ends up with her? No.";
			link.l1 = "It is your life against a piece of gold.";
			link.l1.go = "32";
		break;

		case "32":
			dialog.text = "Against it, then. ...(raises her blade)... You will take the ring off a corpse.";
			link.l1 = "...(raise your weapon)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersVikiNextFightWithDiana");
		break;

		// Viki takes the ring
		case "33":
			dialog.text = "Did you bring it?";
			link.l1 = "...(hand over the ring)...";
			link.l1.go = "34";
		break;

		case "34":
			TakeNItems(PChar, "TwoSisters_Ring", -1);
			dialog.text = "...(she looks at the ring for a long time)... So it is done.";
			link.l1 = "You are going to San Juan.";
			link.l1.go = "35";
		break;

		case "35":
			dialog.text = "I am. ...(she puts the ring away)... And not another word about this, captain. Ever.";
			link.l1 = "To the ship, then.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersVikiQuestComplete");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
