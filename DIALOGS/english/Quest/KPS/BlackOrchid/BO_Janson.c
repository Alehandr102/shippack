// Gregg: Janson and Frank, dock hands out of Le Fransua - «Чёрная Орхидея», глава 7 «Маленький ублюдок».
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
			dialog.text = "Good day to you, Cap.";
			link.l1 = "Good day.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Cap! Cap, hold on! Life and death, on my word!";
			link.l1 = "Speak.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Frank is a fool.";
			link.l1 = "I can see that without you. And?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "I was not finished! Frank is a fool, he went down into the crypt and he has not come out. There are skeletons down there, Cap. Three hours he has been in.";
			link.l1 = "Three hours. Then he is not coming out.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "He will! I can hear him from up here, he is singing. He is alive, Cap, alive, only there is nobody to fetch him.";
			link.l1 = "And what took him down there?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "The drink. We finished the loading in the morning and he started in. Cap, I am asking you. I will go on my knees if that is what it takes.";
			link.l1 = "It is not. Lead on.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelToDungeon");
		break;

		case "6":
			dialog.text = "Hah. A man. Hic.";
			link.l1 = "Is your name Frank?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Frank is me. Hic. And you are who.";
			link.l1 = "The one who came to haul you out.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "No need. It is good here. Quiet.";
			link.l1 = "There are skeletons here.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Hah! Skeletons. I will cut their bones off. Hic. All of them.";
			link.l1 = "...(smiles)... You cannot stand up.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "I can. Give me a shove.";
			link.l1 = "Listen, Frank. There is rum up above.";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "...(falls silent at once)... Where.";
			link.l1 = "At the cave mouth. I left it there.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "And why did you not say so. Hic.";
			link.l1 = "Are we going or not?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "We are going. You first, it is dark in here.";
			link.l1 = "To the way out.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelFromDungeon");
		break;

		case "14":
			dialog.text = "Cap, I... Thank you. I do not even know how to pay it back.";
			link.l1 = "You do not.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "There was a captain through here before you. I asked him too. He heard me out and walked on.";
			link.l1 = "It happens.";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "It happens, Cap, only I will remember this. Will you sit with us? For Frank coming back.";
			link.l1 = "I cannot. I have business and it will not wait.";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "What business? Maybe we can help.";
			link.l1 = "I doubt it.";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "Try us. We owe you, and I keep count of what I owe.";
			link.l1 = "I am looking for a heavy frigate. She put into Le Fransua for provisions and left.";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "...(brightens)... Why, it was Frank who loaded her! He would know.";
			link.l1 = "Frank? A dock hand out of Le Fransua?";
			link.l1.go = "20";
		break;

		case "20":
			dialog.text = "We both are. That day he loaded alone, my back was bad.";
			link.l1 = "And he saw where she went?";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "Seeing he did, telling he will not, not today. He will make something up. Come to the store in a couple of days, he will be a man again by then.";
			link.l1 = "I will ask him now all the same.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelTalkWithFrank");
		break;

		case "22":
			dialog.text = "Where is the rum. You said rum.";
			link.l1 = "Someone took it. I will buy more. Frank, did you load provisions onto that frigate?";
			link.l1.go = "23";
		break;

		case "23":
			dialog.text = "I loaded. Hic. Loaded meat. Loaded fish. Loaded water, water is heavy.";
			link.l1 = "What port did she sail for?";
			link.l1.go = "24";
		break;

		case "24":
			dialog.text = "I sailed for the tavern.";
			link.l1 = "Not you. The frigate.";
			link.l1.go = "25";
		break;

		case "25":
			dialog.text = "Ah. She went for rum.";
			link.l1 = "What rum, Frank?";
			link.l1.go = "26";
		break;

		case "26":
			dialog.text = "The ordinary sort. Hic. The kind in the tavern. Good rum.";
			link.l1 = "...(to himself: one last try)... Where. Did. She. Go.";
			link.l1.go = "27";
		break;

		case "27":
			dialog.text = "To port. Ships go from port to port. Did you not know?";
			link.l1 = "I knew. I will wait two days.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelSecondTalkWithJanson");
		break;

		case "28":
			dialog.text = "Well then, did you get it out of him?";
			link.l1 = "No. He told me about rum.";
			link.l1.go = "29";
		break;

		case "29":
			dialog.text = "I did warn you. Come to the Le Fransua store in two days. And bring rum, three bottles of it.";
			link.l1 = "I pulled him out of a cave and he wants rum from me?";
			link.l1.go = "30";
		break;

		case "30":
			dialog.text = "He is not asking, I am saying it in advance. I will explain to him who you are, maybe it will not be needed.";
			link.l1 = "Very well. I will bring it.";
			link.l1.go = "31";
		break;

		case "31":
			dialog.text = "That is good. Two days, Cap.";
			link.l1 = "Until then, Frank.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelLeFransuaStoreTimer");
		break;

		case "32":
			dialog.text = "...(gets up, not meeting your eye)... Captain. Janson told me everything.";
			link.l1 = "I am not here for apologies.";
			link.l1.go = "33";
		break;

		case "33":
			dialog.text = "I will say it anyway. While I am sober. In an hour I shall have forgotten your name again.";
			link.l1 = "Forgotten. I need an answer to one question.";
			link.l1.go = "34";
		break;

		case "34":
			dialog.text = "I know the one. The frigate sailed for Puerto Principe. I got talking with one of her hands while we loaded, and he said so.";
			link.l1 = "You are certain?";
			link.l1.go = "35";
		break;

		case "35":
			dialog.text = "Certain, Captain. You saved my life. I would not lie to you.";
			link.l1 = "Thank you, Frank.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("SmallMongrelToPuertoPrincipe");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
