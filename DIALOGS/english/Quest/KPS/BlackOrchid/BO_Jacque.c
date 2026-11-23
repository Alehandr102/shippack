// Gregg: Jacque, the vagrant in George Bryan's house - the "Black Orchid" trilogy
// (ported from Damned Destiny). Logic - Program\Quests\reaction_functions\blackorchid.c
// Nodes 1-7 - downstairs by the chest, three ways to send him up; 8-15 - upstairs,
// where he remembers the business with the lost key.

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
			NextDiag.TempNode = "First time";
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Three hundred lads for a keg of wine! Hic... Or how does it go? Hey, and who might you be?";
			link.l1 = "And who are you? What are you doing in George Bryan's house?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Hic... And what business is it of yours? Off with you! Hic...";
			link.l1 = "I am a friend of Bryan's. So what are you doing in his house?";
			link.l1.go = "3";
			link.l2 = "Bryan forgot to return something of mine, and I need it badly. I shall look around, if you do not mind?";
			link.l2.go = "6";
			link.l3 = "Hold your tongue, you wreck. I came to take what is mine. Squeak once more and it will be the worse for you.";
			link.l3.go = "7";
		break;

		case "3":
			dialog.text = "A friend? Of Bryan's? Why, I am a friend of Bryan's too. Hic... Let us be acquainted. Jacque. Just Jacque.";
			link.l1 = "You did not answer me. What are you doing in his house?";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Well, see... he offered it himself, to stay while he is away. I have been here a good while. Not a bad little house. And what brings you?";
			link.l1 = "To take one thing back. Bryan's memory is a sieve, he forgot to return it. May I look?";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Look away. And I shall go and lie down, I am very tired.";
			link.l1 = "A sound decision. Go and sleep it off, and I shall search.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactInHouseGeorgeBryanJacqueToRoom");
		break;

		case "6":
			dialog.text = "Go on, then. Hic... And I shall sleep.";
			link.l1 = "Splendid.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactInHouseGeorgeBryanJacqueToRoom");
		break;

		case "7":
			dialog.text = "What?! We shall see who is the wreck here. Hic... Draw your blade! What, lost your nerve? Just so. You are all great with your tongues, but when it comes to fighting there is nobody. Hic... I may have had a drop too much. I shall go and sleep.";
			link.l1 = "High time. Off with you.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactInHouseGeorgeBryanJacqueToRoom");
		break;

		case "8":
			dialog.text = "Hic... Now what? I had just settled down, and here you are.";
			link.l1 = "Do you know where the key to the chest downstairs is? What I need is in there, and without the key it will not open.";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "I cannot help you, sorry. Bryan lost that key.";
			link.l1 = "Lost it? How long ago?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "I do not recall... No, wait, I have it. Two weeks ago to the day. That day Bryan and I started a fine brawl in the tavern, they will remember it a long while. And afterwards Bryan missed the key. He wanted to look for it, but the keeper shoved him out the door. Bryan took offence and went for a musketeer.";
			link.l1 = "A musketeer? And then?";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "Nothing. He laid Bryan out with the butt of his musket. Bryan lay by the door till morning.";
			link.l1 = "Perhaps the keeper found the key? Or the waitress?";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "The keeper did not, that is certain - we would have heard. And the waitress hid in a corner at once and did not put her head out until it was over. She could not have found it.";
			link.l1 = "And later, when she was cleaning up?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "No, the keeper cleaned up alone. He sent her home straight away, she was frightened half to death.";
			link.l1 = "Might one of the customers have picked it up?";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "What are you at me for with your key! I want to sleep.";
			link.l1 = "Ha. You sleep well enough as it is. At least you made it to the bed.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "I am well enough as I am. Enough, you have worn me out. Let me sleep!";
			link.l1 = "All right, all right, I am going.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToPortPaxTavern");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
